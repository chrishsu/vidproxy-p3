
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
int parse_uri(char *buf, int *br);
char *write_uri(char *buf, int buf_size, int br);

#endif
