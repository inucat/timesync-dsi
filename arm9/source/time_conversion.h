#ifndef TIME_CONVERSION_H
#define TIME_CONVERSION_H

#include <nds.h>

#include <time.h>

/**
 * @brief Converts a Unix timestamp to RtcDateTime member values.
 * @param unix_time The Unix timestamp to convert.
 * @param rtc_words Pointer to an array of u32 where the converted time
 * will be stored. Must have at least 7-word capacity.
 * @return 0 on success, or a negative error code on failure.
 *
 * @note The array of words represent the RtcDateTime member values, where
 * each element corresponds to a specific component of the time: year
 * (2000-2099), month (1-12), day (1-31), weekday (0=Sunday), hour (0-23),
 * minute (0-59), and second (0-59).
 */
int
unix_to_rtc_words(time_t unix_time, u32* rtc_words);

#endif // TIME_CONVERSION_H
