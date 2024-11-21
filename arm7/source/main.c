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

#include "nds/arm7/clock.h"
#include "nds/arm7/serial.h"
#include "nds/fifocommon.h"
#include "nds/system.h"
#include <dswifi7.h>
#include <nds.h>
#include <stdbool.h>


#define FIFO_TO_7 FIFO_USER_01
#define FIFO_TO_9 FIFO_USER_02

volatile bool finished = false;

void
onVblankIrq(void)
{
    Wifi_Update();
}

int
main()
{
    irqInit();
    fifoInit();
    installWifiFIFO();

    irqSet(IRQ_VBLANK, onVblankIrq);
    irqEnable(IRQ_VBLANK | IRQ_VCOUNT | IRQ_NETWORK);

    RTCtime rtc_now;

    rtcGetTimeAndDate((uint8*)&rtc_now);
    fifoSendDatamsg(FIFO_TO_9, sizeof(RTCtime), (uint8*)&rtc_now);

    while (true) {
        if (!finished && fifoCheckDatamsg(FIFO_TO_7)) {
            finished = true;

            if ((fifoCheckDatamsgLength(FIFO_TO_7)) != sizeof(RTCtime)) {
                fifoSendValue32(FIFO_TO_9, -1);
            }

            RTCtime rtc;
            fifoGetDatamsg(FIFO_TO_7, sizeof(RTCtime), (uint8*)&rtc);
            rtcSetTimeAndDate((uint8*)&rtc);
            fifoSendValue32(FIFO_TO_9, 0);
        }
        swiWaitForVBlank();
    }
}
