#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/socket.h>   // for recv...
#endif
#include <unistd.h>       // for read...
#include <stdlib.h>       // for exit...
#include <sys/param.h>
#include <sys/time.h>
#include <pthread.h>

#include "util.h"
#include "debug.h"
#include "nvt.h"
#include "modem_core.h"
#include "ip.h"
#include "getcmd.h"

#include "bridge.h"

const char MDM_NO_ANSWER[] = "NO ANSWER\n";

int accept_connection(modem_config *cfg) {
  LOG_ENTER();

  if(-1 != line_accept(&cfg->line_data)) {
    if(cfg->direct_conn == TRUE) {
      cfg->conn_type = MDM_CONN_INCOMING;
      mdm_off_hook(cfg);
    } else {
      //line_write(cfg,(unsigned char*)CONNECT_NOTICE,strlen(CONNECT_NOTICE));
      cfg->ring_ctr = 0;
      cfg->s[S_REG_RING_COUNT] = 0;
      mdm_send_ring(cfg);  // send the first RING
    }
    // tell parent I got it.
    LOG(LOG_DEBUG, "Informing parent task that I am busy");
    writePipe(cfg->mp[0][1], MSG_BUSY);
  }
  LOG_EXIT();
  return 0;
}

int parse_ip_data(modem_config *cfg, unsigned char *data, int len) {
  // I'm going to cheat and assume it comes in chunks.
  int i = 0;
  unsigned char ch;
  unsigned char text[1025];
  int text_len = 0;

  if(cfg->line_data.is_data_received == FALSE) {
    cfg->line_data.is_data_received = TRUE;
    if((data[0] == 0xff) || (data[0] == 0x1a)) {
      //line_write(cfg, (char*)TELNET_NOTICE,strlen(TELNET_NOTICE));
      LOG(LOG_INFO, "Detected telnet");
      // TODO add in telnet stuff
      cfg->line_data.is_telnet = TRUE;
      /* we need to let the other end know that our end will
       * handle the echo - otherwise "true" telnet clients like
       * those that come with Linux & Windows will echo characters
       * typed and you'll end up with doubled characters if the remote
       * host is echoing as well...
       * - gwb
       */
      send_nvt_command(cfg->line_data.fd, &cfg->line_data.nvt_data, NVT_WILL, NVT_OPT_ECHO);
    }
  }

  if(cfg->line_data.is_telnet == TRUE) {
    // once the serial port has seen a bit of data and telnet is active,
    // we can decide on binary transmit, not before
    if(cfg->is_binary_negotiated == FALSE) {
      if(dce_get_parity(&cfg->dce_data)) {
        // send explicit notice this connection is not 8 bit clean
        send_nvt_command(cfg->line_data.fd,
                         &cfg->line_data.nvt_data,
                         NVT_WONT,
                         NVT_OPT_TRANSMIT_BINARY
                        );
        send_nvt_command(cfg->line_data.fd,
                         &cfg->line_data.nvt_data,
                         NVT_DONT,
                         NVT_OPT_TRANSMIT_BINARY
                        );
      } else {
        send_nvt_command(cfg->line_data.fd,
                         &cfg->line_data.nvt_data,
                         NVT_WILL,
                         NVT_OPT_TRANSMIT_BINARY
                        );
        send_nvt_command(cfg->line_data.fd,
                         &cfg->line_data.nvt_data,
                         NVT_DO,
                         NVT_OPT_TRANSMIT_BINARY
                        );
      }
      cfg->is_binary_negotiated = TRUE;
    }
    while(i < len) {
      ch = data[i];
      if(NVT_IAC == ch) {
        // what if we roll off the end?
        ch = data[i + 1];
        switch(ch) {
          case NVT_WILL:
          case NVT_DO:
          case NVT_WONT:
          case NVT_DONT:
            /// again, overflow issues...
            LOG(LOG_INFO, "Parsing nvt command");
            parse_nvt_command(&cfg->dce_data,
                              cfg->line_data.fd,
                              &cfg->line_data.nvt_data,
                              ch,
                              data[i + 2]
                             );
            i += 3;
            break;
          case NVT_SB:      // sub negotiation
            // again, overflow...
            i += parse_nvt_subcommand(&cfg->dce_data, 
                                      cfg->line_data.fd, 
                                      &cfg->line_data.nvt_data, 
                                      data + i, 
                                      len - i
                                     );
            break;
          case NVT_IAC:
            if (cfg->line_data.nvt_data.binary_recv)
              text[text_len++] = NVT_IAC;
              // fall through to skip this sequence
          default:
            // ignore...
            i += 2;
        }
      } else {
        text[text_len++] = data[i++];
      }
      if(text_len == 1024) {
        text[text_len] = 0;
        // write to serial...
        mdm_write(cfg, text, text_len);
        text_len = 0;
      }
    }
    if(text_len) {
      text[text_len] = 0;
      // write to serial...
      mdm_write(cfg, text, text_len);
    }
  } else {
    mdm_write(cfg, data, len);
  }
  return 0;
}

void *ip_thread(void *arg) {
  modem_config* cfg = (modem_config *)arg;

  int action_pending = FALSE;
  fd_set readfs;
  int max_fd;
  int res = 0;
  unsigned char buf[256];
  int rc;


  LOG_ENTER();
  while(TRUE) {
    FD_ZERO(&readfs);
    FD_SET(cfg->cp[1][0], &readfs);
    max_fd = cfg->cp[1][0];
    if(action_pending == FALSE
       && cfg->conn_type != MDM_CONN_NONE
       && cfg->is_cmd_mode == FALSE
       && cfg->line_data.fd > -1
       && cfg->line_data.is_connected == TRUE
      ) {
      FD_SET(cfg->line_data.fd, &readfs); 
      max_fd=MAX(max_fd, cfg->line_data.fd);
    }
    max_fd++;
    rc = select(max_fd, &readfs, NULL, NULL, NULL);
    if(rc == -1) {
      ELOG(LOG_WARN, "Select returned error");
      // handle error
    } else {
      // we got data
      if (cfg->line_data.is_connected == TRUE && FD_ISSET(cfg->line_data.fd, &readfs)) {  // socket
        LOG(LOG_DEBUG, "Data available on socket");
        res = line_read(&cfg->line_data, buf, sizeof(buf));
        //res = recv(cfg->line_data.fd, buf, sizeof(buf), 0);
        if(0 >= res) {
          LOG(LOG_INFO, "No socket data read, assume closed peer");
          writePipe(cfg->cp[0][1], MSG_DISCONNECT);
          action_pending = TRUE;
        } else {
          LOG(LOG_DEBUG, "Read %d bytes from socket", res);
          buf[res] = 0;
          parse_ip_data(cfg, buf, res);
        }
      }
      if (FD_ISSET(cfg->cp[1][0], &readfs)) {  // pipe

        res = readPipe(cfg->cp[1][0], buf, sizeof(buf) - 1);
        LOG(LOG_DEBUG, "IP thread notified");
        action_pending = FALSE;
      }
    }
  }
  LOG_EXIT();
}

void *ctrl_thread(void *arg) {
  modem_config* cfg = (modem_config *)arg;
  int status;
  int new_status;

  LOG_ENTER();
  status = dce_get_control_lines(&cfg->dce_data);
  while(status > -1) {
    new_status = dce_check_control_lines(&cfg->dce_data);
    if(new_status > -1 && status != new_status) {
      LOG(LOG_DEBUG, "Control Line Change");
      if((new_status & DCE_CL_DTR) != (status & DCE_CL_DTR)) {
        if((new_status & DCE_CL_DTR)) {
          LOG(LOG_INFO, "DTR has gone high");
          writePipe(cfg->wp[0][1], MSG_DTR_UP);
        } else {
          LOG(LOG_INFO, "DTR has gone low");
          writePipe(cfg->wp[0][1], MSG_DTR_DOWN);
        }
      }
      if((new_status & DCE_CL_LE) != (status & DCE_CL_LE)) {
        if((new_status & DCE_CL_LE)) {
          LOG(LOG_INFO, "Link has come up");
          writePipe(cfg->wp[0][1], MSG_LE_UP);
        } else {
          LOG(LOG_INFO, "Link has gone down");
          writePipe(cfg->wp[0][1], MSG_LE_DOWN);
        }
      }
    }
    status = new_status;
  }
  LOG_EXIT();
  // need to quit application, as status cannot be obtained.
  exit(-1);
}

void *bridge_task(void *arg) {
  modem_config *cfg = (modem_config *)arg;
  struct timeval timer;  
  struct timeval *ptimer;  
  int max_fd = 0;
  fd_set readfs;
  int res = 0;
  unsigned char buf[256];
  int rc = 0;

  int last_conn_type;
  int last_cmd_mode = cfg->is_cmd_mode;


  LOG_ENTER();

  if(-1 == pipe(cfg->wp[0])) {
    ELOG(LOG_FATAL, "Control line watch task incoming IPC pipe could not be created");
    exit(-1);
  }
  if(-1 == pipe(cfg->cp[0])) {
    ELOG(LOG_FATAL, "IP thread incoming IPC pipe could not be created");
    exit(-1);
  }
  if(-1 == pipe(cfg->cp[1])) {
    ELOG(LOG_FATAL, "IP thread outgoing IPC pipe could not be created");
    exit(-1);
  }
  if(dce_connect(&cfg->dce_data) < 0) {
    ELOG(LOG_FATAL, "Could not open serial port %s", cfg->dce_data.tty);
    exit(-1);
  }

  spawn_thread((void *)ctrl_thread, (void *)cfg, "CTRL");
  spawn_thread((void *)ip_thread, (void *)cfg, "IP");

  mdm_set_control_lines(cfg);
  last_conn_type = cfg->conn_type;
  cfg->allow_transmit = FALSE;
  // call some functions behind the scenes
  if(cfg->cur_line_idx) {
    mdm_parse_cmd(cfg);
  }
  if (cfg->direct_conn == TRUE) {
    if(strlen((char *)cfg->direct_conn_num) > 0 &&
       cfg->direct_conn_num[0] != ':') {
        // we have a direct number to connect to.
      strncpy(cfg->dialno, cfg->direct_conn_num, sizeof(cfg->dialno));
      if(0 != line_connect(&cfg->line_data, cfg->dialno)) {
        LOG(LOG_FATAL, "Cannot connect to Direct line address!");
        // probably should exit...
        exit(-1);
      } else {
        cfg->conn_type = MDM_CONN_OUTGOING;
      }
    }
  }
  cfg->allow_transmit = TRUE;
  for(;;) {
    if(last_conn_type != cfg->conn_type) {
      LOG(LOG_ALL, "Connection status change, handling");
      writePipe(cfg->cp[1][1], MSG_NOTIFY);
      if(cfg->conn_type == MDM_CONN_OUTGOING) {
        if(strlen(cfg->local_connect) > 0) {
          writeFile(cfg->local_connect, cfg->line_data.fd);
        }
        if(strlen(cfg->remote_connect) > 0) {
          writeFile(cfg->remote_connect, cfg->line_data.fd);
        }
      } else if(cfg->conn_type == MDM_CONN_INCOMING) {
        if(strlen(cfg->local_answer) > 0) {
          writeFile(cfg->local_answer, cfg->line_data.fd);
        }
        if(strlen(cfg->remote_answer) > 0) {
          writeFile(cfg->remote_answer, cfg->line_data.fd);
        }
      }
      last_conn_type = cfg->conn_type;
    }
    if(last_cmd_mode != cfg->is_cmd_mode) {
      writePipe(cfg->cp[1][1], MSG_NOTIFY);
      last_cmd_mode = cfg->is_cmd_mode;
    }
    LOG(LOG_ALL, "Waiting for modem/control line/timer/socket activity");
    LOG(LOG_ALL, "CMD:%d, DCE:%d, LINE:%d, TYPE:%d, HOOK:%d", cfg->is_cmd_mode, cfg->dce_data.is_connected, cfg->line_data.is_connected, cfg->conn_type, cfg->is_off_hook);
    FD_ZERO(&readfs);
    max_fd = cfg->mp[1][0];
    FD_SET(cfg->mp[1][0], &readfs);
    if(cfg->dce_data.is_connected) {
      max_fd = MAX(max_fd, cfg->dce_data.ifd);
      FD_SET(cfg->dce_data.ifd, &readfs);
    }
    max_fd = MAX(max_fd, cfg->wp[0][0]);
    FD_SET(cfg->wp[0][0], &readfs);
    max_fd = MAX(max_fd, cfg->cp[0][0]);
    FD_SET(cfg->cp[0][0], &readfs);
    ptimer = NULL;
    if(cfg->is_cmd_mode == FALSE) {
      if(cfg->pre_break_delay == FALSE || cfg->break_len == 3) {
        LOG(LOG_ALL, "Setting timer for break delay");
        long long usec;
        usec = cfg->s[S_REG_GUARD_TIME] * 20000;
        timer.tv_sec = usec / 1000000;
        timer.tv_usec = usec % 1000000;
        ptimer = &timer;
      } else if(cfg->pre_break_delay == TRUE && cfg->break_len > 0) {
        LOG(LOG_ALL, "Setting timer for inter-break character delay");
        timer.tv_sec = 1;   // 1 second
        timer.tv_usec = 0;
        ptimer = &timer;
      } else if (cfg->s[S_REG_INACTIVITY_TIME] != 0) {
        LOG(LOG_ALL, "Setting timer for inactivity delay");
        timer.tv_sec = cfg->s[S_REG_INACTIVITY_TIME] * 10;
        timer.tv_usec = 0;
        ptimer = &timer;
      }
    } else if(cfg->is_ringing == TRUE // TODO Not sure how we can be ringing with a connection or a lack of IP connection, but leaving in for now.
              && cfg->conn_type == MDM_CONN_NONE
              && cfg->line_data.is_connected == TRUE
             ) {
        LOG(LOG_ALL, "Setting timer for rings");
        if(cfg->ring_ctr)
          timer.tv_sec = 3;   // 3 seconds until next RING send
        else
          timer.tv_sec = 1;   // 1 second for RI to be high
        timer.tv_usec = 0;
        ptimer = &timer;
    }
    max_fd++;
    rc = select(max_fd, &readfs, NULL, NULL, ptimer);
    if(rc == -1) {
      ELOG(LOG_WARN, "Select returned error");
      // handle error
    } else if(rc == 0) {
      // timer popped.
      // TODO ring counter should go back to 0 after 8 secs of no ringing.
      cfg->ring_ctr = 1 - cfg->ring_ctr;
      if(cfg->is_cmd_mode == TRUE
         && cfg->conn_type == MDM_CONN_NONE
         && cfg->line_data.is_connected == TRUE
        ) {
        if(0 == cfg->ring_ctr) {
          if(cfg->s[0] == 0 && cfg->s[S_REG_RING_COUNT] == 10) {
            // not going to answer, send some data back to IP and disconnect.
            if(strlen(cfg->no_answer) == 0) {
              line_write(&cfg->line_data, (unsigned char *)MDM_NO_ANSWER, strlen(MDM_NO_ANSWER));
            } else {
              writeFile(cfg->no_answer, cfg->line_data.fd);
            }
            cfg->is_ringing = FALSE;
            if(cfg->direct_conn) {
              LOG(LOG_INFO, "Direct connection active, maintaining link");
            } else {
              line_disconnect(&cfg->line_data);
            }
            //mdm_disconnect(cfg, FALSE); // not sure need to do a disconnect here, no connection
          } else {
            mdm_send_ring(cfg);
          }
        }
      } else {
        mdm_handle_timeout(cfg);
      }
      mdm_set_control_lines(cfg);
    }
    if (FD_ISSET(cfg->dce_data.ifd, &readfs)) {  // serial port
      LOG(LOG_DEBUG, "Data available on serial port");
      res = mdm_read(cfg, buf, sizeof(buf));
      if(res > 0) {
        if(cfg->conn_type == MDM_CONN_NONE
           && !cfg->is_cmd_mode
           && cfg->is_off_hook) {
          // this handles the case where atdt/ata goes off hook, but no
          // connection
          mdm_disconnect(cfg, FALSE);
        } else {
          mdm_parse_data(cfg, buf, res);
        }
      }
    }
    if (FD_ISSET(cfg->wp[0][0], &readfs)) {  // control pipe
      res = readPipe(cfg->wp[0][0], buf, sizeof(buf) - 1);
      LOG(LOG_DEBUG, "Received %s from control line watch task", buf);
      for(int i = 0; i < res ; i++) {
        switch (buf[0]) {
          case MSG_DTR_DOWN:
            // DTR drop, close any active connection and put
            // in cmd_mode
            mdm_disconnect(cfg, FALSE);
            break;
          default:
            break;
        }
      }
    }
    if (FD_ISSET(cfg->cp[0][0], &readfs)) {  // ip thread pipe
      res = readPipe(cfg->cp[0][0], buf, sizeof(buf));
      LOG(LOG_DEBUG, "Received %c from ip thread", buf[0]);
      switch (buf[0]) {
        case MSG_DISCONNECT:
          if(cfg->direct_conn == TRUE) {
            // what should we do here...
            LOG(LOG_ERROR, "Direct Connection Link broken, disconnecting and awaiting new direct connection");
            mdm_disconnect(cfg, TRUE);
          } else {
            mdm_disconnect(cfg, FALSE);
          }
          break;
      }
    }
    if (FD_ISSET(cfg->mp[1][0], &readfs)) {  // parent pipe
      LOG(LOG_DEBUG, "Data available on incoming IPC pipe");
      res = readPipe(cfg->mp[1][0], buf, sizeof(buf));
      switch (buf[0]) {
        case MSG_CALLING:       // accept connection.
          accept_connection(cfg);
          break;
      }
    }
  }
  LOG_EXIT();
}
