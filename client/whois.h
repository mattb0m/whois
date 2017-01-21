#ifndef WHOIS_H
#define WHOIS_H

/* ----- WINDOWS ----- */
#ifdef __WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>

    /* comment to windows compilers that ws2_32.lib must be linked to */
    #pragma comment(lib, "Ws2_32.lib")
    
    /* cross-platform socket type */
    typedef SOCKET xp_socket;

/* ----- UNIX ----- */
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <netdb.h>      /* getaddrinfo() */
    #include <unistd.h>     /* close() */
    
    /* define winsock-compatible return constants */
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
    
    /* cross-platform socket type */
    typedef int xp_socket;
#endif

/* WHOIS protocol definitions */
#define WHOIS_SRV_LISTEN_PORT 43U
#define WHOIS_SRV_LISTEN_PORT_STR "43"
#define WHOIS_RDBUF_LEN 1024

/* cross-platform socket library init routine */
int sock_init();

/* cross-platform socket library cleanup routine */
int sock_cleanup();

/* cross-platform function to close a socket */
int sock_close(xp_socket s);

/* send WHOIS request on connected socket */
int whois_send(xp_socket sockfd, char *query);

/* receive response message from WHOIS request */
int whois_recv(xp_socket sockfd);

#endif
