#include <nds.h>

#ifndef RTC_TIME_H
#define RTC_TIME_H

typedef struct rtc_time
{
    u8 year;    // Year; last 2 digits (e.g., 24 for 2024)
    u8 month;   // Month (1-12)
    u8 day;     // Day (1-31)
    u8 weekday; // Day of the week (0-6, where 0 = Sunday)
    u8 hour;    // Hour (0-23)
    u8 minute;  // Minute (0-59)
    u8 second;  // Second (0-59)
} rtc_time_t;

#endif // RTC_TIME_H
