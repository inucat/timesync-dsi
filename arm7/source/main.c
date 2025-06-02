#include "calico/nds/pxi.h"
#include "calico/system/mailbox.h"
#include <calico.h>
#include <nds.h>
#include <stdio.h>

static Thread pxiThread;
alignas(8) static u8 pxiThreadStack[1024];

static u8
decToBcd(u8 value)
{
    return ((value / 10) << 4) | (value % 10);
}

static int
pxiThreadMain(void* arg)
{
    Mailbox mailbox;
    RtcDateTime rtcDateTime;
    u32 slots[6];
    mailboxPrepare(&mailbox, slots, 6);
    pxiSetMailbox(PxiChannel_User0, &mailbox);
    // mailboxRecv(&mailbox);

    u8 bytes[6];
    for (int i = 0; i < 6; i++) {
        bytes[i] = mailboxRecv(&mailbox) % 100;
        pxiReply(PxiChannel_User0, 0);
    }
    rtcDateTime.year = decToBcd(bytes[0]);
    rtcDateTime.month = decToBcd(bytes[1] % 12);
    rtcDateTime.day = decToBcd(bytes[2] % 32);
    rtcDateTime.hour = decToBcd(bytes[3] % 24);
    rtcDateTime.minute = decToBcd(bytes[4] % 60);
    rtcDateTime.second = decToBcd(bytes[5] % 60);

    rtcWriteRegister(RtcReg_DateTime, &rtcDateTime, sizeof(rtcDateTime));

    return 0;
}

int
main()
{
    envReadNvramSettings();
    lcdSetIrqMask(DISPSTAT_IE_ALL, DISPSTAT_IE_VBLANK);
    rtcInit();
    rtcSyncTime();
    pmInit();
    blkInit();
    wlmgrStartServer(MAIN_THREAD_PRIO - 8);
    irqEnable(IRQ_VBLANK | IRQ_VCOUNT | IRQ_NETWORK);

    threadPrepare(
      &pxiThread, pxiThreadMain, NULL, pxiThreadStack, MAIN_THREAD_PRIO);
    threadStart(&pxiThread);

    while (pmMainLoop())
        threadWaitForVBlank();
}
