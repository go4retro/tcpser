#include <sys/file.h>
#include <unistd.h>
#include <termios.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "dce.h"
#include "debug.h"

#include "serial.h"

int ser_get_bps_const(int speed) {
  int bps_rate = 0;

  LOG_ENTER();
  
  LOG(LOG_DEBUG,"Checking speed: %d",speed);

  switch (speed) {
#if defined (B921600)
    case 921600:
      bps_rate = B921600;
      break;
#endif /* B921600 */
#if defined (B460800)
    case 460800:
      bps_rate = B460800;
      break;
#endif /* B460800 */
#if defined (B230400)
    case 230400:
      bps_rate = B230400;
      break;
#endif /* B230400 */
#if defined (B115200)
    case 115200:
      bps_rate = B115200;
      break;
#endif /* B115200 */
#if defined (B57600)
    case 57600:
      bps_rate = B57600;
      break;
#endif /* B57600 */
#if defined (B38400)
    case 38400:
      bps_rate = B38400;
      break;
#endif /* B38400 */
#if defined (B19200)
    case 19200:
      bps_rate = B19200;
      break;
#endif /* B19200 */
#if defined (B9600)
    case 9600:
      bps_rate = B9600;
      break;
#endif /* B9600 */
#if defined (B4800)
    case 4800:
      bps_rate = B4800;
      break;
#endif /* B4800 */
#if defined (B2400)
    case 2400:
      bps_rate = B2400;
      break;
#endif /* B2400 */
#if defined (B1200)
    case 1200:
      bps_rate = B1200;
      break;
#endif /* B1200 */
#if defined (B600)
    case 600:
      bps_rate = B600;
      break;
#endif /* B600 */
#if defined (B300)
    case 300:
      bps_rate = B300;
      break;
#endif /* B300 */
#if defined (B150)
    case 150:
      bps_rate = B150;
      break;
#endif /* B150 */
#if defined (B134)
    case 134:
      bps_rate = B134;
      break;
#endif /* B134 */
#if defined (B110)
    case 110:
      bps_rate = B110;
      break;
#endif /* B110 */
#if defined (B75)
    case 75:
      bps_rate = B75;
      break;
#endif /* B75 */
#if defined (B50)
    case 50:
      bps_rate = B50;
      break;
#endif /* B50 */
#if defined (B0)
    case 0:
      bps_rate = B0;
      break;
#endif /* B0 */
    default:
      ELOG(LOG_FATAL, "Unknown baud rate"); 
      bps_rate = -1;
  }
  LOG_EXIT();
  return bps_rate;

}

int ser_init_conn(char *tty, int speed) {
  int fd = -1;
  struct termios tio;
  int bps_rate = 0;

  LOG_ENTER();

  bps_rate = ser_get_bps_const(speed);

  if(bps_rate > -1) {
    /* open the device to be non-blocking (read will return immediatly) */
    LOG(LOG_INFO, "Opening serial device %s at speed %d", tty, speed);

    fd = open(tty, O_RDWR | O_NOCTTY | O_NONBLOCK);

    if (fd <0) {
      ELOG(LOG_FATAL, "TTY %s could not be opened", tty); 
    } else {
      LOG(LOG_INFO, "Opened serial device %s at speed %d as fd %d", tty, speed, fd);

      /* Make the file descriptor asynchronous (the manual page says only 
         O_APPEND and O_NONBLOCK, will work with F_SETFL...) */
      fcntl(fd, F_SETFL, FASYNC);

      tio.c_cflag = CS8 | CLOCAL | CREAD | CRTSCTS;
      tio.c_iflag = IGNBRK;
      tio.c_oflag = 0;
      tio.c_lflag = 0;
      cfsetispeed(&tio, bps_rate);
      cfsetospeed(&tio, bps_rate);

      tio.c_cc[VMIN] = 1;
      tio.c_cc[VTIME] = 0;

      tcflush(fd, TCIFLUSH);
      tcsetattr(fd, TCSANOW, &tio);
      LOG(LOG_INFO, "serial device configured");
    }
  }

  LOG_EXIT();
  return fd;
}

int ser_set_flow_control(int fd, int status) {
  struct termios tio;
  if(0 != tcgetattr(fd, &tio)) {
    ELOG(LOG_FATAL, "Could not get serial port attributes");
    return -1;
  }
  // turn all off.
  tio.c_cflag &= ~(IXON | IXOFF | CRTSCTS);
  tio.c_cflag |= status;
  if(0 != tcsetattr(fd, TCSANOW, &tio)) {
    ELOG(LOG_FATAL,"Could not set serial port attributes");
    return -1;
  }
  return 0;
}

int ser_get_control_lines(int fd) {
  int status;

  if(0 > ioctl(fd, TIOCMGET, &status)) {
    ELOG(LOG_FATAL, "Could not obtain serial port status");
    return -1;
  }

  return (DCE_CL_LE           // RS232 link is always up.
          | ((status & TIOCM_DSR) ? DCE_CL_DTR : 0)
          //| ((status & TIOCM_CTS) ? DCE_CL_RTS : 0)
         );
}

int ser_set_control_lines(int fd, int state) {
  int status;

  if(0 > ioctl(fd, TIOCMGET, &status)) {
    ELOG(LOG_FATAL, "Could not obtain serial port status");
    return -1;
  }

  status &= ~(TIOCM_DTR);
  //status &= ~(TIOCM_RTS | TIOCM_DTR);
  status |= (state & DCE_CL_DCD ? TIOCM_DTR : 0);
  //status |= (state & DCE_CL_CTS ? TIOCM_RTS : 0);
  if(0 > ioctl(fd, TIOCMSET, &status)) {
    ELOG(LOG_WARN, "Could not set serial port status");
  }
  return 0;
}

int ser_write(int fd, unsigned char* data, int len) {
  log_trace(TRACE_MODEM_OUT, data, len);
  return write(fd, data, len);
}

int ser_read(int fd, unsigned char* data, int len) {
  int res;

  res = read(fd, data, len);
  log_trace(TRACE_MODEM_IN, data, res);
  return res;
}
