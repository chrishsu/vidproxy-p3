
#ifndef VP_PARSER
#define VP_PARSER

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

typedef struct bitrate_list {
  int bitrate;
  struct bitrate_list *next;
} bitrate_list;

void bitrate_list_free(bitrate_list *bl);
int bitrate_list_select(bitrate_list *bl, float tpt);
bitrate_list *parse_xml(char *buf, int buf_size);
int parse_headers(char *buf, int buf_size, int *len);
int parse_uri(char *buf, int buf_size, int *br, int *seq, int *frag);
int replace_uri(char *buf, int *buf_size, int br);
int parse_f4m(char *buf, int buf_size);
int replace_f4m(char *buf, int buf_size);

#endif
