#ifndef KIT_SYSTEM_WATCHDOG_WATCHEDEVENTTHREAD_H_
#define KIT_SYSTEM_WATCHDOG_WATCHEDEVENTTHREAD_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/System/IWatchedEventLoop.h"
#include "Kit/System/Watchdog/WatchedThread.h"
#include "Kit/System/Timer.h"
#include <cstdint>

///
namespace Kit {
///
namespace System {

/** This class implements the IWatchedEventLoop interface and provides
    watchdog monitoring capabilities for event loops. It inherits from
    WatchedThread to maintain watchdog state and has a dependency relationship
    with the Supervisor class.

    One instance of WatchedEventThread can be designated as the 'supervisor thread'
    which calls Supervisor::monitorThreads() every tick/event cycle.

    Event threads have a software notify timer that performs periodic health checks.
    If the health check fails, the watchdog is immediately tripped. The health check
    interval must be less than the thread's watchdog timeout interval. The health
    check is customizable on a per-thread basis via the performHealthCheck() virtual method.
 */
class WatchedEventThread : public IWatchedEventLoop, public WatchedThread
{
public:
    /** Constructor
        @param wdogTimeoutMs The watchdog timeout period for this thread in milliseconds
        @param healthCheckIntervalMs The health check interval in milliseconds (must be < wdogTimeoutMs)
        @param isSupervisor Set to true if this thread should act as the supervisor thread
     */
    WatchedEventThread( uint32_t wdogTimeoutMs, uint32_t healthCheckIntervalMs, bool isSupervisor = false ) noexcept;

public:
    /// See IWatchedEventLoop
    void startWatcher( Kit::System::TimerManager& timingSource ) noexcept override;

    /// See IWatchedEventLoop
    void stopWatcher() noexcept override;

    /// See IWatchedEventLoop
    void monitorWdog() noexcept override;

    /// See IWatchedEventLoop
    bool isSupervisorThread() const noexcept override;

protected:
    /** Performs a health check for this thread. This is called periodically by the
        software notify timer. If this method returns false, the watchdog will be
        immediately tripped to force a system reset.

        The default implementation always returns true (healthy). Derived classes
        can override this method to provide application-specific health checks.

        @return true if the thread is healthy, false if the watchdog should be tripped
     */
    virtual bool performHealthCheck() noexcept;

    /** Timer expired callback - called by the TimerComposer when the health check timer expires.
        This method performs the health check and restarts the timer.
     */
    void healthTimerExpired() noexcept;

protected:
    /// Timer composer for health check timer
    Kit::System::TimerComposer<WatchedEventThread> m_timer;

    /// Health check interval in milliseconds
    uint32_t m_healthCheckIntervalMs;

    /// Flag indicating if this is the supervisor thread
    bool m_isSupervisor;

    /// Flag indicating if the watcher is currently active
    bool m_isActive;
};

}  // end namespace System
}  // end namespace Kit
#endif  // end header latch