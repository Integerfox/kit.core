/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/System/Watchdog/Supervisor.h"
#include "Kit/System/ElapsedTime.h"

using namespace Kit::System;

/////////////////////////
// Static member definitions
Kit::Container::SList<WatchedThread> Supervisor::m_watchedThreads;
Mutex                                Supervisor::m_mutex;
uint32_t                             Supervisor::m_currentTick = 0;
bool                                 Supervisor::m_isEnabled   = false;
uint32_t                             Supervisor::m_timeMarker  = 0;

/////////////////////////

void Supervisor::beginWatching( WatchedThread& threadToMonitor ) noexcept
{
    Kit::System::Mutex::ScopeLock criticalSection( m_mutex );

    // Reset the thread's countdown timer
    threadToMonitor.currentCountMs = threadToMonitor.wdogTimeoutMs;

    // Add to the list
    m_watchedThreads.put( threadToMonitor );
}

void Supervisor::endWatching( WatchedThread& threadBeingMonitored ) noexcept
{
    Kit::System::Mutex::ScopeLock criticalSection( m_mutex );

    // Remove from the list
    m_watchedThreads.remove( threadBeingMonitored );
}

void Supervisor::monitorThreads() noexcept
{
    // Use tick divider to reduce monitoring frequency
    m_currentTick++;
    if ( m_currentTick >= OPTION_KIT_SYSTEM_WATCHDOG_SUPERVISOR_TICK_DIVIDER )
    {
        // Housekeeping
        m_currentTick  = 0;
        uint32_t now   = Kit::System::ElapsedTime::milliseconds();
        uint32_t delta = Kit::System::ElapsedTime::deltaMilliseconds( m_timeMarker, now );

        // Walk the list of monitored threads
        Kit::System::Mutex::ScopeLock criticalSection( m_mutex );
        WatchedThread*                thread = m_watchedThreads.first();
        while ( thread )
        {
            // Check for an expired thread timer
            if ( thread->currentCountMs <= delta )
            {
                tripWdog();  // EXPIRED
                return;      // Never executes because the tripWatchdog() method never returns, but it simplifies testing
            }

            // Decrement the thread's timer and get the next thread in the list
            thread->currentCountMs -= delta;
            thread                  = m_watchedThreads.next( *thread );
        }

        // If I get here - all monitored threads are 'healthy'
        kickWdog();
        m_timeMarker = now;
    }
}

void Supervisor::reloadThread( WatchedThread& thread ) noexcept
{
    Kit::System::Mutex::ScopeLock criticalSection( m_mutex );

    // Reset the thread's countdown timer
    thread.currentCountMs = thread.wdogTimeoutMs;
}
