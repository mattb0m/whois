#ifndef XP_SOCK_H
#define XP_SOCK_H

/* ----- WINDOWS ----- */
#ifdef __WIN32

    /* re-define windows version, if version is less than Win 2000 (to support getaddrinfo()) */
    #if (_WIN32_WINNT <= 0x0500)
    #define WINVER 0x0601  
    #define _WIN32_WINNT 0x0601
    #endif
    
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

/* cross-platform socket library init routine */
int sock_init();

/* cross-platform socket library cleanup routine */
int sock_cleanup();

/* cross-platform function to close a socket */
int sock_close(xp_socket s);

#endif