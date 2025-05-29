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

#include <stdint.h>

#define NTP_PORT     123           //< UDP port number for NTP
#define NTP_JAN_1970 2208988800ull //< 1900 to 1970 in seconds

struct ntp_short_format
{
    uint16_t seconds;
    uint16_t fraction;
};

struct ntp_timestamp_format
{
    uint32_t seconds;
    uint32_t fraction;
};

struct ntp_date_format
{
    uint32_t era_number;
    uint32_t era_offset;
    uint64_t fraction;
};

struct ntp_packet
{
    /**
     * @brief Leap Indicator (LI), Version Number (VN), and Mode
     *
     * @li LI - Flag to indicate the leap second is inserted or deleted in the
     * last minute of the current month
     * @li VN - NTP version == 4
     * @li Mode - Association mode (relationship between two NTP nodes)
     */
    uint8_t li_vn_mode;
    uint8_t stratum;   //< Server stratum (1..15)
    uint8_t poll;      //< Poll exponent
    uint8_t precision; //< Precision exponent

    /// Total round-trip delay to the reference clock, in NTP short format
    uint32_t root_delay;
    /// Total dispersion to the reference clock, in NTP short format
    uint32_t root_dispersion;
    /// Reference ID (Clock source)
    uint32_t ref_id;

    /// Reference timestamp: Time when the system clock was last set or
    /// corrected, in NTP timestamp format
    struct ntp_timestamp_format ref_tm;

    /// Origin timestamp: Time at the client when the request departed for the
    /// server, in NTP timestamp format
    struct ntp_timestamp_format orig_tm;

    /// Receive timestamp: Time at the server when the request
    /// arrived from the client, in NTP timestamp format
    struct ntp_timestamp_format rx_tm;

    /// Transmit timestamp: Time at the server when the
    /// response left for the client, in NTP timestamp format
    struct ntp_timestamp_format tx_tm;

    /*
     * Destination Timestamp (dst): Time at the client when the reply arrived
     * from the server, in NTP timestamp format.
     *
     * Note: The Destination Timestamp field is not included as a header field;
     * it is determined upon arrival of the packet and made available in the
     * packet buffer data structure.
     */
    // Key ID (32 b), Digest (128 b)
};

#define LI_NO_WARNING  0b00000000 //< No leap
#define LI_LAST_MIN_61 0b01000000 //< Last minute has 61 second
#define LI_LAST_MIN_59 0b10000000 //< Last minute has 59 second
#define LI_UNKNOWN     0b11000000 //< Unknown

#define VN_NTP_V3 0b00011000 //< NTPv3
#define VN_NTP_V4 0b00100000 //< NTPv4

#define MODE_RESERVED  0b00000000 //< Reserved
#define MODE_SYM_ACTI  0b00000001 //< Symmetric active
#define MODE_SYM_PASS  0b00000010 //< Symmetric passive
#define MODE_CLIENT    0b00000011 //< Client
#define MODE_SERVER    0b00000100 //< Server
#define MODE_BROADCAST 0b00000101 //< Broadcast
#define MODE_NTP_CTRL  0b00000110 //< NTP control message
#define MODE_PRIVATE   0b00000111 //< Reserved for private use

#endif // _NTP_H_
