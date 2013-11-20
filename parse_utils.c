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
 * Gets a list of bitrates from an xml file.
 *
 * @param[in] buf       The buffer.
 * @param[in] buf_size  The buffer size.
 *
 * @return A list of bitrates or NULL.
 */
bitrate_list *parse_xml(char *buf, int buf_size) {
  #define LINE_LEN 256
  int len = buf_size;
  char *pt = buf;
  bitrate_list *list = NULL;
  bitrate_list *next = NULL;
  // Find media element.
  while((pt = strnstr(pt, "<media", len)) != NULL) {
    char *spt;
    // Find bitrate attribute.
    if ((spt = strnstr(pt, "bitrate=", len)) != NULL) {
      int bitrate;
      if (sscanf(spt, "bitrate=\"%d\"", &bitrate) < 1) {
        continue;
      }
      next = bitrate_list_add(next, bitrate);
      // Set list head.
      if (list == NULL) list = next;
    }
    pt = pt + 1; // Move pointer forward.
    len = (int)(pt - buf);
  }

  return list;
}

/**
 * Gets the content length from the buffer.
 *
 * @param[in] buf       The buffer.
 * @param[out] len      The content length.
 * @param[in] buf_size  The buffer size.
 *
 * @return 1 on success, 0 otherwise.
 */
int parse_headers(char *buf, int *len, int buf_size) {
  char *pt = buf;
  if ((pt = strnstr(buf, "Content-Length:", buf_size)) != NULL) {
    if (sscanf(pt, "Content-Length: %d\n", len) < 1) {
      return 0;
    }
    return 1;
  }
  return 0;
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
  if (sscanf(buf, "GET /vod/%dSeq%*d-Frag%*d", br) < 1) {
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
int replace_uri(char *buf, int buf_size, int br) {
  char *newbuf = malloc(buf_size + (br/10) + 1);
  int leftover, seg_num, frag_num;
  leftover = 0; seg_num = 0; frag_num = 0;
  if (sscanf(buf, "GET /vod/%*dSeq%d-Frag%d %n",
	    &seg_num, &frag_num, &leftover) < 2) {
    return 0;
  }

  sprintf(newbuf, "GET /vod/%dSeq%d-Frag%d %s",
          br, seg_num, frag_num, buf+leftover);

  memcpy(buf, newbuf, buf_size + (br/10) + 1);
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
  if (sscanf(buf, "GET /vod/big_buck_bunny.f4m") < 0 ||
      strnstr(buf, "GET /vod/big_buck_bunny.f4m", buf_size) == NULL) {
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
