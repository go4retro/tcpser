#ifndef UTIL_H
#define UTIL_H

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif



int writePipe(int fd, char msg);
int readPipe(int fd, unsigned char *buf, int len);
int writeFile(char *name, int fd);
void spawn_thread(void * thread, void *arg, char *name);

#endif
