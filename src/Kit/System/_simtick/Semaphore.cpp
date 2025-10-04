/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/System/Semaphore.h"
#include "Kit/System/SimTick.h"
#include "Kit/System/Api.h"
#include "Kit/System/ElapsedTime.h"

//------------------------------------------------------------------------------
namespace Kit {
namespace System {


//////////////////////////////////////////////////
bool Semaphore::timedWait( uint32_t milliseconds ) noexcept
{
    // If not a simulated-tick thread -->do a 'regular Semaphore timed wait
    if ( !SimTick::usingSimTicks() )
    {
        return timedWaitInRealTime( milliseconds );
    }

    // Wait, 1 simulated tick at time, for the semaphore to be signaled
    uint32_t start = static_cast<uint32_t>( SimTick::current() );
    while ( milliseconds )
    {
        // Exit when the semaphore has been signaled
        if ( tryWait() )
        {
            return true;
        }

        // Wait (at least) 1 tick for something to happen
        sleep( 1 );

        // Update my count down timer
        uint32_t current = static_cast<uint32_t>( SimTick::current() );
        uint32_t delta   = ElapsedTime::deltaMilliseconds( start, current );
        start            = current;
        milliseconds     = milliseconds > delta ? milliseconds - delta : 0;
    }
    return false;
}


void Semaphore::wait() noexcept
{
    // If not a simulated-tick thread -->do a 'regular Semaphore wait
    if ( !SimTick::usingSimTicks() )
    {
        waitInRealTime();
        return;
    }

    SimTick::applicationWait();
    waitInRealTime();
}

}  // end namespace
}
//------------------------------------------------------------------------------