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
    time_t now;
    time( &now );
    return static_cast<uint64_t>( now );
}

uint64_t Kit::Time::getAbsoluteTimeMilliseconds() noexcept
{
    return getAbsoluteTimeSeconds() * 1000;
}
