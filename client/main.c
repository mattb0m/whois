/*
 * TODO:
 * - clean up code
 * - add support for IPv6
 * - print messages in case of error
 * - add "-h" (HELP) command line option
 * - add "-s" (SERVER) command line option
 * - add "-v" (VERBOSE) logging option
 * - test on Windows
 * - separate x-platform sockets code from WHOIS code
 *
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "whois.h"

/* default to a public .com WHOIS server */
static const char *whois_default_host = "whois.verisign-grs.com";

int main(int argc, char **argv) {
    const char *srv_host = whois_default_host;
    struct addrinfo ai_hints, *srv_addr_list, *ai_node;
    xp_socket sock;
    
    /* temporary until a command line option is set */
    int verbose = 0;
    
    /* validate command line arguments */
    if(argc < 2) {
        printf("error: %s requires at least one argument (destination hostname)\n", argv[0]);
        return(EXIT_FAILURE);
    }
        
    /* init socket library */
    if(!sock_init()) {
        printf("socket lib init failed\n");
        return(EXIT_FAILURE);
    }
    
    if(verbose) {
        printf("socket lib init successful\n");
    }
    
    /* DNS lookup to get destination address info */
    memset(&ai_hints, 0, sizeof(ai_hints));
    ai_hints.ai_family   = AF_INET;
    ai_hints.ai_socktype = SOCK_STREAM;
    
    if(getaddrinfo(srv_host, WHOIS_SRV_LISTEN_PORT_STR, &ai_hints, &srv_addr_list)) {
        printf("getaddrinfo : failed to find destination addresses\n");
        exit(EXIT_FAILURE);
    }
    
    if(verbose) {
        printf("getaddrinfo : found destination addresses\n");
    }
    
    /* open an IPv4 TPC socket (TODO: add option to open an IPv6 socket) */
    if((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("failed to create new socket\n");
        exit(EXIT_FAILURE);
    }
    
    if(verbose) {
        printf("created socket\n");
    }
    
    ai_node = srv_addr_list;
    
    /* attempt to connect to each address until one succeeds */
    while(ai_node) {
        
        /* attempt to connect socket to the dest addr */
        if(connect(sock, ai_node->ai_addr, ai_node->ai_addrlen) != SOCKET_ERROR) {
            break;
        }
        
        ai_node = ai_node->ai_next;
    }
    
    freeaddrinfo(srv_addr_list);
    
    if(!ai_node) {
        printf("error : could not establish connection to destination server\n");
        exit(EXIT_FAILURE);
    }
    
    if(verbose) {
        printf("connection establised\n");
    }
    
    /* send WHOIS request to server */
    if(!whois_send(sock, argv[1])) {
        printf("failed to send request to server\n");
        exit(EXIT_FAILURE);
    }
    
    if(verbose) {
        printf("request sent to server\n");
    }
    
    /* receive WHOIS response from server */
    if(!whois_recv(sock)) {
        printf("failed to rcv response from server\n");
        exit(EXIT_FAILURE);
    }
    
    if(verbose) {
        printf("received response from server\n");
    }
    
    /* close socket */
    if(sock_close(sock) == SOCKET_ERROR) {
        printf("failed to close connection\n");
    }
    
    if(verbose) {
        printf("connection closed\n");
    }
    
    /* cleanup socket library */
    if(!sock_cleanup()) {
        printf("socket lib cleanup failed\n");
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}
