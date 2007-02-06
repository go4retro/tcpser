#ifndef IP_H
#define IP_H

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#include "modem_data.h"

int ip_init(int port);
int ip_connect(char addy[]);
int ip_accept(int sSocket);

#endif

