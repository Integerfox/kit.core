/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/System/api.h"
#include "Kit/System/Private.h"
#include "Kit/System/PrivateStartup.h"
#include "Windows.h"

//------------------------------------------------------------------------------
namespace Kit {
namespace System {

static Mutex systemMutex_;
static Mutex tracingMutex_;
static Mutex tracingOutputMutex_;
static Mutex sysList_;

////////////////////
void initialize() noexcept
{
    // Init the KIT Library sub-systems
    IStartupHook::notifyStartupClients();
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
    Sleep( static_cast<DWORD>(milliseconds) );
}


void suspendScheduling() noexcept
{
    // DO NOT Nothing.  No native Win32 API for this - and one could argue that suspending scheduling on Desktop OS is bad thing
}

void resumeScheduling() noexcept
{
    // DO NOT Nothing.  See above
}

////////////////////////////////////////////////////////////////////////////////
Mutex& PrivateLocks::system( void )
{
    return systemMutex_;
}


Mutex& PrivateLocks::tracing( void )
{
    return tracingMutex_;
}

Mutex& PrivateLocks::sysLists( void )
{
    return sysList_;
}

Mutex& PrivateLocks::tracingOutput( void )
{
    return tracingOutputMutex_;
}


}
}
//------------------------------------------------------------------------------
