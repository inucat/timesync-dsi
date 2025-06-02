#include <nds.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

static const char* MONTHS[12] = { "January", "February", "March",
                                  "April",   "May",      "June",
                                  "July",    "August",   "September",
                                  "October", "November", "December" };

static const char* WEEKDAYS[7] = { "Sunday",   "Monday", "Tuesday", "Wednesday",
                                   "Thursday", "Friday", "Saturday" };
static const u16 FIRST_DAYS_OF_MONTHS[12] = {
    0 % 7,   // januari		31
    31 % 7,  // februari		28+1(leap year)
    59 % 7,  // maart			31
    90 % 7,  // april			30
    120 % 7, // mei			31
    151 % 7, // juni			30
    181 % 7, // juli			31
    212 % 7, // augustus		31
    243 % 7, // september		30
    273 % 7, // oktober		31
    304 % 7, // november		30
    334 % 7  // december		31
};

#define IS_LEAP_YEAR(year)                                                     \
    ((year) % 400 == 0 || ((year) % 100 != 0 && (year) % 4 == 0))

unsigned
getDayOfWeek(unsigned day, unsigned month, unsigned year)
{
    // http://en.wikipedia.org/wiki/Calculating_the_day_of_the_week

    day += 2 * (3 - ((year / 100) % 4));
    year %= 100;
    day += year + (year / 4);
    day += FIRST_DAYS_OF_MONTHS[month] - (IS_LEAP_YEAR(year) && (month <= 1));
    return day % 7;
}

int
m_ain()
{
    lcdMainOnBottom();
    videoSetMode(MODE_0_2D);
    vramSetPrimaryBanks(VRAM_A_MAIN_BG, VRAM_B_LCD, VRAM_C_LCD, VRAM_D_LCD);
    // コンソール画面を初期化
    consoleInit(0, 0, BgType_Text4bpp, BgSize_T_256x256, 31, 0, true, true);

    int hours, seconds, minutes, day, month, year;

    // iprintf("\e[1;0HUTC offset:");
    // iprintf("\e[16;0HL/R to change by 1 hour");
    // iprintf("\e[17;0H</> to change by 1 minute");
    // iprintf("\e[18;0HA   to start sync");
    iprintf("\e[20;0HSTART to quit");

    // struct tm renewedTimeStruct = { 0, 0, 0, 1, 0, 2000 - 1900 };
    // const time_t t = mktime(&renewedTimeStruct);
    // const struct timeval timeval = { t, 0 };
    // settimeofday(&timeval, NULL);

    while (pmMainLoop()) {
        swiWaitForVBlank();
        scanKeys();
        int pressed = keysDown();
        // int held = keysHeld();

        // STARTボタンで終了
        if (pressed & KEY_START) {
            break;
        }

        time_t unixTime = time(NULL);
        struct tm* timeStruct = gmtime(&unixTime);

        hours = timeStruct->tm_hour;
        minutes = timeStruct->tm_min;
        seconds = timeStruct->tm_sec;
        day = timeStruct->tm_mday;
        month = timeStruct->tm_mon;
        year = timeStruct->tm_year + 1900;

        printf("\x1b[2J%02i:%02i:%02i", hours, minutes, seconds);
        printf("\n%s %s %i %i",
               WEEKDAYS[getDayOfWeek(day, month, year)],
               MONTHS[month],
               day,
               year);
        // clock_settime(clockid_t clock_id, const struct timespec *tp)
    }

    return 0;
}
