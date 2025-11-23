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
#include <windows.h>



uint64_t Kit::Time::getAbsoluteTimeSeconds() noexcept
{
    return getAbsoluteTimeMilliseconds() / 1000;
}

uint64_t Kit::Time::getAbsoluteTimeMilliseconds() noexcept
{
    FILETIME ft;
    GetSystemTimeAsFileTime( &ft );
    
    // Convert FILETIME to 64-bit value (100-nanosecond intervals since Jan 1, 1601)
    uint64_t time64 = ( (uint64_t)ft.dwHighDateTime << 32 ) | ft.dwLowDateTime;
    
    // Convert to milliseconds since Unix epoch (Jan 1, 1970)
    // Difference between 1601 and 1970 is 11644473600 seconds
    const uint64_t UNIX_EPOCH_OFFSET = 116444736000000000ULL;
    time64 -= UNIX_EPOCH_OFFSET;
    
    return time64 / 10000; // Convert 100-nanosecond intervals to milliseconds
}
