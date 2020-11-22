#include <unistd.h>
#include <stdlib.h>       // for atoi

#include "getcmd.h"
#include "debug.h"
#include "modem_core.h"

char* mdm_responses[MDM_RESP_END_OF_LIST - 1];

void mdm_init(void) {
  mdm_responses[MDM_RESP_OK] =             "OK";
  mdm_responses[MDM_RESP_RING] =           "RING";
  mdm_responses[MDM_RESP_ERROR] =          "ERROR";
  mdm_responses[MDM_RESP_CONNECT] =        "CONNECT";
  mdm_responses[MDM_RESP_NO_CARRIER] =     "NO CARRIER";
  mdm_responses[MDM_RESP_CONNECT_1200] =   "CONNECT 1200";
  mdm_responses[MDM_RESP_NO_DIALTONE] =    "NO DIALTONE";
  mdm_responses[MDM_RESP_BUSY] =           "BUSY";
  mdm_responses[MDM_RESP_NO_ANSWER] =      "NO ANSWER";
  mdm_responses[MDM_RESP_CONNECT_0600] =   "CONNECT 0600";
  mdm_responses[MDM_RESP_CONNECT_2400] =   "CONNECT 2400";
  mdm_responses[MDM_RESP_CONNECT_4800] =   "CONNECT 4800";
  mdm_responses[MDM_RESP_CONNECT_9600] =   "CONNECT 9600";
  mdm_responses[MDM_RESP_CONNECT_7200] =   "CONNECT 7200";
  mdm_responses[MDM_RESP_CONNECT_12000] =  "CONNECT 12000";
  mdm_responses[MDM_RESP_CONNECT_14400] =  "CONNECT 14400";
  mdm_responses[MDM_RESP_CONNECT_19200] =  "CONNECT 19200";
  mdm_responses[MDM_RESP_CONNECT_38400] =  "CONNECT 38400";
  mdm_responses[MDM_RESP_CONNECT_57600] =  "CONNECT 57600";
  mdm_responses[MDM_RESP_CONNECT_115200] = "CONNECT 115200";
  mdm_responses[MDM_RESP_CONNECT_230400]=  "CONNECT 230400";
  mdm_responses[MDM_RESP_CONNECT_460800] = "CONNECT 460800";
  mdm_responses[MDM_RESP_CONNECT_921600] = "CONNECT 921600";
}

modem_response get_connect_response(int speed, int level) {
  if(level == 0) {
    return MDM_RESP_CONNECT;
  }
  switch (speed) {
    case 921600:
      return MDM_RESP_CONNECT_921600;
    case 460800:
      return MDM_RESP_CONNECT_460800;
    case 230400:
      return MDM_RESP_CONNECT_230400;
    case 115200:
      return MDM_RESP_CONNECT_115200;
    case 57600:
      return MDM_RESP_CONNECT_57600;
    case 38400:
      return MDM_RESP_CONNECT_38400;
    case 19200:
      return MDM_RESP_CONNECT_19200;
    case 9600:
      return MDM_RESP_CONNECT_9600;
    case 4800:
      return MDM_RESP_CONNECT_4800;
    case 2400:
      return MDM_RESP_CONNECT_2400;
    case 1200:
      return MDM_RESP_CONNECT_1200;
    case 600:
      return MDM_RESP_CONNECT_0600;
  }
  return MDM_RESP_CONNECT;
}

void mdm_init_config(modem_config *cfg) {
  int i = 0;
  cfg->local_connect[0] = 0;
  cfg->remote_connect[0] = 0;
  cfg->local_answer[0] = 0;
  cfg->remote_answer[0] = 0;
  cfg->no_answer[0] = 0;
  cfg->inactive[0] = 0;
  cfg->direct_conn = FALSE;
  cfg->direct_conn_num[0] = 0;
  cfg->is_binary_negotiated = FALSE;

  cfg->send_responses = TRUE;
  cfg->connect_response = 0;
  cfg->response_code_level = 4;
  cfg->text_responses = TRUE;
  cfg->is_echo = TRUE;
  cfg->is_cmd_mode = TRUE;
  cfg->conn_type = MDM_CONN_NONE;
  cfg->is_off_hook = FALSE;
  cfg->is_ringing = FALSE;
  cfg->cur_line_idx = 0;
  cfg->rings = 0;

  for(i = 0; i < sizeof(cfg->s) / sizeof(cfg->s[0]); i++) {
    cfg->s[i] = 0;
  }
  cfg->s[S_REG_BREAK] = 43;
  cfg->s[S_REG_CR] = 13;
  cfg->s[S_REG_LF] = 10;
  cfg->s[S_REG_BS] = 8;
  cfg->s[S_REG_BLIND_WAIT] = 2;
  cfg->s[S_REG_CARRIER_WAIT] = 50;
  cfg->s[S_REG_COMMA_PAUSE] = 2;
  cfg->s[S_REG_CARRIER_TIME] = 6;
  cfg->s[S_REG_CARRIER_LOSS] = 14;
  cfg->s[S_REG_DTMF_TIME] = 95;
  cfg->s[S_REG_GUARD_TIME] = 50;

  cfg->crlf[0] = cfg->s[S_REG_CR];
  cfg->crlf[1] = cfg->s[S_REG_LF];
  cfg->crlf[2] = 0;

  cfg->dial_type = 0;
  cfg->last_dial_type = 0;
  cfg->disconnect_delay = 0;

  cfg->pre_break_delay = FALSE;
  cfg->break_len = 0;

  cfg->memory_dial = FALSE;
  cfg->dsr_active = FALSE;
  cfg->force_dsr = TRUE;
  cfg->force_dcd = FALSE;
  cfg->first_ch = 0;
  cfg->is_cmd_started = FALSE;
  cfg->allow_transmit = TRUE;
  cfg->invert_dsr = FALSE;
  cfg->invert_dcd = FALSE;

  dce_init_config(&cfg->dce_data);
  line_init_config(&cfg->line_data);
}

int get_new_cts_state(modem_config *cfg, int up) {
  return DCE_CL_CTS;
}

int get_new_dsr_state(modem_config *cfg, int up) {
  if(cfg->force_dsr || up) {
    return (cfg->invert_dsr ? 0 : DCE_CL_DSR);
  }
  return (cfg->invert_dsr ? DCE_CL_DSR : 0);
}

int get_new_dcd_state(modem_config *cfg, int up) {
  if(cfg->force_dcd || up) {
    return (cfg->invert_dcd ? 0 : DCE_CL_DCD);
  }
  return (cfg->invert_dcd ? DCE_CL_DCD : 0);
}

int mdm_set_control_lines(modem_config *cfg) {
  int state = 0;
  int up = (cfg->conn_type == MDM_CONN_NONE ? FALSE : TRUE);

  state |= get_new_cts_state(cfg, up);
  state |= get_new_dsr_state(cfg, up);
  state |= get_new_dcd_state(cfg, up);

  LOG(LOG_INFO, 
      "Control Lines: DSR:%d DCD:%d CTS:%d",
      ((state & DCE_CL_DSR) != 0 ? 1 : 0),
      ((state & DCE_CL_DCD) != 0 ? 1 : 0),
      ((state & DCE_CL_CTS) != 0 ? 1 : 0)
     );

  dce_set_control_lines(&cfg->dce_data, state);
  return 0;
}

void mdm_write_char(modem_config *cfg, unsigned char data) {
  unsigned char str[1];

  str[0] = data;
  mdm_write(cfg, str, 1);
}

void mdm_write(modem_config *cfg, unsigned char data[], int len) {
  if(cfg->allow_transmit == TRUE) {
    dce_write(&cfg->dce_data, data, len);
  }
}

void mdm_send_response(int msg, modem_config *cfg) {
  char msgID[17];

  LOG(LOG_DEBUG, "Sending %s response to modem", mdm_responses[msg]);
  if(cfg->send_responses == TRUE) {
    mdm_write(cfg, (unsigned char *)cfg->crlf, 2);
    if(cfg->text_responses == TRUE) {
      LOG(LOG_ALL, "Sending text response");
      mdm_write(cfg, (unsigned char *)mdm_responses[msg], strlen(mdm_responses[msg]));
    } else {
      LOG(LOG_ALL, "Sending numeric response");
      sprintf(msgID, "%d", msg);
      mdm_write(cfg, (unsigned char *)msgID, strlen((char *)msgID));
    }
    mdm_write(cfg, (unsigned char *)cfg->crlf, 2);
  }
}

int mdm_print_speed(modem_config *cfg) {
  int speed;

  switch(cfg->connect_response) {
    case 2:
      speed = cfg->dce_data.port_speed;
      break;
    default:
      speed = cfg->line_speed;
      break;

  }
  mdm_send_response(get_connect_response(speed, cfg->response_code_level), cfg);
  return 0;
}

void off_hook(modem_config *cfg) {
  LOG(LOG_INFO, "taking modem off hook");
  cfg->is_off_hook = TRUE;
  line_off_hook(&cfg->line_data);
}

int mdm_off_hook(modem_config *cfg) {
  off_hook(cfg);
  if(cfg->is_ringing == TRUE) {
    cfg->conn_type = MDM_CONN_INCOMING;
    cfg->is_ringing = FALSE;
  }
  if(cfg->conn_type == MDM_CONN_INCOMING) {
    mdm_set_control_lines(cfg);
  }
  return 0;
}

int mdm_answer(modem_config *cfg) {
  if(cfg->is_ringing == TRUE) {
    cfg->is_ringing = FALSE;
    cfg->conn_type = MDM_CONN_INCOMING;
    off_hook(cfg);
    cfg->is_cmd_mode = FALSE;
    mdm_set_control_lines(cfg);
    mdm_print_speed(cfg);
  } else if(cfg->conn_type != MDM_CONN_NONE) {
    // we are connected, just go off hook.
    off_hook(cfg);
    cfg->is_cmd_mode = FALSE;
    mdm_set_control_lines(cfg);
  } else {
    mdm_send_response(MDM_RESP_NO_CARRIER, cfg);
    usleep(cfg->disconnect_delay * 1000);
    //mdm_disconnect(cfg, FALSE);
  }
  return 0;
}

int mdm_connect(modem_config *cfg) {
  off_hook(cfg);
  cfg->is_cmd_mode = FALSE;
  if(cfg->conn_type == MDM_CONN_NONE) {
    if(line_connect(&cfg->line_data, cfg->dialno) == 0) {
      cfg->conn_type = MDM_CONN_OUTGOING;
      mdm_set_control_lines(cfg);
      mdm_print_speed(cfg);
    } else {
      mdm_send_response(MDM_RESP_NO_CARRIER, cfg);
      usleep(cfg->disconnect_delay * 1000);
    }
  }
  return 0;
}

int mdm_listen(modem_config *cfg) {
  return line_listen(&cfg->line_data);
}

int mdm_disconnect(modem_config* cfg, unsigned char force) {
  int type;

  LOG_ENTER();
  LOG(LOG_INFO, "Disconnecting modem");
  cfg->is_cmd_mode = TRUE;
  cfg->is_off_hook = FALSE;
  cfg->break_len = 0;
  cfg->is_ringing = FALSE;
  cfg->pre_break_delay = FALSE;
  cfg->is_binary_negotiated = FALSE;
  if(cfg->direct_conn && !force) {
    LOG(LOG_INFO, "Direct connection active, maintaining link");
  } else {
    line_disconnect(&cfg->line_data);
    type = cfg->conn_type;
    cfg->conn_type = MDM_CONN_NONE;
    mdm_set_control_lines(cfg);
    if(type != MDM_CONN_NONE) {
      mdm_send_response(MDM_RESP_NO_CARRIER, cfg);
      usleep(cfg->disconnect_delay * 1000);
    } else {
      // ath0 after just off hook
      mdm_send_response(MDM_RESP_OK, cfg);
      usleep(cfg->disconnect_delay * 1000);
    }
    cfg->rings = 0;
    mdm_listen(cfg);
  }
  LOG_EXIT();
  return 0;
}

int mdm_parse_cmd(modem_config* cfg) {
  int done = FALSE;
  int index = 0;
  int num = 0;
  int start = 0;
  int end = 0;
  int cmd = AT_CMD_NONE;
  char *command = cfg->cur_line;
  int len = cfg->cur_line_idx;
  char tmp[256];

  LOG_ENTER();
  LOG(LOG_DEBUG, "Evaluating AT%s", command);

  while(TRUE != done ) {
    if(cmd != AT_CMD_ERR) {
      cmd = getcmd(command, len, &index, &num, &start, &end);
      LOG(LOG_DEBUG,
          "Command: %c (%d), Flags: %d, index=%d, num=%d, data=%d-%d",
          (cmd > -1 ? cmd & 0xff : ' '),
          cmd,
          cmd  >> 8,
          index,
          num,
          start,
          end
         );
    }
    switch(cmd) {
      case AT_CMD_ERR:
        mdm_send_response(MDM_RESP_ERROR, cfg);
        done = TRUE;
        break;
      case AT_CMD_END:
        if(cfg->is_cmd_mode == TRUE)
          mdm_send_response(MDM_RESP_OK, cfg);
        done = TRUE;
        break;
      case AT_CMD_NONE:
        done = TRUE;
        break;
      case 'O':
      case 'A':
          mdm_answer(cfg);
          cmd = AT_CMD_END;
          done = TRUE;
          break;
      case 'B':   // 212A versus V.22 connection
        if(num > 1) {
          cmd = AT_CMD_ERR;
        } else {
          //cfg->connect_1200 = num;
        }
        break;
      case 'D':
        if(end > start) {
          strncpy(cfg->dialno, (char *)command + start, end - start);
          cfg->dialno[end - start] = '\0';
          cfg->dial_type = (unsigned char)num;
          cfg->last_dial_type = (unsigned char)num;
          strncpy((char *)cfg->last_dialno, (char *)command+start, end - start);
          cfg->last_dialno[end - start] = '\0';
          cfg->memory_dial = FALSE;
        } else if (num == 'L') {
          strncpy(cfg->dialno, cfg->last_dialno, strlen(cfg->last_dialno));
          cfg->dial_type = cfg->last_dial_type;
          cfg->memory_dial = TRUE;
          mdm_write(cfg, (unsigned char *)cfg->crlf, 2);
          mdm_write(cfg, (unsigned char *)cfg->dialno, strlen(cfg->dialno));
        } else {
          cfg->dialno[0] = 0;
          cfg->last_dialno[0] = 0;
          cfg->dial_type = 0;
          cfg->last_dial_type = 0;
        }
      if (strlen(cfg->dialno) > 0) {
          mdm_connect(cfg);
        } else {
          mdm_off_hook(cfg);
          cfg->is_cmd_mode = FALSE;
        }
        done = TRUE;
        break;
      case 'E':   // still need to define #2
        if(num == 0)
          cfg->is_echo = FALSE;
        else if(num == 1)
          cfg->is_echo = TRUE;
        else {
          cmd = AT_CMD_ERR;
        }
        break;
      case 'H':
          if(num == 0) {
            mdm_disconnect(cfg, FALSE);
          } else if(num == 1) {
            mdm_off_hook(cfg);
          } else
            cmd = AT_CMD_ERR;
          break;
      case 'I':   // Information.
        break;
      case 'L':   // Speaker volume
        if(num < 1 || num > 3)
          cmd = AT_CMD_ERR;
        else {
          //cfg->volume = num;
        }
        break;
      case 'M':   // speaker settings
        if(num > 3)
          cmd=AT_CMD_ERR;
        else {
          //cfg->speaker_setting = num;
        }
        break;
      case 'N':   // automode negotiate
        if(num > 1)
          cmd=AT_CMD_ERR;
        else {
          //cfg->auto_mode=num;
        }
        break;
      case 'P':   // defaut to pulse dialing
        //cfg->default_dial_type=MDM_DT_PULSE;
        break;
      case 'Q':   // still need to define #2
        if(num == 0)
          cfg->send_responses = TRUE;
        else if(num == 1)
          cfg->send_responses = FALSE;
        else if(num == 2)  // this should be yes orig/no answer.
          cfg->send_responses = TRUE;
        else {
          cmd = AT_CMD_ERR;
        }
        break;
      case 'S':
        strncpy(tmp, command + start, end - start);
        tmp[end - start] = '\0';
        if(num < sizeof(cfg->s)) {
          cfg->s[num] = atoi(tmp); // TODO do not assume this is always a number...
          switch(num) {
            case S_REG_CR:
              cfg->crlf[0] = cfg->s[S_REG_CR];
              break;
            case S_REG_LF:
              cfg->crlf[1] = cfg->s[S_REG_LF];
              break;
          }
        } else {
          LOG(LOG_DEBUG, "Ignoring S register %d=%s", num, tmp);
        }
        break;
      case AT_CMD_FLAG_QUERY | 'S':
        sprintf(tmp, "%s%3.3d", cfg->crlf, cfg->s[num]);
        mdm_write(cfg, (unsigned char *)tmp, strlen(tmp));
        break;
      case 'T':   // defaut to tone dialing
        //cfg->default_dial_type = MDM_DT_TONE;
        break;
      case 'V':   // done
        if(num == 0)
          cfg->text_responses = FALSE;
        else if(num == 1)
          cfg->text_responses = TRUE;
        else {
          cmd=AT_CMD_ERR;
        }
        break;
      case 'W':
          if(num > -1 && num < 3) 
            cfg->connect_response = num;
          else
            cmd = AT_CMD_ERR;
          break;
      case 'X':
          if(num > -1 && num < 5) 
            cfg->response_code_level = num;
          else
            cmd = AT_CMD_ERR;
          break;
      case 'Y':   // long space disconnect.
        if(num > 1)
          cmd = AT_CMD_ERR;
        else {
          //cfg->long_disconnect = num;
        }
        break;
      case 'Z':   // long space disconnect.
        if(num > 1)
          cmd = AT_CMD_ERR;
        else {
          // set config0 to cur_line and go.
        }
        break;
      case AT_CMD_FLAG_EXT + 'C':
        switch(num) {
          case 0:
            cfg->force_dcd = TRUE;
            mdm_set_control_lines(cfg);
            break;
          case 1:
            cfg->force_dcd = FALSE;
            mdm_set_control_lines(cfg);
            break;
          default:
            cmd = AT_CMD_ERR;
            break;
        }
        break;
      case AT_CMD_FLAG_EXT + 'K':
        // flow control.
        switch (num) {
          case 0:
            dce_set_flow_control(&cfg->dce_data, 0);
            break;
          case 3:
            dce_set_flow_control(&cfg->dce_data, MDM_FC_RTS);
            break;
          case 4:
            dce_set_flow_control(&cfg->dce_data, MDM_FC_XON);
            break;
          case 5:
            dce_set_flow_control(&cfg->dce_data, MDM_FC_XON);
            // need to add passthrough..  Not sure how.
            break;
          case 6:
            dce_set_flow_control(&cfg->dce_data, MDM_FC_XON | MDM_FC_RTS);
            break;
          default:
            cmd=AT_CMD_ERR;
            break;
        }
        break;
      default:
        break;
    }
  }
  cfg->last_line_idx = cfg->cur_line_idx;
  cfg->cur_line_idx = 0;
  return cmd;
}

/*
 * interestingly, at least the hayes modem I have has the following behavior
 * concerning the AT command:
 *
 * AaaaaaT works (rule is that the first A found is considered the A char, and the
 * trailing T has the same case, so this works
 *
 * abcdefat also works (first 'a' starts the sequence, next letters starts over,
 * and then the last 'a' is paired up with the 't'
 *
 * At and aT will not work (the modem uses AT to determine parity, but mixed case
 * messes with the algorithm
 *
 * If a sequence is started (AT sent) but then the user backspaces, the backspace
 * is echoed to the sender, but then a 'T' is sent if the user has erased all of the
 * command
 *
 * A real modem will complain about incorrect data in the command string.  TCPSER
 * will silently ignore such issues, as it creates more compatibility to ignore
 * unknown commands.
 */

int mdm_handle_char(modem_config *cfg, unsigned char ch) {
  char ch_raw = ch & 0x7f;

  if(cfg->is_echo == TRUE)
    dce_write_char_raw(&cfg->dce_data, ch);
  if(cfg->is_cmd_started == TRUE) { // we previously got an 'AT'
    if(ch_raw == (cfg->s[S_REG_BS])) {
      if(cfg->cur_line_idx == 0 && cfg->is_echo == TRUE) {
        mdm_write_char(cfg, 'T');
      } else {
        cfg->cur_line_idx--;
      }
    } else if(ch_raw == (unsigned char)(cfg->s[S_REG_CR])) {
      // we have a line, process.
      cfg->cur_line[cfg->cur_line_idx] = 0;
      mdm_parse_cmd(cfg);
      cfg->first_ch = 0;
      cfg->is_cmd_started = FALSE;
    } else {
      cfg->cur_line[cfg->cur_line_idx++ % sizeof(cfg->cur_line)] = ch_raw;
    }
  } else if(cfg->first_ch) { // we already got our first char
    // if we received a 't' and the case of both chars is the same, start
    if(((ch_raw & 0x5f) == 'T') && ((cfg->first_ch & 0x20) == (ch_raw & 0x20))) {
      cfg->is_cmd_started = TRUE;
      dce_detect_parity(&cfg->dce_data, cfg->first_ch, ch);
      LOG(LOG_ALL,"'T' parsed in serial stream, switching to command parse mode");
    } else if(ch_raw == '/') {
      LOG(LOG_ALL,"'/' parsed in the serial stream, replaying last command");
      cfg->cur_line_idx = cfg->last_line_idx;
      mdm_parse_cmd(cfg);
      cfg->is_cmd_started = FALSE;
    } else if((ch_raw & 0x5f) != 'A') {
      cfg->first_ch = 0;
    }
  } else if((ch_raw & 0x5f) == 'A') {
    LOG(LOG_ALL, "'A' parsed in serial stream");
    cfg->first_ch = ch;
  }
  return 0;
}

int mdm_clear_break(modem_config* cfg) {
  cfg->break_len = 0;
  cfg->pre_break_delay = FALSE;
  return 0;
}

int mdm_handle_timeout(modem_config *cfg) {
  if(cfg->pre_break_delay == TRUE && cfg->break_len == 3) {
    // pre and post break.
    LOG(LOG_INFO, "Break condition detected");
    cfg->is_cmd_mode = TRUE;
    mdm_send_response(MDM_RESP_OK, cfg);
    mdm_clear_break(cfg);
  } else if(cfg->pre_break_delay == FALSE) {
    // pre break wait over.
    LOG(LOG_DEBUG, "Initial Break Delay detected");
    cfg->pre_break_delay = TRUE;
  } else if(cfg->pre_break_delay == TRUE && cfg->break_len > 0) {
    LOG(LOG_ALL, "Inter-break-char delay time exceeded");
    mdm_clear_break(cfg);
  } else if(cfg->s[S_REG_INACTIVITY_TIME] != 0) {
    // timeout...
    LOG(LOG_INFO, "DTE communication inactivity timeout");
    mdm_disconnect(cfg, FALSE);
  }
  return 0;
}

int mdm_send_ring(modem_config *cfg) {
  LOG(LOG_DEBUG, "Sending 'RING' to modem");
  cfg->is_ringing = TRUE;
  mdm_send_response(MDM_RESP_RING, cfg);
  cfg->rings++;
  LOG(LOG_ALL,"Sent #%d ring", cfg->rings);
  if(cfg->is_cmd_mode == FALSE || (cfg->s[S_REG_RINGS] != 0 && cfg->rings >= cfg->s[S_REG_RINGS])) {
    mdm_answer(cfg);
  }
  return 0;
}

int mdm_parse_data(modem_config *cfg, unsigned char *data, int len) {
  int i;

  if(cfg->is_cmd_mode == TRUE) {
    for(i = 0; i < len; i++) {
      mdm_handle_char(cfg, data[i]);
    }
  } else {
    line_write(&cfg->line_data, data, len);
    if(cfg->pre_break_delay == TRUE) {
      for(i = 0; i < len; i++) {
        if(dce_strip_parity(&cfg->dce_data, data[i])  == (unsigned char)cfg->s[S_REG_BREAK]) {
          LOG(LOG_DEBUG, "Break character received");
          cfg->break_len++;
          if(cfg->break_len > 3) {  // more than 3, considered invalid
            cfg->pre_break_delay = FALSE;
            cfg->break_len = 0;
          }
        } else {
          LOG(LOG_ALL, "Found non-break character, cancelling break");
          // chars past +++
          mdm_clear_break(cfg);
        }
      }
    }
  }
  return 0;
}

int mdm_read(modem_config *cfg, unsigned char *data, int len) {
  int res;

  if(cfg->is_cmd_mode == TRUE) {
    // read one char in raw mode
    res = dce_read_char_raw(&cfg->dce_data);
    if(res > 0) { // we have a character
      data[0] = (unsigned char)res; // fixup
      res = 1;
    }
  } else {
    res = dce_read(&cfg->dce_data, data, sizeof(data));
  }
  return res;
}
