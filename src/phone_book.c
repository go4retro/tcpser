#include <stdio.h>
#include <string.h>
#include "phone_book.h"
#include "debug.h"

char phone_book[PH_BOOK_SIZE][2][PH_ENTRY_SIZE];
int size = 0;

int pb_init() {
  size = 0;
  return 0;
}

int pb_add(char* from, char* to) {
  LOG_ENTER();
  if(size < PH_BOOK_SIZE
     && from != NULL
     && to != NULL
     && strlen(from) > 0
     && strlen(to) > 0
    ) {
    // should really trim spaces.
    strncpy(phone_book[size][0], from, PH_ENTRY_SIZE);
    strncpy(phone_book[size][1], to, PH_ENTRY_SIZE);
    size++;
    LOG_EXIT();
    return 0;
  }
  LOG_EXIT();
  return -1;
}

int pb_search(char *number, char *address) {
  int i=0;

  LOG_ENTER();
  strcpy(address, number);
  for(i = 0; i < size; i++) {
    LOG(LOG_INFO, "Searching entry %d of %d", i, size);
    if(strcmp(phone_book[i][0], number) == 0) {
      LOG(LOG_INFO, "Found a match for '%s': '%s'", number, phone_book[i][1]);
      strncpy(address, phone_book[i][1], PH_ENTRY_SIZE);
      break;
    }
  }
  LOG_EXIT();
  return 0;
}
