#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "../log.h"

int main() {
  char *filename = "tests/test.log";
  request *r;

  printf("testing log..\n\n");

  log_init(filename);

  r = request_init(100, 1, 1);
  r->chunksize = 15000;
  request_complete(r);
  log_print(r, 10.0, "1.0.0.1");
  request_free(r);


  r = request_init(100, 1, 2);
  r->chunksize = 17000;
  request_complete(r);
  log_print(r, 100.5, "1.0.0.1");
  request_free(r);

  printf("passed all tests!\n");

  return 0;
}
