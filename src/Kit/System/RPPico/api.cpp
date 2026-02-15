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
#include "Kit/System/Mutex.h"
#include "Kit/System/Private.h"
#include "Kit/System/PrivateStartup.h"
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

// Global flag indicating if the Kit C++ library has been initialized
bool g_kitInitialized;
void initialize( void ) noexcept
{
    // Init the Colony.Core sub-systems
    IStartupHook::notifyStartupClients();
    g_kitInitialized = true;
}


// NOTE: Simulated time is NOT supported
void sleep( uint32_t milliseconds ) noexcept
{
    sleep_ms( milliseconds );
}

void sleepInRealTime( uint32_t milliseconds ) noexcept
{
    sleep_ms( milliseconds );
}

////////////////////////////////////////////////////////////////////////////////

extern volatile bool g_kitCore1IsRunning;

void suspendScheduling( void ) noexcept
{
    // NOTE: The multicore_lockout_xxx() method assume/require that BOTH cores are executing
    Mutex::ScopeLock criticalSection( PrivateLocks::system() );
    if ( g_kitCore1IsRunning )
    {
        multicore_lockout_start_blocking();
    }
}

void resumeScheduling( void ) noexcept
{
    // NOTE: The multicore_lockout_xxx() method assume/require that BOTH cores are executing
    Mutex::ScopeLock criticalSection( PrivateLocks::system() );
    if ( g_kitCore1IsRunning )
    {
        multicore_lockout_end_blocking();
    }
}


Mutex& PrivateLocks::system( void ) noexcept
{
    return systemMutex_;
}

Mutex& PrivateLocks::sysLists( void ) noexcept
{
    return sysListMutex_;
}

Mutex& PrivateLocks::tracing( void ) noexcept
{
    return tracingMutex_;
}

Mutex& PrivateLocks::tracingOutput( void ) noexcept
{
    return tracingOutputMutex_;
}

}  // end namespace
}
//------------------------------------------------------------------------------