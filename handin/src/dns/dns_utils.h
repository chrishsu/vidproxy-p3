
#ifndef DNS_UTILS
#define DNS_UTILS

#include <stdio.h>
#include <stdlib.h>

// Query or Response
#define QR_QUERY      0   // 0000 0000
#define QR_RESPONSE   128 // 1000 0000
#define AA_QUERY      0   // 0000 0000
#define AA_RESPONSE   8   // 0000 1000
#define DNS_QUERY     QR_QUERY & AA_QUERY
#define DNS_RESPONSE  QR_RESPONSE & AA_RESONSE

// Response codes
#define R_OK      0 // No error
#define R_FORMAT  1 // Bad format
#define R_NAME    3 // Name not found

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
  short qtype;  // set to 1 for requests
  short qclass; // set to 1 for requests
} dns_question;

typedef struct {
  byte *aname;
  short atype;    // set to 1 for responses
  short aclass;   // set to 1 for responses
  int   ttl;      // set to 0
  short rdlength; // # of octets (4)
  int   rdata;    // ip address
} dns_answer;

int dns_process_header(char *b, dns_header *dh);
int dns_process_query(char *b, dns_question *dq);
int dns_process_answer(char *b, dns_question *dq);
char *dns_query_name(dns_question *dq);
int dns_is_valid(dns_header *dh, dns_question *dq);
dns_header *dns_create_header(int qr, byte rcode);
dns_question *dns_create_question();
dns_answer *dns_create_answer(int ip);
int dns_make_buf(dns_header *dh, dns_question *dq, dns_answer *da, char *buf);

#endif
