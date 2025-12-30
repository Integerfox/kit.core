/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/System/Api.h"
#include "Kit/System/Private_.h"
#include "pico/sync.h"
#include "pico/multicore.h"

//------------------------------------------------------------------------------
namespace Kit {
namespace System {


///
static Mutex systemMutex_;
static Mutex tracingMutex_;
static Mutex tracingOutputMutex_;
static Mutex sysListMutex_;


////////////////////////////////////////////////////////////////////////////////
void Api::initialize( void )
{
    // Init the Colony.Core sub-systems
    StartupHook_::notifyStartupClients();
}


// NOTE: Simulated time is NOT supported
void Api::sleep( unsigned long milliseconds ) noexcept
{
    busy_wait_ms( milliseconds );
}

void sleepInRealTime( unsigned long milliseconds ) noexcept
{
    busy_wait_ms( milliseconds );
}

////////////////////////////////////////////////////////////////////////////////
extern void suspend_resume_scheduling_not_supported();
void Api::suspendScheduling( void )
{
    // NOT Supported.  Throw a link-time error
    //suspend_resume_scheduling_not_supported();

    // FIXME: In theory this call should put the 'other' core into a known/busy-wait
    // state - but I can't get it to work - the calling core just blocks forever :(. 
    //multicore_lockout_start_blocking();
}

void Api::resumeScheduling( void )
{
    // NOT Supported.  Throw a link-time error
    suspend_resume_scheduling_not_supported();

    // FIXME: See comment above
    //multicore_lockout_end_blocking();
}

////////////////////////////////////////////////////////////////////////////////
Mutex& Locks_::system( void )
{
    return systemMutex_;
}

Mutex& Locks_::sysLists( void )
{
    return sysListMutex_;
}

Mutex& Locks_::tracing( void )
{
    return tracingMutex_;
}

Mutex& Locks_::tracingOutput( void )
{
    return tracingOutputMutex_;
}

} // end namespace
}
//------------------------------------------------------------------------------