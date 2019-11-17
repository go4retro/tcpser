#ifndef DCE_H
#define DCE_H 1

#define DCE_CL_DSR 1
#define DCE_CL_DCD 2
#define DCE_CL_CTS 4
#define DCE_CL_DTR 8
#define DCE_CL_LE 16

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

/* This is a cool piece of code found by Chris Osborn (fozztexx@fozztexx.com) from
 * https://graphics.stanford.edu/~seander/bithacks.html#ParityWith64Bits  that
 * computes even parity for any 8 bit data value
 */
#ifndef gen_parity
#  define gen_parity(v) ((unsigned char)(((((v) * 0x0101010101010101ULL) & 0x8040201008040201ULL) % 0x1FF) & 1))
#endif

/* This is an impressive piece of code written by Chris Osborn (fozztexx@fozztexx.com)
 * that given a value where SPACE=0,ODD=1,EVEN=2,MARK=3 parity is present in p,
 * it will quickly add the correct parity to a 7 bit data value
 */
#ifndef apply_parity
#  define apply_parity(v, p) ((unsigned char)((v & 0x7f) | (((p >> gen_parity(v & 0x7f))) & 1) << 7))
#endif

enum {
  PARITY_SPACE_NONE = 0,
  PARITY_ODD,
  PARITY_EVEN,
  PARITY_MARK
};

typedef struct dce_config {
  int port_speed;
  int parity;
  int is_ip232;
  char tty[256];
  int fd;
  int dp[2][2];
  int sSocket;
  int is_connected;
  int ip232_dtr;
  int ip232_dcd;
  int ip232_iac;
} dce_config;

void dce_init_config(dce_config *cfg);
int dce_connect(dce_config *cfg);
int dce_set_flow_control(dce_config *cfg, int opts);
int dce_set_control_lines(dce_config *cfg, int state);
int dce_get_control_lines(dce_config *cfg);
int dce_check_control_lines(dce_config *cfg);
int dce_write(dce_config *cfg, unsigned char *data, int len);
int dce_write_char_raw(dce_config *cfg, unsigned char data);
int dce_read(dce_config *cfg, unsigned char *data, int len);
int dce_read_char_raw(dce_config *cfg);
void dce_detect_parity(dce_config *cfg, unsigned char a, unsigned char t);
int dce_strip_parity(dce_config *cfg, unsigned char data);
int dce_get_parity(dce_config *cfg);
//int dce_check_for_break(dce_config *cfg, char ch, int chars_left);

#endif
