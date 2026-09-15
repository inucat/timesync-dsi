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

#include <nds.h>

#include <dswifi9.h>
#include <nds/interrupts.h>
#include <nds/ndstypes.h>
#include <nds/system.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "network.h"
#include "ntp.h"
#include "rtc.h"
#include "time_conversion.h"

#define NTP_SERVER     "ntp.nict.jp"
#define NTP_PORT       123
#define OFFSET_SECONDS 9 * 60 * 60 // JST offset in seconds

void
debug_conversion(u32* rtc_time)
{
    iprintf(
      "Converted Date/Time: %04ld-%02ld-%02ld (%01ld) %02ld:%02ld:%02ld\n",
      rtc_time[0],
      rtc_time[1],
      rtc_time[2],
      rtc_time[3],
      rtc_time[4],
      rtc_time[5],
      rtc_time[6]);
}

int
main(void)
{
    u32 rtc_time[7] = { 0 };

    consoleDemoInit();
    iprintf("= Timesync DSi =\n");

    iprintf("Waking up Wifi.\n");
    if (!Wifi_InitDefault(true)) {
        iprintf("Wifi_InitDefault: Failed\n");
        goto main_loop;
    }

    int sockfd;
    struct sockaddr_in host_addr;
    iprintf("Resolving NTP server address...\n");
    if (init_endpoint(NTP_SERVER, NTP_PORT, &sockfd, &host_addr) < 0) {
        iprintf("init_endpoint: Failed\n");
        goto main_loop;
    }

    iprintf("Sending NTP request to %s...\n", NTP_SERVER);
    if (ntp_send_request(sockfd, (struct sockaddr*)&host_addr) < 0) {
        iprintf("ntp_request_sync: Failed\n");
        goto main_loop;
    }

    iprintf("Waiting for NTP response...\n");
    struct ntp_packet packet;
    if (ntp_receive_response(sockfd, (struct sockaddr*)&host_addr, &packet) <
        0) {
        iprintf("ntp_recv_packet: Failed\n");
        goto main_loop;
    }

    iprintf("NTP response received.\n");
    u64 unix_time = ntp_time_to_unix_time(packet.tx_tm_s, OFFSET_SECONDS);
    if (unix_to_rtc_words(unix_time, rtc_time) < 0) {
        iprintf("unix_to_rtc_words: Failed\n");
        goto main_loop;
    }
    debug_conversion(rtc_time);

    iprintf("Sending RTC time to ARM7\n");
    if (set_rtc_time_via_arm7(
          PxiChannel_User0, rtc_time, sizeof(rtc_time) / sizeof(u32)) < 0) {
        iprintf("RTC time send failed.\n");
        goto main_loop;
    }

    iprintf("RTC time sent successfully.\n");

main_loop:
    iprintf("Press START to exit\n");
    while (1) {
        swiWaitForVBlank();
        scanKeys();
        int keys = keysDown();
        if (keys & KEY_START)
            break;
    }
}
