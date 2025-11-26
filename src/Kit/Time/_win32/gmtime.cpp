/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Time/gmtime.h"
#include "Kit/System/Assert.h"

struct tm* Kit::Time::gmtimeMT(const time_t *timep, struct tm *result) noexcept
{
    KIT_SYSTEM_ASSERT( timep != nullptr );
    KIT_SYSTEM_ASSERT( result != nullptr );

    int err = gmtime_s( result, timep );
    return ( err == 0 ) ? result : nullptr;
}
