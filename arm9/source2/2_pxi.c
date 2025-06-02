#include "calico/nds/pxi.h"
#include <nds.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

struct DateTimePacket
{
    u8 year;
    u8 month;
    u8 day;
    u8 hour;
    u8 minute;
    u8 second;
};

int
main()
{
    lcdMainOnBottom();
    videoSetMode(MODE_0_2D);
    vramSetPrimaryBanks(VRAM_A_MAIN_BG, VRAM_B_LCD, VRAM_C_LCD, VRAM_D_LCD);
    // コンソール画面を初期化
    consoleInit(0, 0, BgType_Text4bpp, BgSize_T_256x256, 31, 0, true, true);
    iprintf("\e[20;0HSTART to quit");

    pxiWaitRemote(PxiChannel_User0);

    // 2020-03-01 12:34:56
    const struct DateTimePacket payload = {
        .year = 20, .month = 3, .day = 1, .hour = 12, .minute = 34, .second = 56
    };
    pxiSendAndReceive(PxiChannel_User0, payload.year);
    pxiSendAndReceive(PxiChannel_User0, payload.month);
    pxiSendAndReceive(PxiChannel_User0, payload.day);
    pxiSendAndReceive(PxiChannel_User0, payload.hour);
    pxiSendAndReceive(PxiChannel_User0, payload.minute);
    pxiSendAndReceive(PxiChannel_User0, payload.second);

    while (pmMainLoop()) {
        swiWaitForVBlank();
        scanKeys();
        int pressed = keysDown();

        // STARTボタンで終了
        if (pressed & KEY_START)
            return 0;

        time_t unixTime = time(NULL);
        struct tm* timeStruct = gmtime(&unixTime);

        int hours, seconds, minutes, day, month, year;
        hours = timeStruct->tm_hour;
        minutes = timeStruct->tm_min;
        seconds = timeStruct->tm_sec;
        day = timeStruct->tm_mday;
        month = timeStruct->tm_mon + 1;
        year = timeStruct->tm_year + 1900;

        iprintf("\e[1;1H%4d-%02d-%02d", year, month, day);
        iprintf("\e[2;1H%02d:%02d:%02d", hours, minutes, seconds);
    }
}
