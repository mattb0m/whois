#include "xp_sock.h"

/* cross-platform socket library init routine */
int sock_init() {
#ifdef __WIN32
    WSADATA wd;
    
    if(WSAStartup(MAKEWORD(2,2), &wd)) {
        return 0;
    }
#endif

    return 1;
}

/* cross-platform socket library cleanup routine */
int sock_cleanup() {
#ifdef __WIN32
    return(WSACleanup() == 0);
#endif
    return 1;
}

/* cross-platform function to close a socket */
int sock_close(xp_socket s) {
#ifdef __WIN32
    return closesocket(s);
#else
    return close(s);
#endif
}