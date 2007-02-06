#include <termios.h>
#include <sys/ioctl.h>

#include "debug.h"
#include "serial.h"
#include "modem_core.h"
#include "dce.h"

int dce_init_config(modem_config *cfg) {
  return 0;
}


int dce_init_conn(modem_config* cfg) {
  LOG_ENTER();
  cfg->dce_data.fd=ser_init_conn(cfg->dce_data.tty,cfg->dte_speed);
  LOG_EXIT();
  return cfg->dce_data.fd;
}


int dce_set_flow_control(modem_config *cfg,int opts) {
  int status=0;
  int rc=0;

  LOG_ENTER();
  if(MDM_FC_RTS) {
    status |= CRTSCTS;
  }
  if(MDM_FC_XON) {
    status |= (IXON | IXOFF);
  }

  rc=ser_set_flow_control(cfg->dce_data.fd,status);

  LOG_EXIT()
  return rc;
}

int dce_set_control_lines(modem_config *cfg,int state) {
  int status;
  int rc;

  LOG_ENTER();
  if((state & MDM_CL_CTS_HIGH) != 0) {
    LOG(LOG_ALL,"Setting CTS pin high");
    status |= TIOCM_RTS;
  } else {
    LOG(LOG_ALL,"Setting CTS pin low");
    status &= ~TIOCM_RTS;
  }
  if((state & MDM_CL_DCD_HIGH) != 0) {
    LOG(LOG_ALL,"Setting DCD pin high");
    status |= TIOCM_DTR;
  } else {
    LOG(LOG_ALL,"Setting DCD pin low");
    status &= ~TIOCM_DTR;
  }
  rc = ser_set_control_lines(cfg->dce_data.fd,status);

  LOG_EXIT();
  return rc;
}

int dce_get_control_lines(modem_config *cfg) {
  int status;
  int rc_status;

  status = ser_get_control_lines(cfg->dce_data.fd);

  rc_status=((status & TIOCM_DSR) != 0?MDM_CL_DTR_HIGH:0);

  return rc_status;
}

int dce_check_control_lines(modem_config *cfg) {
  int status=0;
  int new_status=0;

  LOG_ENTER();
  status = dce_get_control_lines(cfg);
  new_status = status;
  while(status == new_status) {
    usleep(100000);
    new_status = dce_get_control_lines(cfg);
  }

  LOG_EXIT();
  return new_status;
}


int dce_write(modem_config *cfg,char data[], int len) {
  return ser_write(cfg->dce_data.fd,data,len);
}

int dce_read(modem_config *cfg, char data[], int len) {
  return ser_read(cfg->dce_data.fd,data,len);
}


