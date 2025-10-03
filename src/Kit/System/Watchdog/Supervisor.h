#ifndef Kit_System_Watchdog_Supervisor_h_
#define Kit_System_Watchdog_Supervisor_h_
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

#include "WatchedThread.h"
#include "Hal.h"
#include "Kit/Container/SList.h"
#include "Kit/System/Mutex.h"

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

    /** Triggers the watchdog to reset the system. This is called when
        a monitored thread has failed to check in within its timeout period.
     */
    static void tripWdog() noexcept;

    /** Begins monitoring a thread. The thread will be added to the list
        of monitored threads.

        @param threadToMonitor Pointer to the WatchedThread to begin monitoring
        @return true if thread was successfully added to monitoring, false otherwise
     */
    static bool beginWatching(WatchedThread* threadToMonitor) noexcept;

    /** Stops monitoring a thread. The thread will be removed from the list
        of monitored threads.

        @param threadBeingMonitored Pointer to the WatchedThread to stop monitoring
        @return true if thread was successfully removed from monitoring, false otherwise
     */
    static bool endWatching(WatchedThread* threadBeingMonitored) noexcept;

    /** Monitors all registered threads. This method should be called periodically
        (typically every tick/event cycle) by the supervisor thread. It includes
        a tick divider to reduce the performance impact of being called frequently.

        This method checks each monitored thread's currentCountMs against its
        wdogTimeoutMs and triggers the watchdog if any thread has timed out.
     */
    static void monitorThreads() noexcept;

    /** Reloads/resets the watchdog counter for a specific thread. This is
        called by the thread to indicate it is still alive and processing.

        @param thread Pointer to the WatchedThread to reload
     */
    static void reloadThread(WatchedThread* thread) noexcept;

    /** Kicks the hardware watchdog. Should be called periodically when all
        monitored threads are healthy. Typically called by the supervisor thread.
     */
    static void kickWdog() noexcept;

private:
    /// List of threads being monitored
    static Kit::Container::SList<WatchedThread> m_watchedThreads;

    /// Mutex for thread-safe access to the watched threads list
    static Kit::System::Mutex* m_mutex;

    /// Tick divider to reduce monitoring frequency
    static unsigned long m_tickDivider;

    /// Current tick count for the divider
    static unsigned long m_currentTick;

    /// Flag indicating if watchdog system is enabled
    static bool m_isEnabled;

    /// Initialize the supervisor (called once)
    static bool initialize() noexcept;

    /// Check if initialization is needed and perform it
    static void ensureInitialized() noexcept;
};

};      // end namespace Watchdog
};      // end namespace System
};      // end namespace Kit
#endif  // end header latch