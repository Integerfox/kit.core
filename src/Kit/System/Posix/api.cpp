/*-----------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/

#include "Kit/System/api.h"
#include <time.h>

//------------------------------------------------------------------------------
namespace Kit {
namespace System {


////////////////////
void initialize() noexcept
{
    // TODO: IMPLEMENT ME - this is a placeholder for the actual implementation
}

void enableScheduling() noexcept
{
    // Do nothing since by definition we are running in user space
}

bool isSchedulingEnabled() noexcept
{
    return true;  // Always return true since scheduling is always enabled by definition of running in user space
}

void sleepInRealTime( uint32_t milliseconds ) noexcept
{
    // Convert milliseconds to the nanosleep time spec
    static const long nsec2msec = 1000000;
    static const long msec2sec  = 1000;
    time_t            sec       = milliseconds / msec2sec;
    long              nsec      = ( milliseconds % msec2sec ) * nsec2msec;
    const timespec    delay     = { sec, nsec };

    // Sleep
    nanosleep( &delay, 0 );
}


void suspendScheduling() noexcept
{
    // DO NOT Nothing.  No native POSIX API for this - and one could argue that suspending scheduling on Desktop OS is bad thing
}

void resumeScheduling() noexcept
{
    // DO NOT Nothing.  See above
}

}
}
//------------------------------------------------------------------------------
