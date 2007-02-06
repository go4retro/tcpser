#include "modem_core.h"

int sh_init_config(modem_config *cfg) {
  cfg->data.connect[0]=0;
  cfg->data.no_answer[0]=0;
  return 0;
}

