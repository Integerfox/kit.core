/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "RawThread.h"
#include "Watchdog/Supervisor.h"

using namespace Kit::System;

/////////////////////////
RawThread::RawThread( uint32_t wdogTimeoutMs ) noexcept
    : Kit::System::Watchdog::WatchedThread( wdogTimeoutMs )
{
}

bool RawThread::startWatching() noexcept
{
    Watchdog::Supervisor::beginWatching( *this );
    return true;  // Method no longer fails, but maintain return type for compatibility
}

bool RawThread::stopWatching() noexcept
{
    Watchdog::Supervisor::endWatching( *this );
    return true;  // Method no longer fails, but maintain return type for compatibility
}

void RawThread::kickWatchdog() noexcept
{
    Watchdog::Supervisor::reloadThread( *this );
}