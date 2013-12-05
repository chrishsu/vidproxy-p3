#include <stdlib.h>
#include <stdio.h>
#include "../src/proxy/parse_utils.h"

void print_bitrate_list(bitrate_list *bl) {
  printf("Bitrates:\t");
  while (bl != NULL) {
    printf("%d\t", bl->bitrate);
    bl = bl->next;
  }
  printf("\n\n");
}

int main() {
  printf("bitrate_list tests..\n\n");

  char *xml = "<media bitrate=\"200\"> \
              <media bitrate=\"400\"> \
              <media bitrate=\"800\"> \
              <media bitrate=\"1000\">";

  printf("parse_xml:\n");
  bitrate_list *bl = parse_xml(xml, 0);
  print_bitrate_list(bl);

  float tests[8] = { 0.0, 300.0, 700.0, 1199.0, 1199.9999,
		     1200.0001, 1201.0, 1600.0 };

  int i;
  for (i = 0; i < 8; i++) {
    printf("select [%f]: %d\n", tests[i],
           bitrate_list_select(bl, tests[i]));
  }

  bitrate_list_free(bl);

  printf("\nAll tests passed!\n");
  return 0;
}
