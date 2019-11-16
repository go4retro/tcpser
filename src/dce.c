#include <termios.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <unistd.h>

#include "debug.h"
#include "serial.h"
#include "modem_core.h"
#include "ip232.h"      // needs modem_core.h
#include "dce.h"

void dce_init_config(dce_config *cfg) {
  cfg->parity = -1;  // parity not yet checked.
}

int detect_parity (int charA, int charT) {
  int parity, eobits;

  parity = ((charA >> 6) & 2)  | (charT >> 7);
  eobits = gen_parity(charA & 0x7f) << 1 | gen_parity(charT & 0x7f);

  if((parity == 1) || (parity == 2)) {
    if(parity == eobits)
      return PARITY_EVEN;
    else
      return PARITY_ODD;
  } else
      return parity;
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
  unsigned char *buf;
  int rc;
  int i;

  log_trace(TRACE_SERIAL_OUT, data, len);
  if (cfg->is_ip232) {
    return ip232_write(cfg, data, len);
  } else if(cfg->parity) {
    buf = malloc(len);  // TODO what if malloc fails?
    memcpy(buf, data, len);

    for (i = 0; i < len; i++) {
      buf[i] = apply_parity(data[i], cfg->parity);
    }
  } else {
    buf = data;
  }
  rc = ser_write(cfg->fd, buf, len);
  if(cfg->parity)
    free(buf);
  return rc;
}

int dce_write_char_raw(dce_config *cfg, unsigned char data) {
  int rc;

  if (cfg->is_ip232) {
    rc = ip232_write(cfg, &data, 1);
  } else {
    rc = ser_write(cfg->fd, &data, 1);
  }
  data &= 0x7f;
  log_trace(TRACE_SERIAL_OUT, &data, 1);
  return rc;
}

int dce_read(dce_config *cfg, unsigned char data[], int len) {
  int res;
  int i;

  if (cfg->is_ip232) {
    res = ip232_read(cfg, data, len);
  } else {
    res = ser_read(cfg->fd, data, len);
  }
  LOG(LOG_DEBUG, "Read %d bytes from serial port", res);
  if(-1 < res) {
    if(cfg->parity) {
      for (i = 0; i < res; i++) {
        data[i] &= 0x7f;  // strip parity from returned data
      }
    }
    log_trace(TRACE_SERIAL_OUT, data, res);
  }
  return res;
}

int dce_read_char_raw(dce_config *cfg) {
  int res;
  unsigned char data[1];
  unsigned char parity = 0;

  if (cfg->is_ip232) {
    res = ip232_read(cfg, data, 1);
  } else {
    res = ser_read(cfg->fd, data, 1);
  }
  LOG(LOG_DEBUG, "Read %d raw bytes from serial port", res);
  if(-1 < res) {
    if(cfg->parity) {
      parity = data[0] & 0x80;
      data[0] &= 0x7f;
    }
    log_trace(TRACE_SERIAL_IN, data, 1);
    if(cfg->parity)
      data[0] |= parity;  // put parity back on
  }
  return (int)data[0];
}

void dce_detect_parity(dce_config *cfg, unsigned char a, unsigned char t) {
  cfg->parity = detect_parity(a, t);
}

int dce_strip_parity(dce_config *cfg, unsigned char data) {
  return (cfg->parity ? data & 0x7f : data);
}

int dce_is_parity(dce_config *cfg) {
  return cfg->parity;
}

