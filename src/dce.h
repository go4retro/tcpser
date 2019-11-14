#ifndef DCE_H
#define DCE_H 1

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

typedef struct dce_config {
  int port_speed;
  int is_ip232;
  unsigned char tty[256];
  int first_char;
  int fd;
  int dp[2][2];
  int sSocket;
  int ip232_is_connected;
  int ip232_dtr;
  int ip232_dcd;
  int ip232_iac;
} dce_config;

int dce_init(void);
int dce_init_config(dce_config *cfg);
int dce_init_conn(dce_config *cfg);
int dce_set_flow_control(dce_config *cfg, int opts);
int dce_set_control_lines(dce_config *cfg, int state);
int dce_get_control_lines(dce_config *cfg);
int dce_check_control_lines(dce_config *cfg);
int dce_read(dce_config *cfg, unsigned char *data, int len);
int dce_write(dce_config *cfg, unsigned char *data, int len);
//int dce_check_for_break(dce_config *cfg, char ch, int chars_left);

#endif
