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
#include <sys/socket.h>
#include <sys/types.h>

// #include "nds/fifocommon.h"
#include "nds/interrupts.h"
#include "nds/ndstypes.h"
#include "nds/system.h"
#include "ntp.h"
#include "rtc_time.h"
#include "time.h"

#define NTP_SERVER "ntp.nict.jp"

// Convert decimal to BCD
// Note: RTC uses binary-coded decimal (BCD) format for time
static int
to_bcd(int value)
{
    if (value < 0 || value > 99) {
        iprintf("Error: Value %d is out of range for BCD conversion.\n", value);
        return -1;
    }
    return ((value / 10) << 4) | (value % 10);
}

static void
print_time_info(time_t time)
{
    iprintf(" timestamp: %lld\n", time);
    iprintf(" formatted: %s\n", ctime(&time));
}

static void
compose_rtc_time(rtc_time_t* rtc_time, const struct tm* tm_data)
{
    rtc_time->year = to_bcd(tm_data->tm_year % 100);
    rtc_time->month = to_bcd(tm_data->tm_mon + 1);
    rtc_time->day = to_bcd(tm_data->tm_mday);
    rtc_time->weekday = to_bcd(tm_data->tm_wday);
    rtc_time->hour = to_bcd(tm_data->tm_hour);
    rtc_time->minute = to_bcd(tm_data->tm_min);
    rtc_time->second = to_bcd(tm_data->tm_sec);
}

static struct tm
unix_time_to_tm(s64 unix_time)
{
    return *gmtime(&unix_time);
}

static void
tm_to_rtc_time(const struct tm* tm_data, rtc_time_t* rtc_time)
{
    rtc_time->year = to_bcd(tm_data->tm_year % 100);
    rtc_time->month = to_bcd(tm_data->tm_mon + 1);
    rtc_time->day = to_bcd(tm_data->tm_mday);
    rtc_time->weekday = to_bcd(tm_data->tm_wday);
    rtc_time->hour = to_bcd(tm_data->tm_hour);
    rtc_time->minute = to_bcd(tm_data->tm_min);
    rtc_time->second = to_bcd(tm_data->tm_sec);
}

static void
unix_to_rtc(s64 unix_time, rtc_time_t* rtc_time)
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

    compose_rtc_time(rtc_time, tm_data);
}

int
main(void)
{
    rtc_time_t rtc_time;

    consoleDemoInit();
    iprintf("= Timesync DSi =\n");

    iprintf("Waking up Wifi.\n");
    if (!Wifi_InitDefault(true)) {
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

    iprintf("Sending NTP request to %s (%s)\n",
            NTP_SERVER,
            inet_ntoa(host_addr.sin_addr));
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

    // Convert the transmit timestamp to Unix time
    iprintf("NTP response received.\n");
    s64 unix_time =
      ntohl(packet.tx_tm_s) - NTP_TIMESTAMP_DELTA + TIMEZONE_OFFSET;
    print_time_info(unix_time);
    unix_to_rtc(unix_time, &rtc_time);

    printf("RTCTime: %02x-%02x-%02x %x %02x:%02x:%02x\n",
           rtc_time.year,
           rtc_time.month,
           rtc_time.day,
           rtc_time.weekday,
           rtc_time.hour,
           rtc_time.minute,
           rtc_time.second);

    iprintf("Sending RTC time to ARM7\n");
    pxiWaitRemote(PxiChannel_User0);
    // -------------------------------------------------------------------------
    int i = 0;
    iprintf("Sending ");
    for (i = 0; i < sizeof(rtc_time); ++i) {
        u8 byte = ((u8*)&rtc_time)[i];
        iprintf("%02x ", byte);
        u32 retval = pxiSendAndReceive(PxiChannel_User0, byte);
        if (retval != i) {
            iprintf("pxiSendAndReceive: Failed\n");
            break;
        }
    }
    if (i != sizeof(rtc_time)) {
        iprintf("RTC time send failed.\n");
        goto main_loop;
    }
    // -------------------------------------------------------------------------

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
