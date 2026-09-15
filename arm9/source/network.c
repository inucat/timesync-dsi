#include "network.h"

#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

int
init_endpoint(const char* hostname,
              int port,
              int* socket_fd,
              struct sockaddr_in* addr)
{
    if (!hostname || !socket_fd || !addr) {
        return -1;
    }

    *socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (*socket_fd < 0) {
        return -1;
    }

    struct hostent* host = gethostbyname(hostname);
    if (!host) {
        return -1;
    }

    memset(addr, 0, sizeof(struct sockaddr_in));
    addr->sin_family = AF_INET;
    addr->sin_port = htons(port);
    memcpy(&addr->sin_addr.s_addr, host->h_addr_list[0], host->h_length);

    return 0;
}
