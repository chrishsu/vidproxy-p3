#include <stdlib.h>
#include <stdio.h>
#include "../src/parse_utils.h"

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

  /** XML tests **/
  char *xml_blank = "";
  char *xml_1 = "<media something bitrate=\"200\" else>";
  char *xml_2 = "<startshere><media bitrate=\"200\"><media bitrate=\"400\"></startshere>";
  bitrate_list *bl;

  printf("parse_xml: blank\n");
  bl = parse_xml(xml_blank, 100);
  print_bitrate_list(bl);

  printf("parse_xml: 1\n");
  bl = parse_xml(xml_1, 100);
  print_bitrate_list(bl);
  bitrate_list_free(bl);

  printf("parse_xml: 2\n");
  bl = parse_xml(xml_2, 100);
  print_bitrate_list(bl);
  bitrate_list_free(bl);

  /** Headers tests **/
  char *header_fail = "stuf stuff stuff";
  char *header_1 = "Content-Length: 9001";
  char *header_2 = "Content-Type: text/html\r\nContent-Length: 9001\r\nMore Stuff\r\n\r\n";
  int length = 0;

  printf("parse_headers: fail\n");
  printf("result: %d\n\n", parse_headers(header_fail, 100, &length));
  printf("parse_headers: 1\n");
  printf("result: %d\n", parse_headers(header_1, 100, &length));
  printf("length: %d\n\n", length);

  printf("parse_headers: 2\n");
  printf("result: %d\n", parse_headers(header_2, 100, &length));
  printf("length: %d\n\n", length);

  /** URI tests **/
  char *uri_fail = "GET something ELSE";
  char uri_work[28+25] = "GET /vod/200Seg1-Frag3 ELSE";
  int bitrate, seq, frag;

  printf("parse_uri: fail\n");
  printf("result: %d\n\n", parse_uri(uri_fail, 100, &bitrate, &seq, &frag));

  printf("parse_uri: work\n");
  printf("result: %d\n", parse_uri(uri_work, 100, &bitrate, &seq, &frag));
  printf("bitrate: %d\n", bitrate);
  printf("seq: %d\n", seq);
  printf("frag: %d\n\n", frag);

  printf("write_uri: work\n");
  printf("start: %s\n", uri_work);
  bitrate = 1000;
  int buf_len = 28;
  replace_uri(uri_work, &buf_len, bitrate);
  printf("result: %s\n\n", uri_work);

  char *f4m_fail = "GET /vod/big_buck_bunny.f4v";
  char f4m_work[33+25] = "GET /vod/big_buck_bunny.f4m ELSE";

  printf("parse_f4m: fail\n");
  printf("result: %d\n\n", parse_f4m(f4m_fail, 100));

  printf("parse_f4m: work\n");
  printf("result: %d\n\n", parse_f4m(f4m_work, 33));

  printf("write_f4m: work\n");
  printf("start: %s\n", f4m_work);
  replace_f4m(f4m_work, 33);
  printf("result: %s\n\n", f4m_work);


  printf("All tests passed!\n");
  return 0;
}
