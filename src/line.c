#include "debug.h"
#include "modem_core.h"
#include "phone_book.h"
#include "ip.h"
#include "bridge.h"
#include "line.h"

int line_init_config(modem_config *cfg) {
  cfg->line_data.fd=-1;
  return 0;
}
 

int line_write(modem_config *cfg,unsigned char* data,int len) {
  return ip_write(cfg->line_data.fd,data,len);
}


int line_listen(modem_config *cfg) {
  return 0;
}

int line_off_hook(modem_config *cfg) {
  return 0;
}


int line_connect(modem_config *cfg) {
  unsigned char* addy=cfg->dialno;
  LOG(LOG_INFO,"Connecting");
  addy = pb_search(addy);
  cfg->line_data.fd=ip_connect(addy);
  if(cfg->line_data.fd > -1) {
    cfg->line_data.valid_conn = TRUE;
    return 0;
  } else {
    cfg->line_data.valid_conn = FALSE;
    return -1;
  }
}


int line_disconnect(modem_config *cfg) {
  LOG(LOG_INFO,"Disconnecting");
  cfg->line_data.is_telnet=FALSE;
  cfg->line_data.first_char=TRUE;
  if(cfg->line_data.valid_conn == TRUE) {
    ip_disconnect(cfg->line_data.fd);
    cfg->line_data.valid_conn=FALSE;
  }
  return 0;
}

