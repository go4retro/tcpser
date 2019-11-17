#ifndef IP232_H
#define IP232_H 1

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#define MSG_CALLING '+'
#define MSG_BUSY '+'

int ip232_init_conn(dce_config *);
int ip232_set_flow_control(dce_config *, int status);
int ip232_get_control_lines(dce_config *);
int ip232_set_control_lines(dce_config *, int state);
int ip232_write(dce_config *, unsigned char *data, int len);
int ip232_read(dce_config *, unsigned char *data, int len);

#endif
