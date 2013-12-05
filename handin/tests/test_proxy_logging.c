#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "../src/proxy/log.h"

int main() {
  char *filename = "tests/test.log";
  stream *s;

  printf("testing log..\n\n");

  log_init(filename);
  s = stream_init(200);

  stream_add_request(s, request_init(100, 1, 1));
  stream_request_chunksize(s, 15000);
  stream_request_complete(s);
  log_print(s, "1.0.0.1");


  stream_add_request(s, request_init(100, 1, 2));
  stream_request_chunksize(s, 17000);
  stream_request_complete(s);
  log_print(s, "1.0.0.1");
  
  stream_free(s);

  printf("passed all tests!\n");

  return 0;
}
