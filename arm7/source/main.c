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

#define FIFO_TO_7 FIFO_USER_01
#define FIFO_TO_9 FIFO_USER_02

volatile bool finished = false;

// void
// onVblankIrq(void)
// {
//     Wifi_Update();
// }

int
main()
{
    // irqInit();
    //   fifoInit();
    // installWifiFIFO();

    // irqSet(IRQ_VBLANK, onVblankIrq);
    // irqEnable(IRQ_VBLANK | IRQ_VCOUNT | IRQ_NETWORK);
    RtcDateTime rtc_now;

    rtcReadDateTime(&rtc_now);
    //   fifoSendDatamsg(FIFO_TO_9, sizeof(RtcDateTime), (u8 *)&rtc_now);

    while (true) {
        // if (!finished && fifoCheckDatamsg(FIFO_TO_7)) {
        finished = true;

        //   if ((fifoCheckDatamsgLength(FIFO_TO_7)) != sizeof(RtcDateTime))
        //   {
        //     fifoSendValue32(FIFO_TO_9, -1);
        //   }

        RtcDateTime rtc;
        //   fifoGetDatamsg(FIFO_TO_7, sizeof(RtcDateTime), (u8 *)&rtc);
        //   rtcSetTimeAndDate((u8 *)&rtc);
        //   fifoSendValue32(FIFO_TO_9, 0);
        // }
        swiWaitForVBlank();
    }
}
