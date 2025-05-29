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

#include "calico/nds/arm7/rtc.h"
#include <calico.h>
#include <nds.h>
// #include <interrupts.h>
// #include <ndstypes.h>

volatile bool finished = false;

int
main()
{
    // Read settings from NVRAM
    envReadNvramSettings();

    // // Set up extended keypad server (X/Y/hinge)
    // keypadStartExtServer();

    // Configure and enable VBlank interrupt
    lcdSetIrqMask(DISPSTAT_IE_ALL, DISPSTAT_IE_VBLANK);
    irqEnable(IRQ_VBLANK);

    // Set up RTC
    rtcInit();
    rtcSyncTime();

    // Initialize power management
    pmInit();

    // Set up block device peripherals
    blkInit();

    // Set up touch screen driver
    touchInit();
    touchStartServer(80, MAIN_THREAD_PRIO);

    // // Set up sound and mic driver
    // soundStartServer(MAIN_THREAD_PRIO - 0x10);
    // micStartServer(MAIN_THREAD_PRIO - 0x18);

    // Set up wireless manager
    wlmgrStartServer(MAIN_THREAD_PRIO - 8);

    // fifoInit();

    // irqSet(IRQ_VBLANK, onVblankIrq);
    irqEnable(IRQ_VBLANK | IRQ_VCOUNT | IRQ_NETWORK);

    // RTCtime rtc_now;
    RtcDateTime rtcDateTime = { 0, 0, 0, 0, 0, 0, 0 };

    // rtcGetTimeAndDate((uint8*)&rtc_now);
    rtcWriteRegister(RtcReg_DateTime, &rtcDateTime, sizeof(rtcDateTime));
    // rtcWriteRegister(RtcReg_Time, 0, 0);

    // fifoSendDatamsg(FIFO_TO_9, sizeof(RTCtime), (uint8*)&rtc_now);

    while (pmMainLoop()) {
        // if (!finished && fifoCheckDatamsg(FIFO_TO_7)) {
        //     finished = true;

        //     if ((fifoCheckDatamsgLength(FIFO_TO_7)) != sizeof(RTCtime)) {
        //         fifoSendValue32(FIFO_TO_9, -1);
        //     }

        //     RTCtime rtc;
        //     fifoGetDatamsg(FIFO_TO_7, sizeof(RTCtime), (uint8*)&rtc);
        //     rtcSetTimeAndDate((uint8*)&rtc);
        //     fifoSendValue32(FIFO_TO_9, 0);
        // }
        threadWaitForVBlank();
    }
}
