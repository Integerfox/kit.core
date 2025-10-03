/*-----------------------------------------------------------------------------
* This file is part of the KIT C++ Class Library.  The KIT C++ Class Library
* is an open source project with a BSD type of licensing agreement.  See the 
* license agreement (license.txt) in the top/ directory or on the Internet at
* https://integerfox.com/kit/license.txt
*
* Copyright (c) 2025  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file */

#include "WatchedEventThread.h"
#include "Kit/System/FatalError.h"

using namespace Kit::System::Watchdog;

/////////////////////////
WatchedEventThread::WatchedEventThread(unsigned long wdogTimeoutMs, bool isSupervisor, unsigned long healthCheckIntervalMs) noexcept
    : WatchedThread(wdogTimeoutMs)
    , TimerComposer<WatchedEventThread>(*this, &WatchedEventThread::expired)
    , m_isSupervisor(isSupervisor)
    , m_healthCheckIntervalMs(healthCheckIntervalMs)
    , m_timerManager(nullptr)
    , m_isActive(false)
{
    // Set default health check interval to half of watchdog timeout if not specified
    if (m_healthCheckIntervalMs == 0)
    {
        m_healthCheckIntervalMs = wdogTimeoutMs / 2;
    }

    // Ensure health check interval is less than watchdog timeout
    if (m_healthCheckIntervalMs >= wdogTimeoutMs)
    {
        m_healthCheckIntervalMs = wdogTimeoutMs / 2;
        Kit::System::FatalError::logf(0, "WatchedEventThread: Health check interval must be less than watchdog timeout. Adjusted to %lu ms", m_healthCheckIntervalMs);
    }
}

WatchedEventThread::~WatchedEventThread()
{
    stopWatcher();
}

void WatchedEventThread::startWatcher(Kit::System::TimerManager& eventLoop) noexcept
{
    if (m_isActive)
    {
        return;  // Already started
    }

    m_timerManager = &eventLoop;

    // Register this thread with the supervisor
    if (!Supervisor::beginWatching(this))
    {
        Kit::System::FatalError::logf(1, "WatchedEventThread::startWatcher() - Failed to register with supervisor");
        return;
    }

    // Start the health check timer
    setTimingSource(*m_timerManager);
    start(m_healthCheckIntervalMs);

    m_isActive = true;
}

void WatchedEventThread::stopWatcher() noexcept
{
    if (!m_isActive)
    {
        return;  // Not started
    }

    // Stop the health check timer
    stop();

    // Unregister from the supervisor
    Supervisor::endWatching(this);

    m_isActive = false;
    m_timerManager = nullptr;
}

void WatchedEventThread::monitorWdog() noexcept
{
    if (!m_isActive)
    {
        return;
    }

    // If this is the supervisor thread, monitor all threads
    if (m_isSupervisor)
    {
        Supervisor::monitorThreads();
    }

    // Reload this thread's watchdog timer
    Supervisor::reloadThread(this);
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

void WatchedEventThread::expired() noexcept
{
    // Perform the health check
    if (!performHealthCheck())
    {
        // Health check failed - trip the watchdog immediately
        Supervisor::tripWdog();
        return;
    }

    // Health check passed - restart the timer for next check
    if (m_isActive)
    {
        start(m_healthCheckIntervalMs);
    }
}