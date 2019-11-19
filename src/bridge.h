#ifndef BRIDGE_H
#define BRIDGE_H

#include "modem_core.h"

#define MSG_CALLING       'C'
#define MSG_BUSY          'B'
#define MSG_CONTROL_LINES 'D'
#define MSG_DISCONNECT    'H'
#define MSG_NOTIFY        'N'

int accept_connection(modem_config *);
int parse_ip_data(modem_config *cfg, unsigned char *data, int len);
void *run_bridge(void *arg);

#endif
