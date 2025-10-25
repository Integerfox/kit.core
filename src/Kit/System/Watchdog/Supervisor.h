#ifndef KIT_SYSTEM_WATCHDOG_SUPERVISOR_H_
#define KIT_SYSTEM_WATCHDOG_SUPERVISOR_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "kit_config.h"
#include "Kit/System/Watchdog/WatchedThread.h"
#include "Kit/Container/SList.h"
#include "Kit/System/Mutex.h"
#include <cstdint>

/** Tick divider to reduce monitoring frequency. In this context a 'tick' means
    that the 'Supervisor' thread's executes a single iteration if its event loop.
    This is not the same thing as the platform system tick or any fixed interval
    of time.  See the Kit::System::EventLoop for more details on how an Event
    Loop executes
 */
#ifndef OPTION_KIT_SYSTEM_WATCHDOG_SUPERVISOR_TICK_DIVIDER
#define OPTION_KIT_SYSTEM_WATCHDOG_SUPERVISOR_TICK_DIVIDER 10
#endif

///
namespace Kit {
///
namespace System {
///
namespace Watchdog {

/** This class provides a centralized supervisor for monitoring all watched threads
    in the system. It maintains a list of threads being monitored and provides
    thread-safe methods for managing the watchdog system.

    All methods are static and thread-safe to allow access from multiple threads.
    The class uses a composition relationship with SList<WatchedThread> to manage
    the collection of monitored threads.
 */
class Supervisor
{
public:
    /** Enables the watchdog system. Must be called before any threads
        can be monitored.

        @return true if watchdog was successfully enabled, false otherwise
     */
    static bool enableWdog() noexcept;

    /** Begins monitoring a thread. The thread will be added to the list
        of monitored threads. Once a thread has been added to the Supervisor,
        this method can NOT be called again until after endWatching() has
        been called on the same thread.
        @param threadToMonitor Reference to the WatchedThread to begin monitoring
     */
    static void beginWatching( WatchedThread& threadToMonitor ) noexcept;

    /** Stops monitoring a thread. The thread will be removed from the list
        of monitored threads.
        @param threadBeingMonitored Reference to the WatchedThread to stop monitoring
     */
    static void endWatching( WatchedThread& threadBeingMonitored ) noexcept;

    /** Monitors all registered threads. This method should be called periodically
        (typically every tick/event cycle) by the supervisor thread. It includes
        a tick divider to reduce the performance impact of being called frequently.

        This method checks each monitored thread's currentCountMs against its
        wdogTimeoutMs and triggers the watchdog if any thread has timed out.
     */
    static void monitorThreads() noexcept;

    /** Reloads/resets the watchdog counter for a specific thread. This is
        called by the thread to indicate it is still alive and processing.

        @param thread Reference to the WatchedThread to reload
     */
    static void reloadThread( WatchedThread& thread ) noexcept;

    /** Kicks the hardware watchdog. Should be called periodically when all
        monitored threads are healthy. Typically called by the supervisor thread.
     */
    static void kickWdog() noexcept;

protected:
    /// List of threads being monitored
    static Kit::Container::SList<WatchedThread> m_watchedThreads;

    /// Mutex for thread-safe access to the watched threads list
    static Mutex m_mutex;

    /// Current tick count for the divider
    static uint32_t m_currentTick;

    /// Time marker for elapsed time calculations
    static uint32_t m_timeMarker;

    /// Flag indicating if watchdog system is enabled
    static bool m_isEnabled;
};

}  // end namespace Watchdog
}  // end namespace System
}  // end namespace Kit
#endif  // end header latch