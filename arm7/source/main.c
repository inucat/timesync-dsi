/*---------------------------------------------------------------------------------

        default ARM7 core

                Copyright (C) 2005 - 2010
                Michael Noland (joat)
                Jason Rogers (dovoto)
                Dave Murphy (WinterMute)

        This software is provided 'as-is', without any express or implied
        warranty.  In no event will the authors be held liable for any
        damages arising from the use of this software.

        Permission is granted to anyone to use this software for any
        purpose, including commercial applications, and to alter it and
        redistribute it freely, subject to the following restrictions:

        1.	The origin of this software must not be misrepresented; you
                must not claim that you wrote the original software. If you use
                this software in a product, an acknowledgment in the product
                documentation would be appreciated but is not required.

        2.	Altered source versions must be plainly marked as such, and
                must not be misrepresented as being the original software.

        3.	This notice may not be removed or altered from any source
                distribution.

---------------------------------------------------------------------------------*/
#include <nds.h>

#include <calico.h>
#include <string.h>

// Management structure and stack space for PXI server thread
static Thread s_myServerThread;
alignas(8) static u8 s_myServerThreadStack[1024];

/**
 * @brief Converts an integer value to binary-coded decimal (BCD) format.
 * @param value The integer value to convert. Wrapped to the range 0-99 if
 * necessary.
 * @return The BCD representation of the value.
 * @note RTC uses BCD format for date and time.
 */
static int
to_bcd(int value)
{
    value = value % 100;
    return ((value / 10) << 4) | (value % 10);
}

static int
pxiThreadMain(void* arg)
{
    // Set up PXI mailbox, used to receive PXI command words
    Mailbox mb;
    u32 mb_slots[1] = { 0 };
    mailboxPrepare(&mb, mb_slots, 1);
    pxiSetMailbox(PxiChannel_User0, &mb);

    RtcDateTime network_time;

    // Main PXI message loop
    for (int i = 0;; ++i) {
        // Receive a message
        u32 msg = mailboxRecv(&mb);
        switch (i) {
            case 0:
                network_time.year = to_bcd(msg);
                break;
            case 1:
                network_time.month = to_bcd(msg);
                break;
            case 2:
                network_time.day = to_bcd(msg);
                break;
            case 3:
                network_time.weekday = to_bcd(msg);
                break;
            case 4:
                network_time.hour = to_bcd(msg);
                break;
            case 5:
                network_time.minute = to_bcd(msg);
                break;
            case 6:
                network_time.second = to_bcd(msg);
                rtcWriteRegister(
                  RtcReg_DateTime, &network_time, sizeof(RtcDateTime));
                break;
        }

        // Send a reply back to the ARM9
        pxiReply(PxiChannel_User0, i);
    }

    return 0;
}

int
main()
{
    // Read settings from NVRAM
    envReadNvramSettings();

    // Set up extended keypad server (X/Y/hinge)
    keypadStartExtServer();

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

    // Set up sound and mic driver
    soundStartServer(MAIN_THREAD_PRIO - 0x10);
    micStartServer(MAIN_THREAD_PRIO - 0x18);

    // Set up wireless manager
    wlmgrStartServer(MAIN_THREAD_PRIO - 8);

    // Set up Maxmod
    // mmInstall(MAIN_THREAD_PRIO + 1);

    // Set up server thread
    threadPrepare(&s_myServerThread,
                  pxiThreadMain,
                  NULL,
                  &s_myServerThreadStack[sizeof(s_myServerThreadStack)],
                  MAIN_THREAD_PRIO);
    threadStart(&s_myServerThread);

    // Keep the ARM7 mostly idle
    while (pmMainLoop()) {
        threadWaitForVBlank();
    }

    return 0;
}
