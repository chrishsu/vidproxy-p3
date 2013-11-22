#include "parse_utils.h"

/**
 * Adds an item to the bitrate_list.
 *
 * @param[out] bl  The original bitrate list.
 * @param[in] br   The new bitrate to add.
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
 * Deletes the bitrate_list.
 *
 * @param[out] bl  The original bitrate list.
 */
void bitrate_list_free(bitrate_list *bl) {
  bitrate_list *cur;
  while (bl != NULL) {
    cur = bl;
    bl = bl->next;
    free(cur);
  }
}

/**
 * Gets the highest bitrate below the throughput.
 * Assumes that the bitrate list is in increasing order.
 *
 * @param[in] bl   The original bitrate list.
 * @param[in] tpt  The throughput.
 *
 * @return The selected bitrate or 0 if the list was empty.
 */
int bitrate_list_select(bitrate_list *bl, float tpt) {
  bitrate_list *prev = bl;
  while (bl != NULL) {
    if ( tpt < 1.5*(float)bl->bitrate) break;
    prev = bl;
    bl = bl->next;
  }
  if (prev != NULL) return prev->bitrate;
  else return 0;
}

/**
 * Gets a list of bitrates from an xml file.
 *
 * @param[in] buf       The buffer.
 * @param[in] buf_size  The buffer size.
 *
 * @return A list of bitrates or NULL.
 */
bitrate_list *parse_xml(char *buf, int buf_size) {
  #define LINE_LEN 256
  char *pt = buf;
  bitrate_list *list = NULL;
  bitrate_list *next = NULL;
  // Find media element.
  while((pt = strstr(pt, "<media")) != NULL) {
    char *spt;
    // Find bitrate attribute.
    if ((spt = strstr(pt, "bitrate=")) != NULL) {
      int bitrate;
      if (sscanf(spt, "bitrate=\"%d\"", &bitrate) < 1) {
        continue;
      }
      next = bitrate_list_add(next, bitrate);
      // Set list head.
      if (list == NULL) list = next;
    }
    pt = pt + 1; // Move pointer forward.
  }

  return list;
}

/**
 * Gets the content length from the buffer.
 *
 * @param[in] buf       The buffer.
 * @param[in] buf_size  The buffer size.
 * @param[out] len      The content length.
 *
 * @return 1 on success, 0 otherwise.
 */
int parse_headers(char *buf, int buf_size, int *len) {
  //if (buf_size < 19) return 0;
  char *pt = buf;
  if ((pt = strstr(buf, "Content-Length:")) != NULL) {
    if (sscanf(pt, "Content-Length: %d\n", len) < 1) {
      return 0;
    }
    return 1;
  }
  return 0;
}

/**
 * Gets the bitrate, sequence number, and fragment number from a uri.
 *
 * @param[in] buf       The buffer.
 * @param[in] buf_size  The buffer size.
 * @param[out] br       The bitrate.
 * @param[out] seq      The segment number.
 * @param[out] frag     The fragment number.
 *
 * @return 1 on success, 0 on failure.
 */
int parse_uri(char *buf, int buf_size,
              int *br, int *seq, int *frag) {
  if (buf_size < 20) return 0;
  if (sscanf(buf, "GET /vod/%dSeg%d-Frag%d", br, seq, frag) < 3) {
    return 0;
  }
  return 1;
}

/**
 * Replaces the uri with the new bitrate.
 *
 * @param[in/out] buf   The buffer.
 * @param[in] buf_size  The buffer size.
 * @param[in] br        The bitrate.
 *
 * @return 1 on success, 0 on failure.
 */
int replace_uri(char *buf, int *buf_size, int br) {
  //if (buf_size < 21) return 0;
  int bitrate, leftover, seg_num, frag_num;
  bitrate = 0; leftover = 0; seg_num = 0; frag_num = 0;
  if (sscanf(buf, "GET /vod/%dSeg%d-Frag%d %n",
	    &bitrate, &seg_num, &frag_num, &leftover) < 2) {
    return 0;
  }

  printf("Old size: %d\t", buf_size);
  *buf_size += floor(log10(br)) - floor(log10(bitrate));
  printf("New Size: %d\n", buf_size);
  char *newbuf = malloc(*buf_size);
  
  sprintf(newbuf, "GET /vod/%dSeg%d-Frag%d %s",
          br, seg_num, frag_num, buf+leftover);

  memcpy(buf, newbuf, *buf_size);
  free(newbuf);
  
  return 1;
}

/**
 * Checks if uri is for the f4m.
 *
 * @param[in] buf       The buffer.
 * @param[in] buf_size  The buffer size.
 *
 * @return 1 on success, 0 on failure.
 */
int parse_f4m(char *buf, int buf_size) {
  //if (buf_size < 27) return 0;
  if (sscanf(buf, "GET /vod/big_buck_bunny.f4m") < 0 ||
      strstr(buf, "GET /vod/big_buck_bunny.f4m") == NULL) {
    return 0;
  }
  return 1;
}

/**
 * Replaces uri with a new f4m request.
 *
 * @param[in/out] buf   The buffer.
 * @param[in] buf_size  The buffer size.
 *
 * @return 1 on success, 0 on failure.
 */
int replace_f4m(char *buf, int buf_size) {
  //if (buf_size < 28) return 0;
  char *newbuf = malloc(buf_size + 7);
  int leftover = 0;
  if (sscanf(buf, "GET /vod/big_buck_bunny.f4m %n",
      &leftover) < 0) {
    return 0;
  }

  sprintf(newbuf, "GET /vod/big_buck_bunny_nolist.f4m %s",
          buf+leftover);

  memcpy(buf, newbuf, buf_size + 7);
  free(newbuf);
  return 1;
}
