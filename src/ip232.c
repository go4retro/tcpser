#include <sys/socket.h>   // for recv...
#include <stdlib.h>       // for exit...
#include <sys/file.h>
#include <unistd.h>
#include <termios.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include <sys/select.h>

#include "util.h"
#include "debug.h"
#include "dce.h"
#include "ip.h"
#include "ip232.h"

void *ip232_thread(void *arg) {
  dce_config *cfg = (dce_config *)arg;
  int rc;
  unsigned char buf[256];

  fd_set readfs;
  int max_fd = 0;

  LOG_ENTER();
  for (;;) {
    FD_ZERO(&readfs);
    FD_SET(cfg->dp[1][0], &readfs);
    max_fd = cfg->dp[1][0];
    FD_SET(cfg->sSocket, &readfs);
    max_fd = MAX(max_fd, cfg->sSocket);
    LOG(LOG_ALL, "Waiting for incoming ip232 connections");
    rc = select(max_fd + 1, &readfs, NULL, NULL, NULL);

    if (rc < 0) {
      // handle error
    } else {
      if (FD_ISSET(cfg->dp[1][0], &readfs)) {  // pipe
        rc = readPipe(cfg->dp[1][0], buf, sizeof(buf) - 1);
        LOG(LOG_DEBUG, "ip232 thread notified");
      }
      if (FD_ISSET(cfg->sSocket, &readfs)) {  // ip connection
        LOG(LOG_DEBUG, "Incoming ip232 connection");
        rc = ip_accept(cfg->sSocket);
        if(cfg->is_connected) {
          LOG(LOG_DEBUG, "Already have ip232 connection, rejecting new");
          // already have a connection... accept and close
          if(rc > -1) {
            close(rc);
          }
        } else {
          if(rc > -1) {
            cfg->fd = rc;
            cfg->is_connected = TRUE;
            cfg->ip232_dtr = FALSE;
            cfg->ip232_dcd = FALSE;
          }
        }
      }
    }
  }
  LOG_EXIT();
}

int ip232_init_conn(dce_config *cfg) {
  int rc = -1;

  LOG_ENTER();
  LOG(LOG_INFO, "Opening ip232 device");
  rc = ip_init_server_conn(cfg->tty, 25232);

  if (rc < 0) {
    ELOG(LOG_FATAL, "Could not initialize ip232 server socket");
    exit(-1);
  }
  if(-1 == pipe(cfg->dp[0])) {
    ELOG(LOG_FATAL, "ip232 thread incoming IPC pipe could not be created");
    exit(-1);
  }

  if(-1 == pipe(cfg->dp[1])) {
    ELOG(LOG_FATAL, "ip232 thread outgoing IPC pipe could not be created");
    exit(-1);
  }

  cfg->sSocket = rc;
  cfg->is_connected = FALSE;
  spawn_thread(ip232_thread, (void *)cfg, "IP232");
  LOG(LOG_INFO, "ip232 device configured");
  LOG_EXIT();
  return 0;
}


int ip232_set_flow_control(dce_config *cfg, int status) {
  return 0;
}

int ip232_get_control_lines(dce_config *cfg) {

  return ((cfg->is_connected ? DCE_CL_LE : 0)
          | ((cfg->is_connected && cfg->ip232_dtr) ? DCE_CL_DTR : 0)
         );
}

int ip232_set_control_lines(dce_config *cfg, int state) {
  int dcd;
  int ri;
  unsigned char cmd[2];

  dcd = (state & DCE_CL_DCD) ? TRUE : FALSE;
  ri = (state & DCE_CL_RI) ? TRUE : FALSE;
  LOG(LOG_DEBUG, "ip232 control line state dcd:%x ri:%x", dcd, ri);
  if ((dcd != cfg->ip232_dcd) || (ri != cfg->ip232_ri)){
    LOG(LOG_DEBUG, "reconfiguring virtual DCD/RI");
    cfg->ip232_dcd = dcd;
    cfg->ip232_ri = ri;
    if (cfg->is_connected) {
      LOG(LOG_DEBUG, "Sending data");
      cmd[0] = 255;
      cmd[1] = (dcd ? 1 : 0) | (ri ? 2 : 0);
      write(cfg->fd, cmd, sizeof(cmd));
    }
  }

  return 0;
}

int ip232_write(dce_config *cfg, unsigned char* data, int len) {
  int retval;
  int i = 0;
  int double_iac = FALSE;
  unsigned char text[1024];
  int text_len = 0;

  log_trace(TRACE_MODEM_OUT, data, len);
  retval = len;
  if (cfg->is_connected) {
    while(i < len) {
      if (double_iac) {
        text[text_len++] = 255;
        double_iac = FALSE;
        i++;
      } else {
        if(255 == data[i]) {
          text[text_len++] = 255;
          double_iac = TRUE;
        } else {
          text[text_len++] = data[i++];
        }
      }

      if(text_len == sizeof(text)) {
        retval = write(cfg->fd, text, text_len);
        text_len = 0;
      }
    }
    if(text_len) {
      retval = write(cfg->fd, text, text_len);
    }
  }
  return retval;
}

int ip232_read(dce_config *cfg, unsigned char *data, int len) {
  int res;
  //int rc;
  unsigned char buf[256];
  int i = 0;
  unsigned char ch;
  int text_len = 0;

  LOG_ENTER();
  if (len > sizeof(buf)) {
    LOG(LOG_FATAL, "ip232_read: len > sizeof(buf)");
    exit(-1);
  }

  if (cfg->is_connected) {
    res = recv(cfg->fd, buf, len, 0);
    if (0 >= res) {
      LOG(LOG_INFO, "No ip232 socket data read, assume closed peer");
      ip_disconnect(cfg->fd);
      cfg->is_connected = FALSE;
    } else {
      LOG(LOG_DEBUG, "Read %d bytes from ip232 socket", res);
      log_trace(TRACE_MODEM_IN, buf, res);

      while(i < res) {
        ch = buf[i];
        if (cfg->ip232_iac) {
          cfg->ip232_iac = FALSE;
          switch (ch) {
            case 0:
              cfg->ip232_dtr = FALSE;
              LOG(LOG_DEBUG, "Virtual DTR line down");
              break;
            case 1:
              cfg->ip232_dtr = TRUE;
              LOG(LOG_DEBUG, "Virtual DTR line up");
              break;
            case 255:
              data[text_len++] = 255;
              break;
          }
        } else {
          if (255 == ch) {
            cfg->ip232_iac = TRUE;
          } else {
            data[text_len++] = ch;
          }
        }
        i++;
      }
    }
  }
  LOG_EXIT();
  return text_len;
}
