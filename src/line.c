#include "debug.h"
#include "modem_core.h"
#include "phone_book.h"
#include "ip.h"
#include "bridge.h"
#include "line.h"


void reset_config(line_config *cfg) {
  cfg->fd = -1;
  cfg->is_telnet = FALSE;
  cfg->is_data_received = FALSE;
  cfg->is_connected = FALSE;
  nvt_init_config(&cfg->nvt_data);
}

void line_init_config(line_config *cfg) {
  reset_config(cfg);
}

int line_read(line_config *cfg, unsigned char *data, int len) {
  int retval;

  // should do escaping in here, like we do for writes below
  retval = ip_read(cfg->fd, data, len);
  if(0 > retval)
    log_trace(TRACE_IP_IN, data, len);
  return retval;
}

int line_write(line_config *cfg, unsigned char* data, int len) {
  int retval;
  int i = 0;
  int double_iac = FALSE;
  unsigned char text[1024];
  int text_len = 0;
  int mask = 0x7f;

  if(cfg->is_telnet) {
    if(cfg->nvt_data.binary_xmit) {
      mask = 0xff;
    }
    retval = 0;
    while(i < len) {
      if (double_iac) {
        text[text_len++] = NVT_IAC;
        double_iac = FALSE;
        i++;
      } else {
        if(NVT_IAC == data[i]) {
          text[text_len++] = NVT_IAC;
          double_iac = TRUE;
        } else {
          text[text_len++] = data[i++] & mask;
        }
      }
      if(text_len == sizeof(text)) {
        retval = ip_write(cfg->fd, text, text_len);
        text_len = 0;
      }
    }
    if(text_len) {
      retval = ip_write(cfg->fd, text, text_len);
    }
    return retval;
  }

  return ip_write(cfg->fd, data, len);
}

int line_listen(line_config *cfg) {
  return 0;
}

int line_accept(line_config *cfg) {
  cfg->fd = ip_accept(cfg->sfd);
  if(cfg->fd > -1) {
    LOG(LOG_ALL, "Connection accepted");
    cfg->is_connected = TRUE;
    return 0;
  }
  LOG(LOG_ALL, "Could not accept connection");
  return -1;
}

int line_off_hook(line_config *cfg) {
  return 0;
}

int line_connect(line_config *cfg, char *addy) {
  LOG(LOG_INFO, "Connecting line");
  addy = pb_search(addy);
  cfg->fd = ip_connect(addy);
  if(cfg->fd > -1) {
    LOG(LOG_ALL, "Connected to %s", addy);
    cfg->is_connected = TRUE;
    return 0;
  } else {
    LOG(LOG_ALL, "Could not connect to %s",addy);
    return -1;
  }
}

// TODO Need to rationalize where direct_conn flag sits...
int line_disconnect(line_config *cfg, int direct_conn) {
  LOG(LOG_INFO, "Disconnecting line");
  if(direct_conn == TRUE) {
    LOG(LOG_INFO, "Direct connection active, maintaining link");
    return -1;
  } else {
    if(cfg->is_connected == TRUE) {
      ip_disconnect(cfg->fd);
    }
    reset_config(cfg);
  }
  return 0;
}
