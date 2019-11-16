#include "debug.h"
#include "modem_core.h"
#include "phone_book.h"
#include "ip.h"
#include "bridge.h"
#include "line.h"

void line_init_config(line_config *cfg) {
  cfg->fd = -1;
  cfg->is_telnet = FALSE;
  cfg->first_char = TRUE;
  cfg->valid_conn = FALSE;
  nvt_init_config(&cfg->nvt_data);
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
    cfg->valid_conn = TRUE;
    return 0;
  }
  LOG(LOG_ALL, "Could not accept connection");
  return -1;
}

int line_off_hook(line_config *cfg) {
  return 0;
}

int line_connect(line_config *cfg, char *addy) {
  LOG(LOG_INFO, "Connecting");
  addy = pb_search(addy);
  cfg->fd = ip_connect(addy);
  if(cfg->fd > -1) {
    LOG(LOG_ALL, "Connected to %s", addy);
    cfg->valid_conn = TRUE;

    /* we need to let the other end know that our end will
     * handle the echo - otherwise "true" telnet clients like
     * those that come with Linux & Windows will echo characters
     * typed and you'll end up with doubled characters if the remote
     * host is echoing as well...
     * - gwb
     */
    // Make sure that we've got a telnet session before we send out
    // the negotiation sequence to turn off remote echo.
    if (cfg->is_telnet)
      send_nvt_command(cfg->fd, &cfg->nvt_data, NVT_WILL, NVT_OPT_ECHO);


    /* If space parity is detected treat it as 8 bit and try to enable binary mode */
    // TODO add this in.  Not sure how
    //if (!cfg->parity) {
    //  send_nvt_command(cfg->line_data.fd, &cfg->line_data.nvt_data,
		//       NVT_WILL, NVT_OPT_TRANSMIT_BINARY);
    //  send_nvt_command(cfg->line_data.fd, &cfg->line_data.nvt_data,
		//       NVT_DO, NVT_OPT_TRANSMIT_BINARY);
    //}
    return 0;
  } else {
    LOG(LOG_ALL, "Could not connect to %s",addy);
    return -1;
  }
}

// TODO Need to rationalize where direct_conn flag sits...
int line_disconnect(line_config *cfg, int direct_conn) {
  LOG(LOG_INFO, "Disconnecting");
  if(direct_conn == TRUE) {
    LOG(LOG_INFO, "Direct connection active, maintaining link");
    return -1;
  } else {
    cfg->is_telnet = FALSE;
    cfg->first_char = TRUE;
    if(cfg->valid_conn == TRUE) {
      ip_disconnect(cfg->fd);
      cfg->valid_conn = FALSE;
    }
  }
  return 0;
}
