#include <termios.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#ifdef WIN32
#include <time.h>
//#include <Windows.h>
#else
#include <linux/serial.h>
#endif
#include "modem_data.h"
#include "modem_core.h"
#include "bridge.h"
#include "debug.h"
#include "serial.h"

int get_bps_const(int speed) {
  LOG_ENTER();
  int bps_rate=0;
  
  LOG(LOG_DEBUG,"Checking speed: %d",speed);

  switch (speed) {
    case 115200:
      bps_rate=B115200;
      break;
    case 57600:
      bps_rate=B57600;
      break;
    case 38400:
      bps_rate=B38400;
      break;
    case 19200:
      bps_rate=B19200;
      break;
    case 9600:
      bps_rate=B9600;
      break;
    case 4800:
      bps_rate=B4800;
      break;
    case 2400:
      bps_rate=B2400;
      break;
    case 1200:
      bps_rate=B1200;
      break;
    case 600:
      bps_rate=B600;
      break;
    case 300:
      bps_rate=B300;
      break;
    case 150:
      bps_rate=B150;
      break;
    case 134:
      bps_rate=B134;
      break;
    case 110:
      bps_rate=B110;
      break;
    case 75:
      bps_rate=B75;
      break;
    case 50:
      bps_rate=B50;
      break;
    default:
      ELOG(LOG_FATAL,"Unknown baud rate"); 
      exit(-1);
  }
  LOG_EXIT();
  return bps_rate;

}

int dce_init_config(modem_config *cfg) {
  return 0;
}

int dce_init(char* tty, int speed) {
  int fd = 0;
  struct termios oldtio,tio;
  char buf[255];
  int baud_rate=get_bps_const(speed);

  LOG_ENTER();

  /* open the device to be non-blocking (read will return immediatly) */
  LOG(LOG_INFO,"Opening serial device");

  fd = open(tty, O_RDWR | O_NOCTTY);

  if (fd <0) {
    ELOG(LOG_FATAL,"TTY %s could not be opened",tty); 
    exit(-1); 
  }
  LOG(LOG_INFO,"Opened serial device %s at speed %d as fd %d",tty,speed,fd);

  /* Make the file descriptor asynchronous (the manual page says only 
     O_APPEND and O_NONBLOCK, will work with F_SETFL...) */
  fcntl(fd, F_SETFL, FASYNC);

  tcgetattr(fd,&oldtio); /* save current port settings */
  //cfmakeraw(&tio); /* set to raw */

  /* set new port settings for canonical input processing */
  //tio.c_cflag = baud_rate | CRTSCTS | CS8 | CLOCAL | CREAD;

  tio.c_cflag = baud_rate | CS8 | CLOCAL | CREAD | CRTSCTS;

  //tio.c_iflag &= ~(IGNPAR | PARMRK | IXON | IXANY | IXOFF | INLCR | IGNCR | ICRNL | IUCLC);

  //tio.c_iflag &= ~(PARMRK | IXON | INLCR | IGNCR | ICRNL | IUCLC);
  tio.c_iflag = (IGNBRK);
  tio.c_oflag = 0;
  tio.c_lflag = 0;
  tio.c_cc[VMIN]=1;
  tio.c_cc[VTIME]=0;
  cfsetispeed(&tio, baud_rate);
  cfsetospeed(&tio, baud_rate);


  tcflush(fd, TCIFLUSH);
  tcsetattr(fd,TCSANOW,&tio);
  LOG(LOG_INFO,"serial device configured");

  LOG_EXIT();

  return fd;
}


int dce_set_control_lines(modem_config *cfg,int state) {
  int status;

  LOG_ENTER();
  ioctl(cfg->dce_data.fd, TIOCMGET, &status);
  if((state & MDM_CL_CTS_HIGH) != 0) {
    LOG(LOG_ALL,"Setting CTS pin high");
    status |= TIOCM_RTS;
  } else {
    LOG(LOG_ALL,"Setting CTS pin low");
    status &= ~TIOCM_RTS;
  }
  if((state & MDM_CL_DCD_HIGH) != 0) {
    LOG(LOG_ALL,"Setting DCD pin high");
    status |= TIOCM_DTR;
  } else {
    LOG(LOG_ALL,"Setting DCD pin low");
    status &= ~TIOCM_DTR;
  }
  ioctl(cfg->dce_data.fd, TIOCMSET, &status);
  LOG_EXIT();
  return 0;
}

int dce_get_control_lines(modem_config *cfg) {
  int status;
  int rc_status=0;

  ioctl(cfg->dce_data.fd, TIOCMGET, &status);
  rc_status=((status & TIOCM_DSR) != 0?MDM_CL_DTR_HIGH:0);
  return rc_status;
}


int dce_check_control_lines(modem_config *cfg) {
  int status=0;
  int new_status=0;

  LOG_ENTER();
  status = dce_get_control_lines(cfg);
  new_status = status;
  while(status == new_status) {
    usleep(100000);
    new_status = dce_get_control_lines(cfg);
  }
  return new_status;
}

#ifdef JB
int watch_control_lines(int tty_fd, int pipe) {
  int status=0;
  int dsr=TRUE;
  int cts=TRUE;
  int rng=TRUE;
  int dcd=TRUE;
  int rc=0;
  struct serial_icounter_struct c,d;

  LOG_ENTER();
  if(0 > ioctl(tty_fd,TIOCMGET,&status)) {
    ELOG(LOG_FATAL,"Could not obtain serial port status");
    exit(-1);
  }
  if((status & TIOCM_DSR) == 0)
    dsr=FALSE;
  if((status & TIOCM_CTS) == 0)
    cts=FALSE;
  if((status & TIOCM_CD) == 0)
    dcd=FALSE;
  if((status & TIOCM_RNG) == 0)
    rng=FALSE;

  while(TRUE) {
    ioctl(tty_fd, TIOCGICOUNT, &c);
    ioctl(tty_fd, TIOCMIWAIT, TIOCM_DSR | TIOCM_CD | TIOCM_CTS | TIOCM_RNG);
    /* wait for a change in any modem status line (if the wait is
    interrupted by something other than a modem status interrupt, this
    returns an error.  Otherwise it returns 0) */
    rc=ioctl(tty_fd, TIOCGICOUNT, &d); 
    if(rc == 0) {
      LOG(LOG_ALL,"serial port status lines have changed");
      if(0 > ioctl(tty_fd,TIOCMGET,&status)) {
        ELOG(LOG_FATAL,"Could not obtain serial port status");
        exit(-1);
      }
      if(d.dsr - c.dsr > 0) {
        if(dsr == TRUE && (status & TIOCM_DSR) == 0) {
          LOG(LOG_INFO,"DTR has gone low");
          writePipe(pipe,MSG_DTR_DOWN);
          dsr=FALSE;
        } else if(dsr == FALSE && (status & TIOCM_DSR) != 0) {
          LOG(LOG_INFO,"DTR has gone high");
          writePipe(pipe,MSG_DTR_UP);
          dsr=TRUE;
        }
      }
      if(d.cts - c.cts > 0) {
        if(cts == TRUE && (status & TIOCM_CTS) == 0) {
          LOG(LOG_INFO,"RTS has gone low");
          writePipe(pipe,MSG_RTS_DOWN);
          cts=FALSE;
        } else if(dsr == FALSE && (status & TIOCM_DSR) != 0) {
          LOG(LOG_INFO,"RTS has gone high");
          writePipe(pipe,MSG_RTS_UP);
          cts=TRUE;
        }
      }
      if(d.rng - c.rng > 0) {
        if(rng == TRUE && (status & TIOCM_RNG) == 0) {
          LOG(LOG_INFO,"RNG has gone low");
          writePipe(pipe,MSG_RNG_DOWN);
          rng=FALSE;
        } else if(rng == FALSE && (status & TIOCM_RNG) != 0) {
          LOG(LOG_INFO,"RNG has gone high");
          writePipe(pipe,MSG_RNG_UP);
          rng=TRUE;
        }
      }
      if(d.dcd - c.dcd > 0) {
        if(dcd == TRUE && (status & TIOCM_CD) == 0) {
          LOG(LOG_INFO,"DCD has gone low");
          writePipe(pipe,MSG_CD_DOWN);
          dcd=FALSE;
        } else if(dcd == FALSE && (status & TIOCM_CD) != 0) {
          LOG(LOG_INFO,"DCD has gone high");
          writePipe(pipe,MSG_CD_UP);
          dcd=TRUE;
        }
      }
    }
  }
  LOG_EXIT()
  return 0;
}
#endif


int dce_write(modem_config *cfg,char data[], int len) {
    log_trace(TRACE_MODEM_OUT,data,len);
    return write(cfg->dce_data.fd,data,len);
}


