#ifndef MODEM_DATA_H
#define MODEM_DATA_H 1

#include "nvt.h"   // a bit kludgey....

typedef struct dce_config {
  char tty[255];
  int fd;
} dce_config;

typedef struct line_config {
  int valid_conn;
  int fd;
  int sfd;
  int is_telnet;
  int first_char;
  nvt_vars nvt_data;
} line_config;

typedef struct x_config {
  int mp[2][2];
  int cp[2][2];
  int wp[2][2];
  char no_answer[255];
  char connect[255];
  char inactive[255];
} x_config;
#endif
