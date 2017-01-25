#ifndef WHOIS_H
#define WHOIS_H

/* this header relies on the XP_SOCK lib header */
#include "../lib/xp_sock.h"

/* WHOIS protocol definitions */
#define WHOIS_SRV_LISTEN_PORT 43U
#define WHOIS_SRV_LISTEN_PORT_STR "43"
#define WHOIS_RDBUF_LEN 1024

/* send WHOIS request on connected socket */
int whois_send(xp_socket sockfd, char *query);

/* receive response message from WHOIS request */
int whois_recv(xp_socket sockfd);

#endif
