
#ifndef DNS_UTILS
#define DNS_UTILS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>

// Query or Response
#define QR_QUERY      0   // 0000 0000
#define QR_RESPONSE   128 // 1000 0000
#define AA_QUERY      0   // 0000 0000
#define AA_RESPONSE   4   // 0000 0100
#define DNS_QUERY     (QR_QUERY & AA_QUERY)
#define DNS_RESPONSE  (QR_RESPONSE & AA_RESPONSE)
#define DNS_BITS      252 // 1111 1100

// Response codes
#define R_OK      0  // No error
#define R_FORMAT  1  // Bad format
#define R_NAME    3  // Name not found
#define R_BITS    15 // 0000 1111

#define IS_QUERY     0
#define IS_RESPONSE  1
#define IS_ERROR     2

typedef char byte;

typedef struct {
  short id;
  byte  type;
  byte  rcode;
  short qdcount;
  short ancount;
  int   ignore; // nscount, arcount: set to 0
} dns_header;

typedef struct {
  byte *qname;
  int   namelen;
  short qtype;  // set to 1 for requests
  short qclass; // set to 1 for requests
} dns_question;

typedef struct {
  byte *aname;
  int   namelen;
  short atype;    // set to 1 for responses
  short aclass;   // set to 1 for responses
  int   ttl;      // set to 0
  short rdlength; // # of octets (4)
  int   rdata;    // ip address
} dns_answer;

int dns_process_header(char *b, dns_header *dh);
int dns_process_query(char *b, short len, dns_question *dq);
int dns_process_answer(char *b, short len, dns_answer *da);
char *dns_query_name(dns_question *dq);
int dns_is_valid(dns_header *dh, dns_question *dq);
void dns_edit_header(dns_header *dh, short qr, byte rcode);
dns_header *dns_create_header(short qr, byte rcode);
void dns_edit_question(dns_question *dq);
dns_question *dns_create_question();
dns_answer *dns_create_answer(int ip);
char *dns_make_buf(dns_header *dh, dns_question *dq, dns_answer *da, int *buflen);

#endif
