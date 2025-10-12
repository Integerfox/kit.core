#ifndef KIT_SYSTEM_WATCHDOG_IWATCHEDEVENTLOOP_H_
#define KIT_SYSTEM_WATCHDOG_IWATCHEDEVENTLOOP_H_
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

///
namespace Kit {
///
namespace System {

// Forward declaration - EventLoop is in Kit::System namespace
class EventLoop;

/** This abstract interface defines the API for watching/monitoring an EventLoop
    for watchdog purposes. The interface provides methods to start/stop monitoring
    and to perform periodic watchdog checks.

    NOTE: The classes method should ONLY be called/invoked using the above macros
 */
class IWatchedEventLoop
{
public:
    /** Starts the watchdog monitoring for this event loop.
        Should be called when the event loop begins execution.

        @param eventLoop Reference to the EventLoop instance that is being watched
     */
    virtual void startWatcher( Kit::System::EventLoop& eventLoop ) noexcept = 0;

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
    virtual ~IWatchedEventLoop() = default;
};

}  // end namespace System
}  // end namespace Kit

// ============================================================================
// Watchdog Macros
// ============================================================================

/** Compile-time macros for instrumenting EventLoop and raw threads with watchdog monitoring.
    These macros can be enabled/disabled at compile time and support both
    direct function calls and pointer-based calls for flexibility.
 */

#ifdef USE_KIT_SYSTEM_WATCHDOG

/** Macro to start watchdog monitoring in an EventLoop.
    Usage: KIT_SYSTEM_WATCHDOG_START_EVENTLOOP(thisPtr, evLoop)

    @param thisPtr The IWatchedEventLoop instance pointer
    @param evLoop Reference to the EventLoop instance that is being watched
 */
#define KIT_SYSTEM_WATCHDOG_START_EVENTLOOP( thisPtr, evLoop ) \
    do                                                         \
    {                                                          \
        if ( thisPtr )                                         \
        {                                                      \
            thisPtr->startWatcher( evLoop );                   \
        }                                                      \
    }                                                          \
    while ( 0 )

/** Macro to stop watchdog monitoring in an EventLoop.
    Usage: KIT_SYSTEM_WATCHDOG_STOP_EVENTLOOP(thisPtr)

    @param thisPtr The IWatchedEventLoop instance pointer
 */
#define KIT_SYSTEM_WATCHDOG_STOP_EVENTLOOP( thisPtr ) \
    do                                                \
    {                                                 \
        if ( thisPtr )                                \
        {                                             \
            thisPtr->stopWatcher();                   \
        }                                             \
    }                                                 \
    while ( 0 )

/** Macro to perform watchdog monitoring in an EventLoop.
    Usage: KIT_SYSTEM_WATCHDOG_EVENTLOOP_MONITOR(thisPtr)

    @param thisPtr The IWatchedEventLoop instance pointer
 */
#define KIT_SYSTEM_WATCHDOG_EVENTLOOP_MONITOR( thisPtr ) \
    do                                                   \
    {                                                    \
        if ( thisPtr && thisPtr->isSupervisorThread() )  \
        {                                                \
            thisPtr->monitorWdog();                      \
        }                                                \
    }                                                    \
    while ( 0 )

/** Macro to start watchdog monitoring for a raw thread.
    Usage: KIT_SYSTEM_WATCHDOG_START_RAWTHREAD(threadPtr)

    @param threadPtr The WatchedRawThread instance pointer
 */
#define KIT_SYSTEM_WATCHDOG_START_RAWTHREAD( threadPtr ) \
    do                                                   \
    {                                                    \
        if ( threadPtr )                                 \
        {                                                \
            threadPtr->startWatching();                  \
        }                                                \
    }                                                    \
    while ( 0 )

/** Macro to stop watchdog monitoring for a raw thread.
    Usage: KIT_SYSTEM_WATCHDOG_STOP_RAWTHREAD(threadPtr)

    @param threadPtr The WatchedRawThread instance pointer
 */
#define KIT_SYSTEM_WATCHDOG_STOP_RAWTHREAD( threadPtr ) \
    do                                                  \
    {                                                   \
        if ( threadPtr )                                \
        {                                               \
            threadPtr->stopWatching();                  \
        }                                               \
    }                                                   \
    while ( 0 )

/** Macro to kick/reload watchdog for a raw thread.
    Usage: KIT_SYSTEM_WATCHDOG_KICK_RAWTHREAD(threadPtr)

    @param threadPtr The WatchedRawThread instance pointer
 */
#define KIT_SYSTEM_WATCHDOG_KICK_RAWTHREAD( threadPtr ) \
    do                                                  \
    {                                                   \
        if ( threadPtr )                                \
        {                                               \
            threadPtr->kickWatchdog();                  \
        }                                               \
    }                                                   \
    while ( 0 )

#else

// When watchdog is disabled, macros expand to nothing

/// Watchdog framework disabled at compile time
#define KIT_SYSTEM_WATCHDOG_START_EVENTLOOP( thisPtr, evLoop )

/// Watchdog framework disabled at compile time
#define KIT_SYSTEM_WATCHDOG_STOP_EVENTLOOP( thisPtr )

/// Watchdog framework disabled at compile time
#define KIT_SYSTEM_WATCHDOG_EVENTLOOP_MONITOR( thisPtr )

/// Watchdog framework disabled at compile time
#define KIT_SYSTEM_WATCHDOG_START_RAWTHREAD( threadPtr )

/// Watchdog framework disabled at compile time
#define KIT_SYSTEM_WATCHDOG_STOP_RAWTHREAD( threadPtr )

/// Watchdog framework disabled at compile time
#define KIT_SYSTEM_WATCHDOG_KICK_RAWTHREAD( threadPtr )

#endif  // USE_KIT_SYSTEM_WATCHDOG

#endif  // end header latch