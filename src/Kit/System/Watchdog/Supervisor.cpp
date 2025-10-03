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

#include "Supervisor.h"
#include "Kit/System/FatalError.h"
#include <new>

using namespace Kit::System::Watchdog;

/////////////////////////
// Static member definitions
Kit::Container::SList<WatchedThread> Supervisor::m_watchedThreads;
Kit::System::Mutex* Supervisor::m_mutex = nullptr;
unsigned long Supervisor::m_tickDivider = 10;  // Default: monitor every 10 ticks
unsigned long Supervisor::m_currentTick = 0;
bool Supervisor::m_isEnabled = false;

/////////////////////////
bool Supervisor::enableWdog() noexcept
{
    ensureInitialized();

    if (!m_isEnabled)
    {
        m_isEnabled = Kit::System::Watchdog::enableWdog();
    }

    return m_isEnabled;
}

void Supervisor::tripWdog() noexcept
{
    Kit::System::Watchdog::tripWdog();
}

bool Supervisor::beginWatching(WatchedThread* threadToMonitor) noexcept
{
    if (!threadToMonitor)
    {
        return false;
    }

    ensureInitialized();

    if (!m_mutex)
    {
        return false;
    }

    m_mutex->lock();

    // Check if already being watched
    if (threadToMonitor->isBeingWatched)
    {
        m_mutex->unlock();
        return false;
    }

    // Reset the thread's countdown timer
    threadToMonitor->currentCountMs = threadToMonitor->wdogTimeoutMs;
    threadToMonitor->isBeingWatched = true;

    // Add to the list
    m_watchedThreads.put(*threadToMonitor);

    m_mutex->unlock();
    return true;
}

bool Supervisor::endWatching(WatchedThread* threadBeingMonitored) noexcept
{
    if (!threadBeingMonitored)
    {
        return false;
    }

    if (!m_mutex)
    {
        return false;
    }

    m_mutex->lock();

    // Check if actually being watched
    if (!threadBeingMonitored->isBeingWatched)
    {
        m_mutex->unlock();
        return false;
    }

    // Remove from the list
    m_watchedThreads.remove(*threadBeingMonitored);
    threadBeingMonitored->isBeingWatched = false;

    m_mutex->unlock();
    return true;
}

void Supervisor::monitorThreads() noexcept
{
    if (!m_isEnabled || !m_mutex)
    {
        return;
    }

    // Use tick divider to reduce monitoring frequency
    m_currentTick++;
    if (m_currentTick < m_tickDivider)
    {
        return;
    }
    m_currentTick = 0;

    m_mutex->lock();

    bool anyThreadFailed = false;

    // Iterate through all watched threads
    WatchedThread* current = m_watchedThreads.first();
    while (current != nullptr)
    {
        // Decrement the countdown timer
        if (current->currentCountMs > m_tickDivider)
        {
            current->currentCountMs -= m_tickDivider;
        }
        else
        {
            // Thread has timed out
            anyThreadFailed = true;
            break;
        }

        current = m_watchedThreads.next(*current);
    }

    m_mutex->unlock();

    if (anyThreadFailed)
    {
        // Trip the watchdog to force a reset
        tripWdog();
    }
    else if (m_watchedThreads.first() != nullptr)
    {
        // All threads are healthy, kick the hardware watchdog
        kickWdog();
    }
}

void Supervisor::reloadThread(WatchedThread* thread) noexcept
{
    if (!thread || !thread->isBeingWatched || !m_mutex)
    {
        return;
    }

    m_mutex->lock();

    // Reset the thread's countdown timer
    thread->currentCountMs = thread->wdogTimeoutMs;

    m_mutex->unlock();
}

void Supervisor::kickWdog() noexcept
{
    Kit::System::Watchdog::kickWdog();
}

bool Supervisor::initialize() noexcept
{
    if (m_mutex != nullptr)
    {
        return true;  // Already initialized
    }

    m_mutex = new(std::nothrow) Kit::System::Mutex();
    if (m_mutex == nullptr)
    {
        Kit::System::FatalError::logf(1, "Kit::System::Watchdog::Supervisor::initialize() - Failed to create mutex");
        return false;
    }

    return true;
}

void Supervisor::ensureInitialized() noexcept
{
    if (m_mutex == nullptr)
    {
        if (!initialize())
        {
            Kit::System::FatalError::logf(1, "Kit::System::Watchdog::Supervisor::ensureInitialized() - Failed to initialize");
        }
    }
}