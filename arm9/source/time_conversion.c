#include "time_conversion.h"

#include <nds.h>

#include <string.h>
#include <time.h>

/**
 * @brief Converts a struct tm to RtcDateTime member values.
 * @param words Pointer to an array of u32 where the converted time will be
 * stored. Must have at least 7-word capacity.
 * @param tm_data Pointer to a struct tm containing the time information to
 * convert.
 */
static void
tm_to_rtc_words(const struct tm* tm_data, u32* words)
{
    words[0] = tm_data->tm_year + 1900;
    words[1] = tm_data->tm_mon + 1;
    words[2] = tm_data->tm_mday;
    words[3] = tm_data->tm_wday;
    words[4] = tm_data->tm_hour;
    words[5] = tm_data->tm_min;
    words[6] = tm_data->tm_sec;
}

int
unix_to_rtc_words(time_t unix_time, u32* rtc_time)
{
    struct tm* tm_data = malloc(sizeof(struct tm));
    gmtime_r(&unix_time, tm_data);
    if (!tm_data) {
        free(tm_data);
        return -1;
    }

    tm_to_rtc_words(tm_data, rtc_time);

    free(tm_data);
    return 0;
}
