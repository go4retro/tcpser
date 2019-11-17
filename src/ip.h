#ifndef IP_H
#define IP_H

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

int ip_init(void);
int ip_init_server_conn(char *ip);
int ip_connect(char *ip);
int ip_accept(int sSocket);
int ip_disconnect(int fd);
int ip_write(int fd, unsigned char *data, int len);
int ip_read(int fd, unsigned char *data, int len);

#endif
