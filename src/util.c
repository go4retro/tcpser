#include <unistd.h>
#include <stdio.h>

#include "util.h"

int writePipe(int fd, char msg) {
  char tmp[3];

  tmp[0] = msg;
  tmp[1] = '\n';
  tmp[2] = '\0';

  //printf("Writing %c to pipe fd: %d\n",msg,fd);
  
  return write(fd, tmp, 2);
}

int readPipe(int fd, unsigned char *buf, int len) {
  return read(fd, buf, len);
}

int writeFile(char *name, int fd) {
  FILE *file;
  unsigned char buf[255];
  size_t res;
  size_t size = 1;
  size_t max = sizeof(buf);

  if(NULL != (file = fopen(name, "rb"))) {
    do {
      res = fread(buf, size, max, file);
      if(res > 0)
        res = write(fd, buf, res);
    } while (res > 0);
    fclose(file);
    return 0;
  }
  return -1;
}
