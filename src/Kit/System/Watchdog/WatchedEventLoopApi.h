#ifndef Kit_System_Watchdog_WatchedEventLoopApi_h_
#define Kit_System_Watchdog_WatchedEventLoopApi_h_
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

///
namespace Kit {
///
namespace System {

// Forward declaration - TimerManager is in Kit::System namespace
class TimerManager;

///
namespace Watchdog {

/** This abstract interface defines the API for watching/monitoring an EventLoop
    for watchdog purposes. The interface provides methods to start/stop monitoring
    and to perform periodic watchdog checks.
 */
class WatchedEventLoopApi
{
public:
    /** Starts the watchdog monitoring for this event loop.
        Should be called when the event loop begins execution.

        @param eventLoop Reference to the EventLoop instance that provides the timing source
     */
    virtual void startWatcher(Kit::System::TimerManager& eventLoop) noexcept = 0;

    /** Stops the watchdog monitoring for this event loop.
        Should be called when the event loop is shutting down.
     */
    virtual void stopWatcher() noexcept = 0;

    /** Performs watchdog monitoring check. This method should be called
        periodically during event loop execution to indicate the thread
        is still alive and processing events.
     */
    virtual void monitorWdog() noexcept = 0;

    /** Returns true if this is the supervisor thread responsible for
        monitoring other threads.

        @return true if this is the supervisor thread, false otherwise
     */
    virtual bool isSupervisorThread() const noexcept = 0;

    /// Virtual destructor
    virtual ~WatchedEventLoopApi() = default;
};

};      // end namespace Watchdog
};      // end namespace System
};      // end namespace Kit
#endif  // end header latch