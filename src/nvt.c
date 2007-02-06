#include <string.h>

#include "ip.h"
#include "nvt.h"

unsigned char get_nvt_cmd_response(unsigned char action, unsigned char type) {
  unsigned char rc=0;

  if(type == TRUE) {
    switch (action) {
      case NVT_DO:
        rc=NVT_WILL_RESP;
        break;
      case NVT_DONT:
        rc=NVT_WONT_RESP;
        break;
      case NVT_WILL:
        rc=NVT_DO_RESP;
        break;
      case NVT_WONT:
        rc=NVT_DONT_RESP;
        break;
    }
  } else {
    switch (action) {
      case NVT_DO:
      case NVT_DONT:
        rc=NVT_WONT_RESP;
        break;
      case NVT_WILL:
      case NVT_WONT:
        rc=NVT_DONT_RESP;
        break;
    }
  }
  return rc;
}

int parse_nvt_subcommand(int fd, nvt_vars vars , unsigned char * data, int len) {
  // overflow issue, again...
  int opt=data[2];
  unsigned char resp[100];
  int resp_len=0;
  unsigned char tty_type[]="VT100";
  int rc=0;
  int slen=0;

  resp[0]=NVT_IAC;
  resp[1]=NVT_SB;
  resp[2]=opt;
  resp[3]=0;

  switch(opt) {
    case NVT_OPT_TERMINAL_TYPE:
      slen=strlen(tty_type);
      strncpy(resp + 4,tty_type, slen);
      resp_len=slen+4;
      rc=6;
      break;
    default:
      resp_len=4;
      rc=6;
      break;

  }
  if(resp_len > 0)
    resp[resp_len++]=NVT_IAC;
    resp[resp_len++]=NVT_SE;
    ip_write(fd,resp,resp_len);
  return rc;
}


int parse_nvt_command(int fd, unsigned char action, unsigned char opt) {
  unsigned char resp[3];
  resp[0]= NVT_IAC;
  resp[2]= opt;

  switch (opt) {
    case NVT_OPT_NAWS:
    case NVT_OPT_TERMINAL_TYPE:
    case NVT_OPT_SUPPRESS_GO_AHEAD:
    case NVT_OPT_ECHO:
    case NVT_OPT_X_DISPLAY_LOCATION:  // should not have to have these
    case NVT_OPT_ENVIRON:             // but telnet seems to expect.
    case NVT_OPT_NEW_ENVIRON:         // them.
    case NVT_OPT_TERMINAL_SPEED:
      resp[1] = get_nvt_cmd_response(action,TRUE);
      break;
    default:
      resp[1] = get_nvt_cmd_response(action,FALSE);
      break;
  }
  ip_write(fd,resp,3);
  return 0;
}



