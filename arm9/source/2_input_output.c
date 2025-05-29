#include "calico/gba/keypad.h"
#include "calico/nds/pm.h"
#include "nds/arm9/background.h"
#include "nds/arm9/console.h"
#include "nds/arm9/input.h"
#include "nds/arm9/sprite.h"
#include "nds/arm9/video.h"
#include "nds/interrupts.h"
#include "nds/system.h"
#include <nds.h>
#include <stdio.h>
#include <string.h>

int
main()
{
    lcdMainOnBottom();
    videoSetMode(MODE_0_2D);
    vramSetPrimaryBanks(VRAM_A_MAIN_BG, VRAM_B_LCD, VRAM_C_LCD, VRAM_D_LCD);
    // コンソール画面を初期化
    consoleInit(0, 0, BgType_Text4bpp, BgSize_T_256x256, 31, 0, true, true);

    int offsetMinutes = 0;

    iprintf("\e[1;0HUTC offset:");
    iprintf("\e[16;0HL/R to change by 1 hour");
    iprintf("\e[17;0H</> to change by 1 minute");
    iprintf("\e[18;0HA   to start sync");
    iprintf("\e[20;0HSTART to quit");

    while (pmMainLoop()) {
        swiWaitForVBlank();
        scanKeys();
        int pressed = keysDown();
        // int held = keysHeld();

        // STARTボタンで終了
        if (pressed & KEY_START) {
            break;
        }

        if (pressed & KEY_LEFT) {
            offsetMinutes--;
        }
        if (pressed & KEY_L) {
            offsetMinutes -= 60;
        }
        if (pressed & KEY_RIGHT) {
            offsetMinutes++;
        }
        if (pressed & KEY_R) {
            offsetMinutes += 60;
        }

        // if (held & KEY_A) {
        //     iprintf("\e[2;2HA");
        // } else {
        //     iprintf("\e[2;2H ");
        // }

        const char* sign = offsetMinutes >= 0 ? "+" : "-";
        const int hh =
          offsetMinutes >= 0 ? offsetMinutes / 60 : -offsetMinutes / 60;
        const int mm =
          offsetMinutes >= 0 ? offsetMinutes % 60 : -offsetMinutes % 60;
        iprintf("\e[1;16H%s%02d:%02d", sign, hh, mm);
    }

    return 0;
}
