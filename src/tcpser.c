#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>

#include <sys/param.h>
#include <pthread.h>

#include "bridge.h"
#include "debug.h"
#include "init.h"
#include "ip.h"
#include "modem_core.h"
#include "phone_book.h"
#include "util.h"

const char MDM_BUSY[] = "BUSY\n";

#define MAX_MODEMS 16

int main(int argc, char *argv[]) {
  modem_config cfg[MAX_MODEMS];
  int modem_count;
  char *ip_addr = NULL;
  char default_ip[] = "6400";

  char all_busy[255];
  pthread_t thread_id;
  int i;
  int rc = 0;
  int sSocket = 0;
  fd_set readfs;
  int max_fd = 0;
  int accept_pending = FALSE;
  unsigned char buf[255];
  int cSocket;

  log_init();

  LOG_ENTER();

  log_set_level(LOG_FATAL);

  mdm_init();

  pb_init();
  
  signal(SIGIO, SIG_IGN); /* Some Linux variant term on SIGIO by default */

  modem_count = init(argc, argv, cfg, MAX_MODEMS, &ip_addr, all_busy, sizeof(all_busy));
  if(ip_addr == NULL)
    ip_addr = default_ip;
  sSocket = ip_init_server_conn(ip_addr);
  if(-1 == sSocket) {
    ELOG(LOG_FATAL, "Could not listen on %s", ip_addr);
    exit (-1);
  }

  for(i = 0; i < modem_count; i++) {
    LOG(LOG_INFO, "Creating modem #%d", i);
    if( -1 == pipe(cfg[i].mp[0])) {
      ELOG(LOG_FATAL, "Bridge task incoming IPC pipe could not be created");
      exit(-1);
    }
    if( -1 == pipe(cfg[i].mp[1])) {
      ELOG(LOG_FATAL, "Bridge task outgoing IPC pipe could not be created");
      exit(-1);
    }
    if(dce_connect(&cfg[i].dce_data) < 0) {
      ELOG(LOG_FATAL, "Could not open serial port %s", cfg[i].dce_data.tty);
      exit(-1);
    }
    cfg[i].line_data.sfd = sSocket;

    rc=pthread_create(&thread_id, NULL, *run_bridge, (void *)&cfg[i]);
    if(rc < 0) {
        ELOG(LOG_FATAL, "IP thread could not be started");
        exit(-1);
    }

  }

  for(;;) {
    FD_ZERO(&readfs);
    max_fd = 0;
    for(i = 0; i < modem_count; i++) {
      FD_SET(cfg[i].mp[0][0], &readfs);
      max_fd=MAX(max_fd, cfg[i].mp[0][0]);
    }
    if(accept_pending == FALSE) {
      max_fd=MAX(max_fd, sSocket);
      FD_SET(sSocket, &readfs); 
    }
    LOG(LOG_ALL, "Waiting for incoming connections and/or indicators");
    select(max_fd+1, &readfs, NULL, NULL, NULL);
    for(i = 0; i < modem_count; i++) {
      if (FD_ISSET(cfg[i].mp[0][0], &readfs)) {  // child pipe
        rc = read(cfg[i].mp[0][0], buf, sizeof(buf) -1);
        if(rc > -1) {
          buf[rc] = 0;
          LOG(LOG_DEBUG, "modem core #%d sent response '%s'", i, buf);
          accept_pending = FALSE;
        }
      }
    }
    if (FD_ISSET(sSocket, &readfs)) {  // IP traffic
      if(!accept_pending) {
        LOG(LOG_DEBUG, "Incoming connection pending");
        // first try for a modem that is listening.
        for(i = 0; i < modem_count; i++) {
          if(cfg[i].s[0] != 0 && cfg[i].is_off_hook == FALSE) {
            // send signal to pipe saying pick up...
            LOG(LOG_DEBUG, "Sending incoming connection to listening modem #%d", i);
            writePipe(cfg[i].mp[1][1], MSG_CALLING);
            accept_pending = TRUE;
            break;
          }
        }
        // now, send to any non-active modem.
        for(i = 0; i < modem_count; i++) {
          if(cfg[i].is_off_hook == FALSE) {
            // send signal to pipe saying pick up...
            LOG(LOG_DEBUG, "Sending incoming connection to non-connected modem #%d", i);
            writePipe(cfg[i].mp[1][1], MSG_CALLING);
            accept_pending = TRUE;
            break;
          }
        }
        if(i == modem_count) {
          LOG(LOG_DEBUG, "No open modem to send to, send notice and close");
          // no connections.., accept and print error
          cSocket = ip_accept(sSocket);
          if(cSocket > -1) {
            // No tracing on this data output
            if(strlen(all_busy) < 1) {
              ip_write(cSocket, (unsigned char *)MDM_BUSY, strlen(MDM_BUSY));
            } else {
              writeFile(all_busy, cSocket);
            }
            close(cSocket);
          }
        }
      }
    }
  }
  LOG_EXIT();
  return rc;
}
