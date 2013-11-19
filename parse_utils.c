#include "parse_utils.h"

/**
 * Adds an item to the bitrate_list.
 *
 * @param[in/out] bl  The original bitrate list.
 * @param[in] br      The new bitrate to add.
 *
 * @return Pointer to new bitrate list.
 */
bitrate_list *bitrate_list_add(bitrate_list *bl, int br) {
  bitrate_list *nbl = malloc(sizeof(bitrate_list));
  nbl->bitrate = br;
  nbl->next = NULL;
  if (bl == NULL) return nbl;
  bl->next = nbl;
  return nbl;
}

/**
 * Gets a list of bitrates from an xml file.
 *
 * @param[in] buf  The buffer.
 *
 * @return A list of bitrates or NULL.
 */
bitrate_list *parse_xml(char *buf) {
  #define LINE_LEN 256
  char line[LINE_LEN];
  char *pt = buf;
  int total = 0;
  bitrate_list *list = NULL;
  bitrate_list *next = NULL;
  // Find media element.
  while((pt = strstr(pt, "<media")) != NULL) {
    char *spt;
    // Find bitrate attribute.
    if ((spt = strstr(pt, "bitrate=")) != NULL) {
      int bitrate;
      if (sscanf(spt, "bitrate=\"%d\"%*n", &bitrate) < 1) {
        continue;
      }
      next = bitrate_list_add(next, bitrate);
      // Set list head.
      if (list == NULL) list = next;
    }
  }
  
  return list;
}

/**
 * Gets the bitrate from a uri.
 *
 * @param[in] buf  The buffer.
 * @param[out] br  The bitrate.
 * 
 * @return 1 on success, 0 on failure.
 */
int parse_uri(char *buf, int *br) {
  if (sscanf(buf, "GET /vod/%d%Seg%*d-Frag%*d %*n", br) < 1) {
    return 0;
  }
  return 1;
}

/**
 * Writes a new uri and request.
 *
 * @param[in] buf       The buffer.
 * @param[in] buf_size  The buffer size.
 * @param[in] br        The bitrate.
 *
 * @return The new uri string.
 */
char *write_uri(char *buf, int buf_size, int br) {
  char *newbuf = malloc(buf_size + (br/10) + 1);
  int leftover, seg_num, frag_num;
  if (sscanf(buf, "GET /vod/%*d%Seg%d-Frag%d %n", seg_num, frag_num, leftover) < 3) {
    return NULL;
  }
  
  sprintf(newbuf, "GET /vod/%dSeg%d-Frag%d %s", br, seg_num, frag_num, buf+leftover);
  
  return newbuf;
}