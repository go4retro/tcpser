#ifndef NVT_H
#define NVT_H 1

typedef enum {
  NVT_SE = 240,
  NVT_NOP = 241,
  NVT_DM = 242,
  NVT_SB = 250,
  NVT_WILL = 251,
  NVT_WONT = 252,
  NVT_DO = 253,
  NVT_DONT = 254,
  NVT_IAC = 255,
  NVT_WILL_RESP = 251,
  NVT_WONT_RESP = 252,
  NVT_DO_RESP = 253,
  NVT_DONT_RESP = 254,
} nvt_command;

typedef enum {
  NVT_OPT_TRANSMIT_BINARY = 0,
  NVT_OPT_ECHO = 1,
  NVT_OPT_SUPPRESS_GO_AHEAD = 3,
  NVT_OPT_STATUS = 5,
  NVT_OPT_RCTE = 7,
  NVT_OPT_TIMING_MARK = 6,
  NVT_OPT_NAOCRD = 10,
  NVT_OPT_TERMINAL_TYPE = 24,
  NVT_OPT_NAWS = 31,
  NVT_OPT_TERMINAL_SPEED = 32,
  NVT_OPT_LINEMODE = 34,
  NVT_OPT_X_DISPLAY_LOCATION = 35,
  NVT_OPT_ENVIRON = 36,
  NVT_OPT_NEW_ENVIRON = 39,
} nvt_option;

#define NVT_SB_IS 0
#define NVT_SB_SEND 1

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#include "dce.h"

typedef struct nvt_vars {
  int binary_xmit;
  int binary_recv;
  unsigned char term[256];
} nvt_vars;

void nvt_init_config(nvt_vars *vars);
unsigned char get_nvt_cmd_response(unsigned char action, unsigned char type);
int parse_nvt_subcommand(dce_config *cfg, int fd, nvt_vars *vars , unsigned char *data, int len);
int parse_nvt_command(dce_config *cfg, int fd, nvt_vars *vars, nvt_command action, nvt_option opt);
int send_nvt_command(int fd, nvt_vars *vars, nvt_command action, nvt_option opt);

#endif
