#include <stdio.h>
#include <stdlib.h>       // for exit,atoi
#include <unistd.h>
#include "version.h"
#include "debug.h"
#include "phone_book.h"
#include "init.h"

void print_help(char* name) {
  fprintf(stderr, "Usage: %s <parameters>\n", name);
  fprintf(stderr, "  -p   tcp port (or address:port) to listen on (defaults to 6400)\n");
  fprintf(stderr, "  -t   trace flags: (can be combined)\n");
  fprintf(stderr, "       'm' = modem input\n");
  fprintf(stderr, "       'M' = modem output\n");
  fprintf(stderr, "       's' = serial input\n");
  fprintf(stderr, "       'S' = serial output\n");
  fprintf(stderr, "       'i' = IP input\n");
  fprintf(stderr, "       'I' = IP output\n");
  fprintf(stderr, "  -l   0 (NONE), 1 (FATAL) - 7 (DEBUG_X) (defaults to 0)\n");
  fprintf(stderr, "  -L   log file (defaults to stderr)\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "  The following can be repeated for each modem desired\n");
  fprintf(stderr, "  (-s, -S, and -i will apply to any subsequent device if not set again)\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "  -d   serial device (e.g. /dev/ttyS0). Cannot be used with -v\n");
  fprintf(stderr, "  -v   tcp port (or address:port) for virtual RS232. Cannot be used with -d\n");
  fprintf(stderr, "  -s   serial port speed (defaults to 38400)\n");
  fprintf(stderr, "  -S   speed modem will report (defaults to -s value)\n");
  fprintf(stderr, "  -I   invert DCD pin\n");
  fprintf(stderr, "  -n   add phone entry (number=replacement)\n");
  fprintf(stderr, "  -a   filename to send to local side upon answer\n");
  fprintf(stderr, "  -A   filename to send to remote side upon answer\n");
  fprintf(stderr, "  -c   filename to send to local side upon connect\n");
  fprintf(stderr, "  -C   filename to send to remote side upon connect\n");
  fprintf(stderr, "  -N   filename to send when no answer\n");
  fprintf(stderr, "  -B   filename to send when modem(s) busy\n");
  fprintf(stderr, "  -T   filename to send upon inactivity timeout\n");
  fprintf(stderr, "  -i   modem init string (defaults to '', leave off 'at' prefix when specifying)\n");
  fprintf(stderr, "  -D   direct connection (follow with hostname:port for caller, : for receiver)\n");
  fprintf(stderr, "  -V   print version number\n");
  exit(0);
}

int init(int argc,
         char **argv,
         modem_config cfg[],
         int max_modem,
         char **ip_addr, 
         char *all_busy,
         int all_busy_len
         ) {
  int i = 0;
  int j = 0;
  int opt = 0;
  int trace_flags = 0;
  char *tok;
  int dce_set = FALSE;
  int tty_set = FALSE;

  LOG_ENTER();
  mdm_init_config(&cfg[0]);
  cfg[0].dce_data.port_speed = 38400;
  cfg[0].line_speed = 38400;

  while(opt>-1 && i < max_modem) {
    opt=getopt(argc, argv, "p:s:S:d:v:hw:i:Il:L:t:n:a:A:c:C:N:B:T:D:V");
    switch(opt) {
      case 't':
        trace_flags = log_get_trace_flags();
        for(j = 0; j < strlen(optarg); j++) {
          switch(optarg[j]) {
            case 'm':
              trace_flags |= TRACE_MODEM_IN;
              break;
            case 'M':
              trace_flags |= TRACE_MODEM_OUT;
              break;
            case 's':
              trace_flags |= TRACE_SERIAL_IN;
              break;
            case 'S':
              trace_flags |= TRACE_SERIAL_OUT;
              break;
            case 'i':
              trace_flags |= TRACE_IP_IN;
              break;
            case 'I':
              trace_flags |= TRACE_IP_OUT;
              break;
          }
          log_set_trace_flags(trace_flags);
        }
        break;
      case 'a':
        strncpy(cfg[i].local_answer, optarg, sizeof(cfg[i].local_answer));
        break;
      case 'A':
        strncpy(cfg[i].remote_answer, optarg, sizeof(cfg[i].remote_answer));
        break;
      case 'c':
        strncpy(cfg[i].local_connect, optarg, sizeof(cfg[i].local_connect));
        break;
      case 'C':
        strncpy(cfg[i].remote_connect, optarg, sizeof(cfg[i].remote_connect));
        break;
      case 'B':
        strncpy(all_busy, optarg, all_busy_len);
        break;
      case 'N':
        strncpy(cfg[i].no_answer, optarg, sizeof(cfg[i].no_answer));
        break;
      case 'T':
        strncpy(cfg[i].inactive, optarg, sizeof(cfg[i].inactive));
        break;
      case 'i':
        strncpy(cfg[i].cur_line, optarg, sizeof(cfg[i].cur_line));
        cfg[i].cur_line_idx = strlen(cfg[i].cur_line);
        break;
      case 'I':
        cfg[i].invert_dcd = TRUE;
        break;
      case 'p':
        *ip_addr = optarg;
        break;
      case 'n':
        tok = strtok(optarg, "=");
        pb_add(tok, strtok(NULL, "="));
        break;
      case 'l':
        log_set_level(atoi(optarg));
        break;
      case 'L':
        log_set_file(fopen(optarg, "w+"));
        // should check to see if an error occurred...
        break;
      case 's':
        cfg[i].dce_data.port_speed = atoi(optarg);
        LOG(LOG_ALL, "Setting DTE speed to %d", cfg[i].dce_data.port_speed);
        if(dce_set == FALSE)
          cfg[i].line_speed = cfg[i].dce_data.port_speed;
        break;
      case '?':
      case 'h':
        print_help(argv[0]);
        break;
      case 'd':
      case 'v':
        if (tty_set) {
          if (++i < max_modem) {
            dce_set = FALSE;
            mdm_init_config(&cfg[i]);
            cfg[i].dce_data.port_speed = cfg[i - 1].dce_data.port_speed;
            cfg[i].line_speed = cfg[i - 1].line_speed;
            cfg[i].dce_data.is_ip232 = FALSE;
            strncpy((char *)cfg[i].cur_line, (char *)cfg[i - 1].cur_line, sizeof(cfg[i].cur_line));
            strncpy((char *)cfg[i].local_connect, (char *)cfg[i - 1].local_connect, sizeof(cfg[i].local_connect));
            strncpy((char *)cfg[i].remote_connect, (char *)cfg[i - 1].remote_connect, sizeof(cfg[i].remote_connect));
            strncpy((char *)cfg[i].local_answer, (char *)cfg[i - 1].local_answer, sizeof(cfg[i].local_answer));
            strncpy((char *)cfg[i].remote_answer, (char *)cfg[i - 1].remote_answer, sizeof(cfg[i].remote_answer));
            strncpy((char *)cfg[i].no_answer, (char *)cfg[i - 1].no_answer, sizeof(cfg[i].no_answer));
            strncpy((char *)cfg[i].inactive, (char *)cfg[i - 1].inactive, sizeof(cfg[i].inactive));
          } else {
            LOG(LOG_WARN, "Maximum modems defined - ignoring extra");
            break;
          }
        }
        strncpy((char *)cfg[i].dce_data.tty, optarg, sizeof(cfg[i].dce_data.tty));
        LOG(LOG_ALL, "Setting TTY to %s", optarg);
        cfg[i].dce_data.is_ip232 = ('v' == opt);
        tty_set = TRUE;
        break;
      case 'S':
        cfg[i].line_speed = atoi(optarg);
        dce_set = TRUE;
        break;
      case 'D':
        cfg[i].direct_conn = TRUE;
        strncpy(cfg[i].direct_conn_num, optarg, sizeof(cfg[i].direct_conn_num));
        break;
      case 'V':
        printf("%s (TCPSER) %s\n", argv[0], VERSION);
        printf("Copyright (C) 2004-%s Jim Brain (tcpser@jbrain.com)\n", __DATE__ + 7);
        exit(0);
        break;
    }
  }

  if (tty_set) {
    if (i < max_modem)
      i++;
  } else {
    // no modems defined
    LOG(LOG_FATAL, "No modems defined");
    print_help(argv[0]);
  }

  LOG(LOG_DEBUG, "Read configuration for %i serial port(s)", i);

  LOG_EXIT();
  return i;
}
