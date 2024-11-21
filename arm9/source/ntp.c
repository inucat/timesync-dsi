/**
 * @copyright GNU Public License.
 *
 * Timesync DSi - Synchronize your DSi clock with NTP
 * Copyright (C) 2024  inucat
 *
 * This file is part of Timesync DSi.
 *
 * Timesync DSi is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Timesync DSi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Timesync DSi.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <netinet/in.h>

#include "ntp.h"

int ntp_request_sync(int sockfd, struct sockaddr *addr) {
    struct ntp_packet packet;
    memset(&packet, 0, sizeof(packet));
    packet.li_vn_mode = VN_NTP_V3 | MODE_CLIENT;

    return sendto(sockfd, (char *)&packet, sizeof(packet), 0, addr,
                  sizeof(struct sockaddr));
}

int ntp_recv_packet(int sockfd, struct sockaddr *addr,
                    struct ntp_packet *packet) {
    memset(packet, 0, sizeof(struct ntp_packet));
    int addr_len = sizeof(struct sockaddr);

    return recvfrom(sockfd, packet, sizeof(struct ntp_packet), 0, addr,
                    &addr_len);
}
