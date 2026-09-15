#ifndef NETWORK_H
#define NETWORK_H

#include <netinet/in.h>

/**
 * @brief Initializes a network endpoint for communication.
 * @param hostname The hostname of the server to connect to.
 * @param port The port number to connect to.
 * @param socket_fd Pointer to an integer where the socket file descriptor will
 * be stored.
 * @param addr Pointer to a sockaddr_in structure where the resolved address
 * will be stored.
 * @return 0 on success, or a negative error code on failure.
 */
int
init_endpoint(const char* hostname,
              int port,
              int* socket_fd,
              struct sockaddr_in* addr);

#endif // NETWORK_H
