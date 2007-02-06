#define AT_CMD_END -1
#define AT_CMD_ERR -2
#define AT_CMD_NONE -4
#define AT_CMD_IERR -3
#define AT_CMD_FLAG_BAS 0
#define AT_CMD_FLAG_EXT 256
#define AT_CMD_FLAG_PRO 512

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

int getData(unsigned char line[], 
            int* index, 
            int len, 
            int* data_start,
            int* data_end,
            int simple_parse
           );
int getNumber(unsigned char line[], int* index, int len );
int skip(unsigned char line[], int* index, int len, unsigned char ch);
int getCommand(unsigned char line[], 
               int flags, 
               int* index, 
               int* num, 
               int len 
               );
int parseCommand(unsigned char line[], 
               int flags, 
               int* index, 
               int* num, 
               int len 
               );
int parseRegister(unsigned char line[], 
                int flags, 
                int* index, 
                int* num, 
                int len, 
                int* data_start, 
                int* data_end,
                int simple_parse
               );
int getcmd(unsigned char line[], 
           int* index, 
           int* num, 
           int* data_start, 
           int* data_end
          );
