/*
 * TODO:
 * - clean up code
 * - add support for IPv6
 * - print messages in case of error
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "whois.h"

/* default to a public .com WHOIS server */
static char *srv_host = "whois.verisign-grs.com";

/* verbose logging flag */
static int verbose = 0;

/* query string */
static char *query = 0;

/* address family to use for communication (IPv4=AF_INET or IPv6=AF_INET6) */
static int addr_family = AF_INET;

/* print program help dialog */
static void print_help(void) {
    printf(
        "================================================================\n"
        "This sample program implements a WHOIS client for learning\n"
        "purposes, according to RFC 3912.\n\n"
        "The WHOIS program is called as (args in brackets optional):\n"
        "\twhois query [-s server] [-v]\n"
        "OR\n"
        "\twhois query -h\n\n"
        "with arguments:\n"
        "\tquery: The query to send to the WHOIS server\n"
        "\t-h: Print this help message and exit the program\n"
        "\t-s hostname: This option provides the IP address or hostname\n"
        "\t\tof the WHOIS server to query\n"
        "\t-v: Enable verbose logging mode\n"
        "================================================================\n");
}

/* parse and handle command line arguments */
static void parse_args(int argc, char **argv) {
    char *arg, *next;
    size_t arg_len;
    
    for(int i = 1; i < argc; ++i) {
        arg = argv[i];
        arg_len = strlen(arg);
        
        if(i + 1 < argc) {
            next = argv[i + 1];
        } else {
            next = 0;
        }
        
        /* consider all args preceded by a hyphen to be options */
        if(arg[0] == '-' && arg_len > 1) {
            switch(arg[1]) {
                
            /* print help text */
            case 'h':
                print_help();
                exit(EXIT_SUCCESS);
                break;
                
            /* set destination server */
            case 's':
                if(next) {
                    printf("set server : %s\n", next);
                    srv_host = next;
                    ++i;
                } else {
                    printf("error : a server name must follow option -s\n");
                }
                break;
                
            /* enable verbose logging */
            case 'v':
                printf("set verbose logging ON\n");
                verbose = 1;
                break;

	    /* enable IPv6 communication */
	    case '6':
		printf("enabled IPv6\n");
                addr_family = AF_INET6;
		break;
            
            /* invalid argument passed to program */
            default:
                printf(
                    "Error: unrecognized argument : %s\n"
                    "See help (-h) for list of allowed arguments.\n", 
                    arg);
                    
                exit(EXIT_FAILURE);
                break;
            }
        
        /* consider arguments not preceded by a hyphen to be a query */
        } else {
            query = arg;
        }
    }
}

int main(int argc, char **argv) {
    struct addrinfo ai_hints, *srv_addr_list, *ai_node;
    xp_socket sock;
    
    printf("\n");
    
    /* validate command line arguments */
    parse_args(argc, argv);
    
    if(!query) {
        printf("error: 'query' argument required (see option -h for help)\n");
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
    ai_hints.ai_family   = addr_family;
    ai_hints.ai_socktype = SOCK_STREAM;
    
    if(getaddrinfo(srv_host, WHOIS_SRV_LISTEN_PORT_STR, &ai_hints, &srv_addr_list)) {
        printf("getaddrinfo : failed to find destination addresses\n");
        exit(EXIT_FAILURE);
    }
    
    if(verbose) {
        printf("getaddrinfo : found destination addresses\n");
    }
    
    /* open a TPC socket */
    if((sock = socket(addr_family, SOCK_STREAM, 0)) == INVALID_SOCKET) {
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
    if(!whois_send(sock, query)) {
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
    
    printf("\n");
    return EXIT_SUCCESS;
}
