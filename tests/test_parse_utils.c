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
  bitrate_list_free(bl);

  printf("parse_xml: 2\n");
  bl = parse_xml(xml_2);
  print_bitrate_list(bl);
  bitrate_list_free(bl);


  /** URI tests **/
  char *uri_fail = "GET something ELSE";
  char *uri_work = "GET /vod/200Seq1-Frag3 ELSE";
  int bitrate;

  printf("parse_uri: fail\n");
  printf("result: %d\n\n", parse_uri(uri_fail, &bitrate));

  printf("parse_uri: work\n");
  printf("result: %d\n", parse_uri(uri_work, &bitrate));
  printf("bitrate: %d\n\n", bitrate);

  printf("write_uri: work\n");
  printf("start: %s\n", uri_work);
  bitrate = 1000;
  char *result = write_uri(uri_work, 28, bitrate);
  printf("result: %s\n\n", result);
  free(result);
  
  char *f4m_fail = "GET /vod/big_buck_bunny.f4";
  char *f4m_work = "GET /vod/big_buck_bunny.f4v ELSE";

  printf("parse_f4m: fail\n");
  printf("result: %d\n\n", parse_uri(f4m_fail));

  printf("parse_f4m: work\n");
  printf("result: %d\n", parse_uri(f4m_work));

  printf("write_f4m: work\n");
  printf("start: %s\n", f4m_work);
  char *result = write_uri(f4m_work, 32);
  printf("result: %s\n\n", result);
  free(result);
  
  
  printf("All tests passed!\n");
  return 0;
}
