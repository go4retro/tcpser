#include <pthread.h>
#include <stdlib.h>       // for exit...
#include <stdio.h>
#include <unistd.h>

#include "debug.h"
#include "util.h"

int writePipe(int fd, char msg) {
  char tmp[1];

  tmp[0] = msg;

  return write(fd, tmp, 1);
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

void spawn_thread(void * thread, void *arg, char *name) {
  int rc;
  pthread_t thread_id;

  rc = pthread_create(&thread_id, NULL, thread, arg);
  LOG(LOG_ALL, "%s thread ID = %ld", name, (long)thread_id);

  if(rc < 0) {
    ELOG(LOG_FATAL, "%s thread could not be started", name);
    exit(-1);
  }
}
