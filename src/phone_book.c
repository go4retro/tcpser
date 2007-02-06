#include <stdio.h>
#include <string.h> 
#include "phone_book.h"

#define PBSIZE 100

unsigned char phone_book[PBSIZE][2][128];
int size=0;

int pb_init() {
  return 0;
}

int pb_add(unsigned char* from,unsigned char* to) {
  if(size < PBSIZE 
     && from != NULL
     && to != NULL
     && strlen(from) > 0
     && strlen(to) > 0
    ) {
    // should really trim spaces.
    strncpy(phone_book[size][0],from,sizeof(phone_book[size][0]));
    strncpy(phone_book[size][1],to,sizeof(phone_book[size][1]));
    size++;
    return 0;
  }
  return -1;
}

unsigned char* pb_search(unsigned char *number) {
  int i=0;
  for(i=0;i<size;i++) {
    if(strcmp(phone_book[i][0],number) == 0) {
      printf("Found a match\n");
      strcpy(number,phone_book[i][1]);
      break;
    }
  }
  return number;
}

