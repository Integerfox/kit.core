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
static bool schedulerStarted_ = false;

////////////////////////////////////////////////////////////////////////////////

void initialize() noexcept
{
    // Init the KIT library sub-systems
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
    static NeverDestroyed<Mutex> theNullMutex_;
    return theNullMutex_.get();
}

Mutex& PrivateLocks::tracing() noexcept
{
    static NeverDestroyed<Mutex> theNullMutex_;
    return theNullMutex_.get();
}

Mutex& PrivateLocks::sysLists() noexcept
{
    static NeverDestroyed<Mutex> theNullMutex_;
    return theNullMutex_.get();
}

Mutex& PrivateLocks::tracingOutput() noexcept
{
    static NeverDestroyed<Mutex> theNullMutex_;
    return theNullMutex_.get();
}

}  // end namespace
}
//------------------------------------------------------------------------------