/*
 ============================================================================
 Name        : IP232Test.c
 Author      : Jim Brain
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <sys/socket.h>   // for recv...
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>       // for read...

#include "dce.h"
#include "debug.h"
#include "util.h"
#include "ip.h"

void *ip_thread(void *arg) {

  fd_set readfs;
  int max_fd = 0;
  int res = 0;
  unsigned char buf[256];
  dce_config *data = (dce_config *)arg;
  int rc;


  LOG_ENTER();
  while(TRUE) {
    FD_ZERO(&readfs);
    FD_SET(data->ifd, &readfs);
    max_fd=MAX(max_fd, data->ifd);
    max_fd++;
    rc = select(max_fd, &readfs, NULL, NULL, NULL);
    if(rc == -1) {
      ELOG(LOG_FATAL, "Select returned error");
      exit(-1);
    } else {
      // we got data
      if (FD_ISSET(data->ifd, &readfs)) {  // socket
        LOG(LOG_DEBUG, "Data available on socket");
        res = dce_read(data, buf, sizeof(buf));
        LOG(LOG_DEBUG, "Read %d bytes from socket", res);
        buf[res] = 0;
        //if(!res) {
        //  ELOG(LOG_FATAL, "Lost connection");
        //  exit(-1);
        //}
        //parse_ip_data(cfg, buf, res);
      }
    }
  }
  LOG_EXIT();
}

void send_cmd(int sd, unsigned char cmd) {
  unsigned char buf[2];

  buf[0] = 255;
  buf[1] = cmd;
  ip_write(sd, buf, 2);

}

int main(int argc, char *argv[]) {
  dce_config data;

  char address[] = "localhost:25232";
  //unsigned char text[] = "atdtdilbert\x0d";

  log_init();
  dce_init_config(&data);

  printf("IP232 Test Program\n");

  log_set_level(LOG_ALL);
  log_set_trace_flags(255);

  data.ofd = ip_connect(address);
  if(data.ofd > -1) {
    data.ifd = data.ofd;
    data.is_connected = TRUE;
    spawn_thread(ip_thread, (void *)&data, "IP");
    //dce_write(&data, text, strlen((char *)text));
    // add in more text here.
    while(1);
  }


  return EXIT_SUCCESS;
}
