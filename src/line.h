#ifndef LINE_H
#define LINE_H 1

#include "nvt.h"

typedef struct line_config {
  int fd;
  int sfd;
  int is_connected;
  int is_telnet;
  int is_data_received;
  nvt_vars nvt_data;
} line_config;

void line_init_config(line_config *cfg);
int line_init_conn(line_config *cfg);
int line_read(line_config *cfg, unsigned char *data, int len);
int line_write(line_config *cfg, unsigned char *data, int len);
int line_listen(line_config *cfg);
int line_accept(line_config *cfg);
int line_off_hook(line_config *cfg);
int line_connect(line_config *cfg, char* dialno);
int line_disconnect(line_config *cfg, int direct_conn);

#endif
