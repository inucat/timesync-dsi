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

#include "ntp.h"

#include <netinet/in.h>
#include <sys/socket.h>

int
ntp_send_request(int sockfd, const struct sockaddr* addr)
{
    struct ntp_packet packet;
    memset(&packet, 0, sizeof(packet));
    packet.li_vn_mode = VN_NTP_V3 | MODE_CLIENT;

    return sendto(
      sockfd, (char*)&packet, sizeof(packet), 0, addr, sizeof(struct sockaddr));
}

int
ntp_receive_response(int sockfd,
                     struct sockaddr* addr,
                     struct ntp_packet* packet)
{
    memset(packet, 0, sizeof(struct ntp_packet));
    socklen_t addr_len = sizeof(struct sockaddr);

    return recvfrom(
      sockfd, packet, sizeof(struct ntp_packet), 0, addr, &addr_len);
}

time_t
ntp_time_to_unix_time(u64 ntp_timestamp, s64 offset_seconds)
{
    return (time_t)(ntohl(ntp_timestamp) - NTP_UNIX_DELTA + offset_seconds);
}
