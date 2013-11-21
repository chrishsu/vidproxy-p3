#include <stdlib.h>
#include <stdio.h>
#include "../parse_utils.h"

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
              <media bitrate=\"1400\">";
  
  printf("parse_xml:\n");
  bitrate_list *bl = parse_xml(xml, 0);
  print_bitrate_list(bl);
  
  printf("select [%d]: %d\n", 0,
         bitrate_list_select(bl, 0));
  printf("select [%d]: %d\n", 200,
         bitrate_list_select(bl, 200));
  printf("select [%d]: %d\n", 700,
         bitrate_list_select(bl, 700));
  printf("select [%d]: %d\n", 1600,
         bitrate_list_select(bl, 1600));
  
  printf("All tests passed!\n");
  return 0;
}