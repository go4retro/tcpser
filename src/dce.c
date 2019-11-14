#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "debug.h"
#include "serial.h"
#include "modem_core.h"
#include "ip232.h"      // needs modem_core.h
#include "dce.h"

int dce_init_config(dce_config *cfg) {
  return 0;
}

int dce_init_conn(dce_config *cfg) {
  int rc;

  LOG_ENTER();
  if (cfg->is_ip232) {
    rc = ip232_init_conn(cfg);
  } else {
    rc = ser_init_conn(cfg->tty, cfg->port_speed);
    cfg->fd = rc;
  }

  LOG_EXIT();
  return rc;
}

int dce_set_flow_control(dce_config *cfg, int opts) {
  int status = 0;
  int rc = 0;

  LOG_ENTER();
  if(opts == 0) {
    LOG(LOG_ALL, "Setting NONE flow control");
  } else {
    if((opts & MDM_FC_RTS) != 0) {
      LOG(LOG_ALL, "Setting RTSCTS flow control");
      status |= CRTSCTS;
    }
    if((opts & MDM_FC_XON) != 0) {
      status |= (IXON | IXOFF);
      LOG(LOG_ALL, "Setting XON/XOFF flow control");
    }
  }

  if (cfg->is_ip232) {
    rc = ip232_set_flow_control(cfg, status);
  } else {
    rc = ser_set_flow_control(cfg->fd, status);
  }

  LOG_EXIT()
  return rc;
}

int dce_set_control_lines(dce_config *cfg, int state) {
  int status = 0;
  int rc;

  LOG_ENTER();
  if((state & MDM_CL_CTS_HIGH) != 0) {
    LOG(LOG_ALL, "Setting CTS pin high");
    status |= TIOCM_RTS;
  } else {
    LOG(LOG_ALL, "Setting CTS pin low");
    //status &= ~TIOCM_RTS;
  }
  if((state & MDM_CL_DCD_HIGH) != 0) {
    LOG(LOG_ALL, "Setting DCD pin high");
    status |= TIOCM_DTR;
  } else {
    LOG(LOG_ALL, "Setting DCD pin low");
    //status &= ~TIOCM_DTR;
  }

  if (cfg->is_ip232) {
    rc = ip232_set_control_lines(cfg, status);
  } else {
    rc = ser_set_control_lines(cfg->fd, status);
  }

  LOG_EXIT();
  return rc;
}

int dce_get_control_lines(dce_config *cfg) {
  int status;
  int rc_status;

  if (cfg->is_ip232) {
    status = ip232_get_control_lines(cfg);
  } else {
    status = ser_get_control_lines(cfg->fd);
  }

  if(status > -1) {
    rc_status = ((status & TIOCM_DSR) != 0 ? MDM_CL_DTR_HIGH : 0);
  } else {
    rc_status = status;
  }

  return rc_status;
}

int dce_check_control_lines(dce_config *cfg) {
  int status = 0;
  int new_status = 0;

  LOG_ENTER();
  status = dce_get_control_lines(cfg);
  new_status = status;
  while(new_status > -1 && status == new_status) {
    usleep(100000);
    new_status = dce_get_control_lines(cfg);
  }

  LOG_EXIT();
  return new_status;
}

int dce_write(dce_config *cfg, unsigned char data[], int len) {
  if (cfg->is_ip232) {
    return ip232_write(cfg, data, len);
  }
  return ser_write(cfg->fd, data, len);
}

int dce_read(dce_config *cfg, unsigned char data[], int len) {
  if (cfg->is_ip232) {
    return ip232_read(cfg, data, len);
  }
  return ser_read(cfg->fd, data, len);
}
