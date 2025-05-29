/*---------------------------------------------------------------------------------

---------------------------------------------------------------------------------*/
#include "ntp.h"
// #include <cerrno>
#include <dswifi9.h>
#include <errno.h>
#include <nds.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdint.h>
#include <sys/socket.h>

#include <stdio.h>

#define NTP_SERVER "pool.ntp.org"

static void
getNtp(char* url);

//---------------------------------------------------------------------------------
int
main(void)
{
    //---------------------------------------------------------------------------------

    consoleDemoInit(); // setup the sub screen for printing

    iprintf("\n\n\tSimple NTP Connection Demo\n\n");
    iprintf("Connecting via WFC data ...\n");

    if (!Wifi_InitDefault(WFC_CONNECT)) {
        iprintf("Failed to connect!");
    } else {

        iprintf("Connected\n\n");

        getNtp(NTP_SERVER);
    }

    while (pmMainLoop()) {
        swiWaitForVBlank();
        int keys = keysDown();
        if (keys & KEY_START)
            break;
    }

    return 0;
}

//---------------------------------------------------------------------------------
static void
getNtp(char* url)
{
    struct ntp_packet packet = { VN_NTP_V4 | MODE_CLIENT };
    // printf("%02x\n", packet.li_vn_mode);
    // printf("packet\n");
    const struct hostent* host = gethostbyname(url);

    // Create a UDP socket
    // int ntp_socket = socket(AF_INET, SOCK_STREAM, 0);
    const int ntp_socket = socket(AF_INET, SOCK_DGRAM, 0);
    const struct timeval timeout = { 5, 0 };
    setsockopt(ntp_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    printf("Created Socket!\n");

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(NTP_PORT);
    addr.sin_addr.s_addr = *((unsigned long*)(host->h_addr_list[0]));

    // socklen_t addr_len = sizeof(addr);
    printf("Sending to server!\n");
    const int send_err = sendto(ntp_socket,
                                &packet,
                                sizeof(packet),
                                0,
                                (struct sockaddr*)&addr,
                                sizeof(addr));
    printf("Sent %d:%d\n", send_err, send_err >= 0 ? 0 : errno);

    socklen_t alen;
    const int recv_err = recvfrom(
      ntp_socket, &packet, sizeof(packet), 0, (struct sockaddr*)&addr, &alen);
    printf("Received %d:%d\n", recv_err, recv_err >= 0 ? 0 : errno);

    uint32_t tx_seconds = ntohl(packet.tx_tm.seconds);
    // printf("%02x %d\n", packet.li_vn_mode, packet.stratum);
    uint32_t unix_seconds = tx_seconds - NTP_JAN_1970;
    printf("unix_seconds=%lu\n", unix_seconds);
    shutdown(ntp_socket, 0);
    closesocket(ntp_socket);
}
