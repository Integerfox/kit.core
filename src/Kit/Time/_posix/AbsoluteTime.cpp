/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Time/AbsoluteTime.h"
#include <time.h>



uint64_t Kit::Time::getAbsoluteTimeSeconds() noexcept
{
    return getAbsoluteTimeMilliseconds() / 1000;
}

uint64_t Kit::Time::getAbsoluteTimeMilliseconds() noexcept
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (uint64_t)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}
