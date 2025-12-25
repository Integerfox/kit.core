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
#include "Kit/System/BareMetal/Thread.h"
#include "Kit/System/Mutex.h"
#include "Kit/System/PrivateStartup.h"
#include "Kit/System/Private.h"
#include "Hal.h"


//------------------------------------------------------------------------------
namespace Kit {
namespace System {

///
///
static Mutex theNullMutex_;

static bool schedulerStarted_ = false;

////////////////////////////////////////////////////////////////////////////////

void initialize() noexcept
{
    // Init the Colony.Core sub-systems
    IStartupHook::notifyStartupClients();
}

// This method returns when the Application's IRunnable instance completes
void enableScheduling() noexcept
{
    schedulerStarted_ = true; 
    Kit::System::BareMetal::Thread::entryPoint_();
}

bool isSchedulingEnabled() noexcept
{
    return schedulerStarted_;
}

void sleep( uint32_t milliseconds ) noexcept
{
    Kit::System::BareMetal::busyWait( milliseconds );
}

void sleepInRealTime( uint32_t milliseconds ) noexcept
{
    Kit::System::BareMetal::busyWait( milliseconds );
}

void suspendScheduling() noexcept
{
    // DO nothing
}

void resumeScheduling() noexcept
{
    // DO nothing
}

////////////////////////////////////////////////////////////////////////////////
Mutex& PrivateLocks::system() noexcept
{
    return theNullMutex_;
}


Mutex& PrivateLocks::tracing() noexcept
{
    return theNullMutex_;
}

Mutex& PrivateLocks::sysLists() noexcept
{
    return theNullMutex_;
}

Mutex& PrivateLocks::tracingOutput() noexcept
{
    return theNullMutex_;
}

}  // end namespace
}
//------------------------------------------------------------------------------