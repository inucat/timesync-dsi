#include <nds.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

int
main()
{
    struct tm renewedTimeStruct = { 0, 0, 0, 1, 0, 2000 - 1900 };
    const time_t t = mktime(&renewedTimeStruct);
    const struct timeval timeval = { t, 0 };
    settimeofday(&timeval, NULL);

    while (pmMainLoop()) {
        swiWaitForVBlank();
    }

    return 0;
}
