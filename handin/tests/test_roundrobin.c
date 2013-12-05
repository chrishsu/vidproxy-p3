#include <stdlib.h>
#include <stdio.h>
#include "../src/dns/roundrobin.h"

int main() {
  printf("roundrobin tests..\n\n");

  printf("parsing server:\n");
  rr_parse_servers("tests/test_roundrobin_file.txt");

  printf("looping through servers:\n");
  int i = 0;
  while (i < 10) {
    printf("%s\n", rr_next_server());
    i++;
  }

  rr_free();

  printf("\nAll tests passed!\n");
  return 0;
}
