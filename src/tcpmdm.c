#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/param.h>
#include <termios.h>
#include <sys/ioctl.h>

#include "serial.h"
#include "ip.h"
#include "debug.h"
#include "tcpser.h"



int main(int argc, char* argv[]) {
  struct timeval timer;  
  struct timeval* ptimer;  
  unsigned char tty[255]="/dev/ttyS2";
  //unsigned char addy[255]="dilbert:6400";
  unsigned char init[255]="at";
  unsigned char cr[1]="\x0d";
  int speed=38400;
  int sfd=-1;
  int csd=-1;
  //int ssd=-1;
  int max_fd=0;
  fd_set readfs; 
  int res=0;
  unsigned char buf[255];
  int rc;

  log_init();
  log_set_trace_flags(TRACE_MODEM_IN | TRACE_MODEM_OUT);
  log_set_level(7);
  // open serial port
  // config serial port
  if(0 > (sfd = ser_init_conn(tty,speed))) {
    ELOG(LOG_FATAL,"Could not open serial port");
    exit(-1);
  }

  // send init string
  if(strlen(init) > 0) {
    ser_write(sfd,init,strlen(init));
    ser_write(sfd,cr,1);
  }

  // if configured, listen on ip address

  // check for carrier detect line
  // when line goes high, connect to IP:port
  // when line goes low, disconnect
  for(;;) {
    FD_ZERO(&readfs);
    max_fd=sfd;
    FD_SET(sfd, &readfs); 
    if(-1 < csd) {
      // we are connected.
      max_fd=MAX(max_fd,csd);
      FD_SET(csd, &readfs); 
    }
    max_fd++;

    timer.tv_sec=0;
    timer.tv_usec=1000;
    ptimer=&timer;
    
    LOG(LOG_ALL,"csd=%d",csd);
    rc=select(max_fd, &readfs, NULL, NULL, ptimer);
    if(-1 == rc) {
      ELOG(LOG_WARN,"Select returned error");
    } else if (0 == rc) {
      // timer popped.
      if(-1 == csd && (ser_get_control_lines(sfd) & TIOCM_CD) > 0) {
        // we need to connect
        // commented out for now.
        //csd=ip_connect(addy);
      } else if(-1 < csd && (ser_get_control_lines(sfd) & TIOCM_CD) == 0) {
        // we need to disconnect.
          ip_disconnect(csd);
          csd=-1;
      } 
    } else if(rc > 0) {
      // we got data from someone.
      if (FD_ISSET(sfd,&readfs)) {  // serial port
        LOG(LOG_DEBUG,"Data available on serial port");
        res = ser_read(sfd,buf,sizeof(buf) -1);
        if(res > 0 && csd > -1) {
          ip_write(csd,buf,res);
        }
      }
      if (csd > -1 && FD_ISSET(csd,&readfs)) {  // ip port
        LOG(LOG_DEBUG,"Data available on tcp/ip port");
        res = ip_read(sfd,buf,sizeof(buf) -1);
        if(res==0) {
          // conn closed, disconnect.
          ip_disconnect(csd);
          csd=-1;
        }
        if(res > 0 && csd > -1) {
          ser_write(csd,buf,res);
        }
      }

    }
 
    

  }






}
