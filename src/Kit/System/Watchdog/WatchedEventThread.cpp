/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "WatchedEventThread.h"
#include "Supervisor.h"
#include "Hal.h"
#include "Kit/System/ElapsedTime.h"
#include "Kit/System/TimerManager.h"
#include "Kit/System/Assert.h"

using namespace Kit::System::Watchdog;

/////////////////////////
WatchedEventThread::WatchedEventThread( uint32_t wdogTimeoutMs, uint32_t healthCheckIntervalMs, bool isSupervisor ) noexcept
    : WatchedThread( wdogTimeoutMs )
    , m_timer( *this, &WatchedEventThread::healthTimerExpired )
    , m_healthCheckIntervalMs( healthCheckIntervalMs )
    , m_isSupervisor( isSupervisor )
    , m_isActive( false )
{
    KIT_SYSTEM_ASSERT( wdogTimeoutMs > healthCheckIntervalMs );
}

void WatchedEventThread::startWatcher( Kit::System::TimerManager& timingSource ) noexcept
{
    if ( m_isActive )
    {
        return;  // Already started
    }

    // Set the timing source for the notify timer
    m_timer.setTimingSource( timingSource );

    // Register this thread with the supervisor
    Supervisor::beginWatching( *this );

    // Start the health check timer
    m_timer.start( m_healthCheckIntervalMs );
    m_isActive = true;
}

void WatchedEventThread::stopWatcher() noexcept
{
    if ( !m_isActive )
    {
        return;  // Not started
    }

    // Stop the health check timer
    m_timer.stop();

    // Unregister from the supervisor
    Supervisor::endWatching( *this );
    m_isActive = false;
}

void WatchedEventThread::monitorWdog() noexcept
{
    if ( !m_isActive )
    {
        return;
    }
    Supervisor::monitorThreads();
}

bool WatchedEventThread::isSupervisorThread() const noexcept
{
    return m_isSupervisor;
}

bool WatchedEventThread::performHealthCheck() noexcept
{
    // Default implementation - always healthy
    // Derived classes can override this for custom health checks
    return true;
}

void WatchedEventThread::healthTimerExpired() noexcept
{
    // Perform the health check
    if ( !performHealthCheck() )
    {
        // Health check failed - trip the watchdog immediately
        Supervisor::tripWdog();
        return;
    }

    // Health check passed - reload this thread's watchdog timer
    Supervisor::reloadThread( *this );

    // Restart the timer for next check
    m_timer.start( m_healthCheckIntervalMs );
}