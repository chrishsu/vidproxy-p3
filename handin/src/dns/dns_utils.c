#include "dns_utils.h"

/**
 * Takes a byte array and makes it into a header.
 */
int dns_process_header(char *b, dns_header *dh) {
  memcpy(dh, b, sizeof(dns_header));
  return 1;
}

/**
 * Calculates the length of the name.
 */
int name_length(char *b, short len) {
  int i;
  for (i = 0; i < len; i++) {
    if (b[i] == 0) break;
  }
  return i+1;
}

/**
 * Takes a byte array and makes it into a question.
 */
int dns_process_query(char *b, short len, dns_question *dq) {
  int namelen = name_length(b, len);
  dq->namelen = namelen;
  dq->qname = malloc(namelen);
  memcpy(dq->qname, b, namelen);
  if (len == 0) dq->qname = NULL;
  int variance = sizeof(byte *) + sizeof(int);

  memcpy((char *)dq + variance, b + namelen,
	 sizeof(dns_question) - variance);

  //printf("b: %hhd %hhd %hhd %hhd\n", b[0], b[1], b[2], b[3]);
  printf("Set type to %hd and class to %hd\n", dq->qtype, dq->qclass);
  return 1;
}

/**
 * Takes a byte array and makes it into an answer.
 */
int dns_process_answer(char *b, short len, dns_answer *da) {
  int namelen = name_length(b, len);
  da->namelen = namelen;
  da->aname = malloc(namelen);
  memcpy(da->aname, b + namelen, namelen);
  int variance = sizeof(byte *) + sizeof(int);
  int delta = sizeof(dns_answer) - variance
              - sizeof(short) - sizeof(int);
  memcpy((char *)da + variance, b + namelen, delta);
  mempcy(da->rdata, b + namelen + delta, sizeof(int));
  
  return 1;
}

/**
 * Returns the name from a question.
 */
char *dns_query_name(dns_question *dq) {
  if (dq == NULL) return NULL;
  char *name = malloc(dq->namelen - 1);
  memset(name, 0, dq->namelen - 1);

  int i, sec_len, len_count; int is_len = 1;
  for (i = 0; i < dq->namelen - 1; i++) {
    if (is_len) {
      sec_len = dq->qname[i];
      is_len = 0;
      len_count = 0;
      if (i != 0) name[i-1] = '.';
    } else {
      name[i-1] = dq->qname[i];
      len_count++;
    }

    if (sec_len == len_count) {
      is_len = 1;
    }
  }

  return name;
}

/**
 * Checks if a header and question are valid.
 */
int dns_is_valid(dns_header *dh, dns_question *dq) {
  if (dh == NULL || dq == NULL) return 0;
  if ((dh->type&DNS_BITS) != DNS_QUERY) {
    printf("DNS: not query (%d)\n", (int)dh->type);
    //return 0;
  }
  if ((dh->rcode&R_BITS) != R_OK) {
    printf("DNS: not OK (%d)\n", (int)dh->rcode);
    //return 0;
  }
  if (ntohs(dh->qdcount) != 1 && ntohs(dh->ancount) != 0) {
    printf("DNS: not only 1 question and no answer\n");
    //return 0;
  }
  return 1;
}

/**
 * Edits the given DNS header.
 */
void dns_edit_header(dns_header *dh, short qr, byte rcode) {
  if (dh == NULL) return;

  dh->rcode = rcode;

  if (qr == IS_QUERY) {
    dh->type = DNS_QUERY;
    dh->qdcount = ntohs(1);
  }
  else if (qr == IS_RESPONSE) {
    dh->type = DNS_RESPONSE;
    dh->qdcount = ntohs(1);
    dh->ancount = ntohs(1);
  }
  else if (qr == IS_ERROR) {
    dh->type = DNS_RESPONSE;
    dh->qdcount = ntohs(1);
  }
  dh->ignore = 0;
}

/**
 * Creates a DNS header.
 */
dns_header *dns_create_header(short qr, byte rcode) {
  dns_header *dh = malloc(sizeof(dns_header));

  dh->id = htons(15441);
  dh->type = 0;
  dh->qdcount = 0;
  dh->ancount = 0;
  dh->ignore = 0;

  dns_edit_header(dh, qr, rcode);

  return dh;
}

void dns_edit_question(dns_question *dq) {
  if (dq == NULL) return;
  dq->qtype = htons(1);
  dq->qclass = htons(1);
}

#define NAME "5video2cs3cmu3edu0"
#define NAME_LEN 18

/**
 * Creates a name.
 */
byte *create_name() {
  byte *b = malloc(NAME_LEN);
  memcpy(b, NAME, NAME_LEN);
  return b;
}

/**
 * Creates a DNS question.
 */
dns_question *dns_create_question() {
  dns_question *dq = malloc(sizeof(dns_question));

  dq->qname = create_name();
  dq->namelen = NAME_LEN;
  dq->qtype = htons(1);
  dq->qclass = htons(1);

  return dq;
}

/**
 * Creates a DNS answer.
 */
dns_answer *dns_create_answer(int ip) {
  dns_answer *da = malloc(sizeof(dns_answer));

  da->aname = create_name();
  da->namelen = NAME_LEN;
  da->atype = htons(1);
  da->aclass = htons(1);
  da->ttl = 0;
  da->rdlength = htons(4);
  da->rdata = htonl(ip);

  return da;
}

/**
 * Creates a buffer from various DNS header, question, or answer.
 * Also keeps track of the total buffer length.
 *
 * @param[out] buflen  The resulting length of the buffer.
 *
 * @return The buffer.
 */
char *dns_make_buf(dns_header *dh, dns_question *dq, dns_answer *da, int *buflen) {
  if (dh == NULL) return 0;

  char *buf;
  *buflen = sizeof(dns_header);
  int variance = sizeof(byte *) + sizeof(int);

  if (dq != NULL) {
    *buflen += (sizeof(dns_question) - variance) + dq->namelen;
  }
  if (da != NULL) {
    *buflen += (sizeof(dns_answer) - variance) + da->namelen;
  }

  buf = malloc(*buflen);
  memcpy(buf, dh, sizeof(dns_header));
  int delta = sizeof(dns_header);

  if (dq != NULL) {
    memcpy(buf + delta, dq->qname, dq->namelen);
    delta += dq->namelen;
    memcpy(buf + delta, (char *)dq + variance, sizeof(dns_question) - variance);
    delta += sizeof(dns_question) - variance;
  }
  if (da != NULL) {
    memcpy(buf + delta, da->aname, da->namelen);
    delta += da->namelen;
    int offset = sizeof(dns_answer) - variance
                 - sizeof(short) - sizeof(int);
    memcpy(buf + delta, (char *)da + variance, offset);
    delta += offset;
    memcpy(buf + delta, da->rdata, sizeof(int));
  }

  return buf;
}
