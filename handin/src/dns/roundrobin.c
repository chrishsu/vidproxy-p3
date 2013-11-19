
#include "roundrobin.h"

/**
 * Parse the file for server ip's.
 *
 * @param[in] filename  The name of the server file.
 */
void rr_parse_servers(char *filename) {
  servers = NULL;
  FILE *fp = fopen(filename, "r");

  server_loop *prev = NULL;
  server_loop *cur = NULL;
  char c; int idx = 0;

  while ((c = fgetc(fp)) != EOF) {
    if (cur == NULL) {
      cur = malloc(sizeof(server_loop));
      cur->ip[0] = 0;
      cur->next = NULL;
      // Connect the elements
      if (prev != NULL) {
        prev->next = cur;
      }
      // Start of loop
      if (servers == NULL) {
        servers = cur;
      }
    }
    if (c == '\n') {
      cur->ip[idx] = 0;
      idx = 0;
      prev = cur;
      cur = NULL;
      continue;
    }
    cur->ip[idx] = c;
    idx++;
  }

  // Loop last element to start
  if (servers != NULL && prev != NULL) {
    prev->next = servers;
  }

  fclose(fp);
}

/**
 * Return the next server on the loop.
 *
 * @return The next server.
 */
char *rr_next_server() {
  server_loop *tmp = servers;
  servers = servers->next;
  return tmp->ip;
}

/**
 * Free the server loop.
 */
void rr_free() {
  server_loop *tmp = servers->next;
  server_loop *prev = servers;
  int i = 0;
  while (tmp != NULL) {
    if (prev != servers) {
      prev->next = NULL;
      free(prev);
    }
    if (tmp == servers) {
      free(tmp);
      break;
    }
    prev = NULL;
    prev = tmp;
    tmp = tmp->next;
    i++;
  }
}
