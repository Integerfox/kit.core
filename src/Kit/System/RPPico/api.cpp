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
    BSemaMutex::ScopeLock criticalSection( PrivateLocks::system() );
    if ( g_kitCore1IsRunning )
    {
        multicore_lockout_start_blocking();
    }
}

void resumeScheduling( void ) noexcept
{
    // NOTE: The multicore_lockout_xxx() method assume/require that BOTH cores are executing
    BSemaMutex::ScopeLock criticalSection( PrivateLocks::system() );
    if ( g_kitCore1IsRunning )
    {
        multicore_lockout_end_blocking();
    }
}


BSemaMutex& PrivateLocks::system( void ) noexcept
{
    static NeverDestroyed<BSemaMutex> systemMutex_;
    return systemMutex_.get();
}

Mutex& PrivateLocks::systemRecursive( void ) noexcept
{
    static NeverDestroyed<Mutex> systemRecursiveMutex_;
    return systemRecursiveMutex_.get();
}

BSemaMutex& PrivateLocks::tracing( void ) noexcept
{
    static NeverDestroyed<BSemaMutex> tracingMutex_;
    return tracingMutex_.get();
}

Mutex& PrivateLocks::tracingOutput( void ) noexcept
{
    static NeverDestroyed<Mutex> tracingOutputMutex_;
    return tracingOutputMutex_.get();
}

}  // end namespace
}
//------------------------------------------------------------------------------