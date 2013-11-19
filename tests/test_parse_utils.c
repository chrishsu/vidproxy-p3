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
  printf("parse_utils tests..\n\n");
  
  /** XML Tests **/
  char *xml_blank = "";
  char *xml_1 = "<media something bitrate=\"200\" else>";
  char *xml_2 = "<startshere><media bitrate=\"200\"><media bitrate=\"400\"></startshere>";
  bitrate_list *bl;
  
  printf("parse_xml: blank\n");
  bl = parse_xml(xml_blank);
  print_bitrate_list(bl);
  
  printf("parse_xml: 1\n");
  bl = parse_xml(xml_1);
  print_bitrate_list(bl);
  
  printf("parse_xml: 2\n");
  bl = parse_xml(xml_2);
  print_bitrate_list(bl);
  
  
  /** URI tests **/
  char *uri_fail = "GET something ELSE";
  char *uri_work = "GET /vod/200Seq1-Frag3 ELSE";
  int bitrate;
  
  printf("parse_uri: fail\n");
  printf("result: %d\n\n", parse_uri(uri_fail, &bitrate));
  
  printf("parse_uri: work\n");
  printf("result: %d\n", parse_uri(uri_fail, &bitrate));
  printf("bitrate: %d\n\n", bitrate);
  
  printf("write_uri: work\n");
  printf("start: %s\n", uri_work);
  bitrate = 1000;
  printf("result: %d\n\n", write_uri(uri_work, 28, bitrate);
  
  
  printf("All tests passed!\n");
}