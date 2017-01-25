/* 
 * This program implements the WHOIS Internet protocol, as defined in RFC-3912:
 * https://tools.ietf.org/html/rfc3912
 *
 * This program follows the following porting guidelines for compatibility with Windows:
 * https://msdn.microsoft.com/en-us/library/windows/desktop/ms740096(v=vs.85).aspx
 *
 * Compile on windows with gcc with: -lws2_32
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "whois.h"

/* send WHOIS request on connected socket */
int whois_send(xp_socket sockfd, char *query) {
    int rslt, flags = 0;
    size_t len, msg_len, sent = 0;
    char *msg;
    
    /* allocate message buffer */
    len = strlen(query); 
    msg_len = len + 2;
    msg = malloc(msg_len);
    
    /* format WHOIS message */
    memcpy(msg, query, len);
    msg[msg_len - 2] = '\r';
    msg[msg_len - 1] = '\n';
    
    /* send message (in principle, a partial send could occur) */
    do {
        rslt = send(sockfd, msg + sent, msg_len - sent, flags);
        sent += rslt;
    } while(rslt != SOCKET_ERROR && sent < msg_len);
    
    return(rslt != SOCKET_ERROR);
}

/* receive response message from WHOIS request */
int whois_recv(xp_socket sockfd) {
    int rslt, flags = 0;
    char rdbuf[WHOIS_RDBUF_LEN];

    /* read data from socket until disconnect (or error) */
    while((rslt = recv(sockfd, rdbuf, sizeof(rdbuf), flags)) > 0 && rslt != SOCKET_ERROR) {
        /* TODO : handle read data (buffer it internally, or just print it out immediately)*/
        fwrite(rdbuf, 1, rslt, stdout);
    }
    
    return(rslt != SOCKET_ERROR);
}
