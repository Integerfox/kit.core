#ifndef Kit_System_Watchdog_WatchedEventThread_h_
#define Kit_System_Watchdog_WatchedEventThread_h_
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

#include "WatchedEventLoopApi.h"
#include "WatchedThread.h"
#include "Supervisor.h"
#include "Kit/System/Timer.h"
#include "Kit/System/ElapsedTime.h"

///
namespace Kit {
///
namespace System {
///
namespace Watchdog {

/** This class implements the WatchedEventLoopApi interface and provides
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
class WatchedEventThread : public WatchedEventLoopApi, public WatchedThread, public TimerComposer<WatchedEventThread>
{
public:
    /** Constructor
        @param wdogTimeoutMs The watchdog timeout period for this thread in milliseconds
        @param isSupervisor Set to true if this thread should act as the supervisor thread
        @param healthCheckIntervalMs The health check interval in milliseconds (default: wdogTimeoutMs/2, must be < wdogTimeoutMs)
     */
    WatchedEventThread(unsigned long wdogTimeoutMs = 1000, bool isSupervisor = false, unsigned long healthCheckIntervalMs = 0) noexcept;

    /// Virtual destructor
    virtual ~WatchedEventThread();

public:
    /// See WatchedEventLoopApi
    void startWatcher(Kit::System::TimerManager& eventLoop) noexcept override;

    /// See WatchedEventLoopApi
    void stopWatcher() noexcept override;

    /// See WatchedEventLoopApi
    void monitorWdog() noexcept override;

    /// See WatchedEventLoopApi
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

        @param currentTick The current system tick when the timer expired
        @param currentTime The current elapsed time when the timer expired
     */
    void expired() noexcept;

private:
    /// Flag indicating if this is the supervisor thread
    bool m_isSupervisor;

    /// Health check interval in milliseconds
    unsigned long m_healthCheckIntervalMs;

    /// Pointer to the timer manager (EventLoop) for this thread
    Kit::System::TimerManager* m_timerManager;

    /// Flag indicating if the watcher is currently active
    bool m_isActive;
};

};      // end namespace Watchdog
};      // end namespace System
};      // end namespace Kit
#endif  // end header latch