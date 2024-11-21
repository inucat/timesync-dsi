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

#include "offset.h"

#include <nds.h>

#include <dswifi9.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>

#include "nds/fifocommon.h"
#include "nds/interrupts.h"
#include "nds/ndstypes.h"
#include "nds/system.h"
#include "ntp.h"
#include "time.h"

#define NTP_SERVER "ntp.nict.jp"
#define FIFO_TO_7  FIFO_USER_01
#define FIFO_TO_9  FIFO_USER_02

// Prepare the RTC register values
// Note: RTC uses binary-coded decimal (BCD) format for time
// Convert decimal to BCD
#define DEC_TO_BCD(x) (((x) / 10) << 4) | ((x) % 10)

static void
print_time_info(time_t time)
{
    iprintf(" timestamp: %lld\n", time);
    iprintf(" formatted: %s\n", ctime(&time));
}

static void
compose_rtctime(RTCtime* rtctime,
                uint16 year,
                uint8 mon,
                uint8 day,
                uint8 wday,
                uint8 hour,
                uint8 min,
                uint8 sec)
{
    rtctime->year = DEC_TO_BCD(year % 100);
    rtctime->month = DEC_TO_BCD(mon);
    rtctime->day = DEC_TO_BCD(day);
    rtctime->weekday = DEC_TO_BCD(wday);
    rtctime->hours = DEC_TO_BCD(hour);
    rtctime->minutes = DEC_TO_BCD(min);
    rtctime->seconds = DEC_TO_BCD(sec);
}

static void
unix_to_rtc(int64 unix_time, RTCtime* rtc_time)
{
    struct tm* tm_data = gmtime(&unix_time);

    printf("struct tm: %04d-%02d-%02d %d %2d:%02d:%02d\n",
           tm_data->tm_year,
           tm_data->tm_mon,
           tm_data->tm_mday,
           tm_data->tm_wday,
           tm_data->tm_hour,
           tm_data->tm_min,
           tm_data->tm_sec);

    compose_rtctime(rtc_time,
                    tm_data->tm_year,
                    tm_data->tm_mon + 1,
                    tm_data->tm_mday,
                    tm_data->tm_wday,
                    tm_data->tm_hour,
                    tm_data->tm_min,
                    tm_data->tm_sec);

    printf("RTCTime: %04d-%02d-%02d %d %2d:%02d:%02d\n",
           rtc_time->year,
           rtc_time->month,
           rtc_time->day,
           rtc_time->weekday,
           rtc_time->hours,
           rtc_time->minutes,
           rtc_time->seconds);
}

int
main(void)
{
    RTCtime rtc_time;

    consoleDemoInit();
    iprintf("= Timesync DSi =\n");
    while (1) {
        if (fifoCheckDatamsg(FIFO_TO_9)) {
            fifoGetDatamsg(FIFO_TO_9, sizeof(RTCtime), (uint8*)&rtc_time);
            printf("RTCTime: %04d-%02d-%02d %d %2d:%02d:%02d\n",
                   rtc_time.year,
                   rtc_time.month,
                   rtc_time.day,
                   rtc_time.weekday,
                   rtc_time.hours,
                   rtc_time.minutes,
                   rtc_time.seconds);
            break;
        }
        swiWaitForVBlank();
    }

    // iprintf("Current time on DSi:\n");
    // print_time_info(time(NULL));

    iprintf("Waking up Wifi.\n");
    if (!Wifi_InitDefault(WFC_CONNECT)) {
        iprintf("Wifi_InitDefault: Failed\n");
        goto main_loop;
    }

    iprintf("Creating socket.\n");
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        iprintf("socket: Failed\n");
        goto main_loop;
    }

    iprintf("Resolving server address.\n");
    struct hostent* host = gethostbyname(NTP_SERVER);
    if (!host) {
        iprintf("gethostbyname: Failed\n");
        goto main_loop;
    }

    // Prepare address info
    struct sockaddr_in host_addr;
    memset(&host_addr, 0, sizeof(host_addr));
    host_addr.sin_family = AF_INET;
    host_addr.sin_port = htons(NTP_PORT);
    memcpy(&host_addr.sin_addr.s_addr, host->h_addr_list[0], host->h_length);

    if (ntp_request_sync(sockfd, (struct sockaddr*)&host_addr) < 0) {
        iprintf("ntp_request_sync: Failed\n");
        goto main_loop;
    }

    struct ntp_packet packet;
    if (ntp_recv_packet(sockfd, (struct sockaddr*)&host_addr, &packet) < 0) {
        iprintf("ntp_recv_packet: Failed\n");
        goto main_loop;
    }

    // Convert the transmit timestamp to Unix time
    int64 unix_time =
      ntohl(packet.tx_tm_s) - NTP_TIMESTAMP_DELTA + TIMEZONE_OFFSET;
    print_time_info(unix_time);
    unix_to_rtc(unix_time, &rtc_time);

    iprintf("Sending RTCData to ARM7\n");
    if (!fifoSendDatamsg(FIFO_TO_7, sizeof(RTCtime), (void*)&rtc_time)) {
        iprintf("fifoSendDatamsg: Failed\n");
    }

main_loop:
    while (1) {
        if (fifoCheckValue32(FIFO_TO_9)) {
            int result = fifoGetValue32(FIFO_TO_9);
            if (result == 0) {
                iprintf("Successfully synced!\n");
            } else {
                iprintf("Failed to sync!\n");
            }
            iprintf("Press START to exit\n");
        }
        swiWaitForVBlank();

        int keys = keysDown();
        if (keys & KEY_START)
            break;
    }
}
