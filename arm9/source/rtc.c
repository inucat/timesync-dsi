#include "rtc.h"

#include <nds.h>

int
set_rtc_time_via_arm7(enum PxiChannel channel,
                      u32* rtc_words,
                      size_t word_count)
{
    if (rtc_words == NULL) {
        return -1;
    }

    pxiWaitRemote(channel);
    for (int i = 0; i < word_count; ++i) {
        u8 byte = (u8)(rtc_words)[i];
        u32 retval = pxiSendAndReceive(channel, byte);
        if (retval != i) {
            return -1;
        }
    }

    return 0;
}
