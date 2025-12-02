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
#include "Kit/System/Mutex.h"

static Kit::System::Mutex gmtimeMutex_;

struct tm* Kit::Time::gmtimeMT(const time_t *timep, struct tm *result) noexcept
{
    KIT_SYSTEM_ASSERT( timep != nullptr );
    KIT_SYSTEM_ASSERT( result != nullptr );

    Kit::System::Mutex::ScopeLock  criticalSection( gmtimeMutex_ );
    struct tm* tmp = gmtime( timep );
    if ( tmp != nullptr )
    {
        // Copy the result to the user provided struct tm instance
        *result = *tmp;
        return result;
    }
    return nullptr;
}
