#ifndef SERIAL_H
#define SERIAL_H 1

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

int dce_get_bps_const(int speed);
int dce_init(char* tty, int speed);
int dce_watch_control_lines(int tty_fd, int pipe);

#endif
