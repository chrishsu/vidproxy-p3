#include "dns_utils.h"

int dns_process_header(char *b, dns_header *dh) {
  memcpy(dh, b, sizeof(dns_header));
  return 1;
}

int name_length(char *b, short len) {
  int i; 
  for (i = 0; i < len; i++) {
    if (b[i] == 0) break;
  }
  return i+1;
}

int dns_process_query(char *b, short len, dns_question *dq) {
  int len = name_length(b, len);
  dq->namelen = len;
  dq->qname = malloc(len);
  memcpy(dq->qname, b, len);
  if (len == 0) dq->qname = NULL;
  int variance = sizeof(byte *) + sizeof(int);
  memcpy(dq + variance, b,
          sizeof(dns_question) - variance);
  return 1;
}

int dns_process_answer(char *b, dns_answer *da) {
  int len = name_length(b, len);
  da->namelen = len;
  da->aname = malloc(len);
  memcpy(da->aname, b, len);
  int variance = sizeof(byte *) + sizeof(int);
  memcpy(da + variance, b,
          sizeof(dns_answer) - variance);
  return 1;
}

char *dns_query_name(dns_question *dq) {
  if (dq == NULL) return NULL;
  char *name = malloc(dq->namelen - 1);
  name[0] = 0;
  
  int i, sec_len, len_count; int is_len = 1;
  for (i = 0; i <dq->namelen - 1; i++) {
    if (is_len) {
      sec_len = dq->qname[i];
      is_len = 0;
      len_count = 0;
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

int dns_is_valid(dns_header *dh, dns_question *dq) {
  return 1;
}

dns_header *dns_create_header(int qr, byte rcode) {
  dns_header *dh = malloc(sizeof(dns_header));
  
  dh->id = 0;
  dh->type = 0;
  dh->rcode = rcode;
  dh->qdcount = 0;
  dh->ancount = 0;
  dh->ignore = 0;
  if (qr == IS_QUERY) {
    dh->type = DNS_QUERY;
    dh->qdcount = ntohs(1);
  }
  else if (qr == IS_RESPONSE) {
    dh->type = DNS_RESPONSE;
    dh->ancount = ntohs(1);
  }
  
  return dh;
}

#define NAME "5video2cs3cmu3edu0"
#define NAME_LEN 18

byte *create_name() {
  byte *b = malloc(NAME_LEN);
  memcpy(b, NAME, NAME_LEN);
}

dns_question *dns_create_question() {
  dns_question *dq = malloc(sizeof(dns_question));
  
  dq->qname = create_name();
  dq->namelen = NAME_LEN;
  dq->qtype = htons(1);
  dq->qclass = htons(1);
  
  return dq;
}

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

char *dns_make_buf(dns_header *dh, dns_question *dq, dns_answer *da, int *buflen) {
  if (dh == NULL) return 0;
  
  char *buf;
  buflen = sizeof(dns_header);
  int variance = sizeof(byte *) + sizeof(int);
  
  if (dq != NULL) {
    buflen += (sizeof(dns_question) - variance) + dq->namelen;
  }
  else if (da != NULL) {
    buflen += (sizeof(dns_answer) - variance) + da->namelen;
  }
  
  buf = malloc(buflen);
  memcpy(buf, dh, sizeof(dh));
  int delta = sizeof(dh);
  
  if (dq != NULL) {
    mempcy(buf + delta, dq->qname, dq->namelen);
    delta += dq->namelen;
    mempcy(buf + delta, dq + variance, sizeof(dns_question) - variance);
  }
  else if (da != NULL) {
    mempcy(buf + delta, da->aname, da->namelen);
    delta += da->namelen;
    mempcy(buf + delta, da + variance, sizeof(dns_answer) - variance);
  }
  
  return buf;
}