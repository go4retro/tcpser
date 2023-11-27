/* Replacement termios functions for MinGW32.  These versions of termios
   functions serialize over a pipe to another process, which adjusts
   the terminal.

   Copyright (C) 2008 CodeSourcery, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include <windows.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

#include "mingw-termios.h"

static HANDLE pipe_handle = INVALID_HANDLE_VALUE;
static HANDLE pipe_mutex = INVALID_HANDLE_VALUE;

static int
link_to_wrapper (void)
{
  static int tried = 0;

  if (!tried)
    {
      char pipe_name[256];

      tried = 1;

      /* Connect to the named pipe for communication with the wrapper.  */
      sprintf (pipe_name, "\\\\.\\pipe\\gdb_wrapper_%ld",
	       GetCurrentProcessId ());

      pipe_handle = CreateFile (pipe_name,
				GENERIC_READ | GENERIC_WRITE,
				FILE_SHARE_READ | FILE_SHARE_WRITE,
				NULL,
				OPEN_EXISTING,
				0,
				NULL);
      if (pipe_handle != INVALID_HANDLE_VALUE)
	pipe_mutex = CreateMutex (NULL, TRUE, NULL);
    }
  else if (pipe_mutex != INVALID_HANDLE_VALUE)
    WaitForSingleObject (pipe_mutex, INFINITE);

  return (pipe_handle != INVALID_HANDLE_VALUE);
}

static int
cygming_wrapper_cmd (const char *cmd, const void *args, int arglen, int *ret,
		     char **retdata)
{
  if (link_to_wrapper ())
    {
      static char buf[4096] = {0};
      int len = strlen (cmd) + 1;
      DWORD read;
      DWORD written;

      memcpy (buf, cmd, len);
      memcpy (buf + len, args, arglen);
      //fprintf (stderr, "%.8lx writing to the wrapper\n", GetCurrentThreadId ());
      if (!WriteFile (pipe_handle, buf, len + arglen, &written, NULL))
	{
	  fprintf (stderr, "error writting to cygming wrapper: %ld\n", GetLastError ());
	  fflush (stderr);
	  ReleaseMutex (pipe_mutex);
	  return 0;
	}

      if (!ReadFile (pipe_handle, buf, sizeof (buf), &read, NULL))
	{
	  fprintf (stderr, "error waiting for cygming wrapper response: %ld",
		   GetLastError ());
	  fflush (stderr);
	  ReleaseMutex (pipe_mutex);
	  return 0;
	}
      //fprintf (stderr, "%.8lx finished read from the wrapper\n", GetCurrentThreadId ());

      /* The returned value should be "OK" <number> ';' <bytes>.  */
      if (memcmp (buf, "OK", 2) != 0)
	{
	  fprintf (stderr, "error: unexpected wrapper response: %s\n", buf);
	  fflush (stderr);
	  ReleaseMutex (pipe_mutex);
	  return 0;
	}

      *ret = strtol (buf + 2, retdata, 0);
      (*retdata)++;
      ReleaseMutex (pipe_mutex);
      return 1;
    }
  else
    return 0;
}

int
tcgetattr (int fd, struct termios *buf)
{
  int ret;
  char *retdata;

  if (cygming_wrapper_cmd ("tcgetattr", "", 0, &ret, &retdata))
    {
      if (ret == 0)
	memcpy (buf, retdata, sizeof (*buf));
      else
	errno = EINVAL;
      return ret;
    }
  else
    {
      memset (buf, 0, sizeof (*buf));
      /* If FD is a TTY, readline will use getch for it, so we will
	 must echo manually - getch does not echo.  If it is not a
	 TTY, we must not echo since we have no way to disable the
	 typical echo.  */
      if (isatty (fd))
	buf->c_lflag = ECHO;
      return 0;
    }
}

int
tcsetattr (int fd, int actions, const struct termios *buf)
{
  int ret;
  char *retdata;
  if (cygming_wrapper_cmd ("tcsetattr", buf, sizeof (*buf), &ret, &retdata))
    {
      if (ret != 0)
	errno = EINVAL;
      return ret;
    }
  else
    {
      errno = EINVAL;
      return -1;
    }
}

int
tcdrain (int fd)
{
  int ret;
  char *retdata;
  if (cygming_wrapper_cmd ("tcdrain", "", 0, &ret, &retdata))
    {
      if (ret != 0)
	errno = EINVAL;
      return ret;
    }
  else
    {
      errno = EINVAL;
      return -1;
    }
}

int
tcflow (int fd, int action)
{
  errno = ENOTTY;
  return -1;
}

int
mingw_getwinsize (struct winsize *window_size)
{
  int ret;
  char *retdata;
  if (cygming_wrapper_cmd ("getwinsize", "", 0, &ret, &retdata))
    {
      if (ret == 0)
	memcpy (window_size, retdata, sizeof (*window_size));
      else
	errno = EINVAL;
      return ret;
    }
  else
    {
      errno = EINVAL;
      return -1;
    }
}
