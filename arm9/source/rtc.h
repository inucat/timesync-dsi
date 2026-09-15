#ifndef RTC_H
#define RTC_H

#include <nds.h>

/**
 * @brief Sets the RTC time via ARM7 using the provided words.
 * @param channel The PXI channel to use.
 * @param rtc_words A pointer to the words representing the RtcDateTime members.
 * @param word_count The number of words.
 * @return 0 on success, -1 on failure.
 */
int
set_rtc_time_via_arm7(enum PxiChannel channel,
                      u32* rtc_words,
                      size_t word_count);

#endif // RTC_H
