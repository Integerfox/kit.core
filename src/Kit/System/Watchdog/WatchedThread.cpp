/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/System/Watchdog/WatchedThread.h"
#include "Kit/System/Watchdog/Supervisor.h"

using namespace Kit::System;

/////////////////////////
WatchedThread::WatchedThread( unsigned long wdogTimeoutMs ) noexcept
    : currentCountMs( wdogTimeoutMs )
    , wdogTimeoutMs( wdogTimeoutMs )
{
}

/////////////////////////
WatchedRawThread::WatchedRawThread( unsigned long wdogTimeoutMs ) noexcept
    : WatchedThread( wdogTimeoutMs )
{
}

bool WatchedRawThread::startWatching() noexcept
{
    Supervisor::beginWatching( *this );
    return true;  // Method no longer fails, but maintain return type for compatibility
}

bool WatchedRawThread::stopWatching() noexcept
{
    Supervisor::endWatching( *this );
    return true;  // Method no longer fails, but maintain return type for compatibility
}

void WatchedRawThread::kickWatchdog() noexcept
{
    Supervisor::reloadThread( *this );
}