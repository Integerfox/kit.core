#ifndef KIT_SYSTEM_WATCHDOG_WATCHEDTHREAD_H_
#define KIT_SYSTEM_WATCHDOG_WATCHEDTHREAD_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Container/ListItem.h"

///
namespace Kit {
///
namespace System {

/** This class represents a thread being monitored by the watchdog system.
    It contains the state information needed to track the thread's watchdog
    status. This class inherits from Container::ListItem to allow it to be
    stored in container classes like SList.
 */
class WatchedThread : public Kit::Container::ListItem
{
public:
    /** Constructor
        @param wdogTimeoutMs The watchdog timeout period for this thread in milliseconds
     */
    WatchedThread( unsigned long wdogTimeoutMs = 1000 ) noexcept;

    /// Virtual destructor
    virtual ~WatchedThread() = default;

public:
    /** Current countdown timer in milliseconds. This value is decremented by the
        Supervisor during each monitoring cycle. When it reaches zero, the thread
        is considered to have failed. Only accessed by the Supervisor.
     */
    unsigned long currentCountMs;

    /** Watchdog timeout period in milliseconds. This is the initial value that
        currentCountMs is reset to when the thread checks in. Only accessed by the Supervisor.
     */
    unsigned long wdogTimeoutMs;
};

/** WatchedRawThread - A concrete implementation for raw threads that provides
    wrapper methods for manual watchdog management via the Supervisor.
 */
class WatchedRawThread : public WatchedThread
{
public:
    /** Constructor
        @param wdogTimeoutMs The watchdog timeout period for this thread in milliseconds
     */
    WatchedRawThread( unsigned long wdogTimeoutMs = 1000 ) noexcept;

    /** Starts watchdog monitoring for this thread by registering it with the Supervisor.
        @return true if successfully started monitoring, false otherwise
     */
    bool startWatching() noexcept;

    /** Stops watchdog monitoring for this thread by unregistering it from the Supervisor.
        @return true if successfully stopped monitoring, false otherwise
     */
    bool stopWatching() noexcept;

    /** Kicks/reloads the watchdog timer for this thread via the Supervisor.
        This should be called periodically to indicate the thread is healthy.
     */
    void kickWatchdog() noexcept;


    /// Virtual destructor
    virtual ~WatchedRawThread() = default;
};

};  // end namespace System
};  // end namespace Kit
#endif  // end header latch