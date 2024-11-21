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

#ifndef _NTP_H_
#define _NTP_H_

#include <nds.h>

#define NTP_PORT            123
#define NTP_TIMESTAMP_DELTA 2208988800ull // 1900 to 1970 in seconds

struct ntp_packet {
    uint8_t li_vn_mode;
    uint8_t stratum;
    uint8_t poll;
    uint8_t precision;
    uint32_t root_delay;
    uint32_t root_dispersion;
    uint32_t ref_id;
    uint32_t ref_tm_s;
    uint32_t ref_tm_f;
    uint32_t orig_tm_s;
    uint32_t orig_tm_f;
    uint32_t rx_tm_s;
    uint32_t rx_tm_f;
    uint32_t tx_tm_s;
    uint32_t tx_tm_f;
};

#define LI_NO_WARNING  0b00000000
#define LI_LAST_MIN_61 0b01000000
#define LI_LAST_MIN_59 0b10000000
#define LI_UNKNOWN     0b11000000

#define VN_NTP_V3 0b00011000
#define VN_NTP_V4 0b00100000

#define MODE_RESERVED  0b00000000
#define MODE_SYM_ACTI  0b00000001
#define MODE_SYM_PASS  0b00000010
#define MODE_CLIENT    0b00000011
#define MODE_SERVER    0b00000100
#define MODE_BROADCAST 0b00000101
#define MODE_NTP_CTRL  0b00000110
#define MODE_PRIVATE   0b00000111

/**
 * @brief Sends an NTP packet via `sockfd` to the `addr`.
 *
 * @param sockfd File descriptor of the socket.
 * @param addr Address data of the NTP server.
 * @return Succeeded: >= 0; Failed: < 0
 */
extern int ntp_request_sync(int sockfd, struct sockaddr *addr);

/**
 * @brief Receives an NTP packet via `sockfd` from the `addr`.
 *
 * @param sockfd File descriptor of the socket.
 * @param addr Address data of the NTP server.
 * @param packet Pointer to store received NTP packet.
 * @return Succeeded: >= 0; Failed: < 0
 */
extern int ntp_recv_packet(int sockfd, struct sockaddr *addr,
                           struct ntp_packet *packet);

#endif // _NTP_H_
