
#ifndef VP_PARSER
#define VP_PARSER

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct bitrate_list {
  int bitrate;
  struct bitrate_list *next;
} bitrate_list;

void bitrate_list_free(bitrate_list *bl);
bitrate_list *parse_xml(char *buf);
int parse_headers(char *buf, int *len);
int parse_uri(char *buf, int *br);
char *write_uri(char *buf, int buf_size, int br);
int parse_f4m(char *buf);
char *write_f4m(char *buf, int buf_size);

#endif