#ifndef KIT_SYSTEM_WATCHDOG_RAWTHREAD_H_
#define KIT_SYSTEM_WATCHDOG_RAWTHREAD_H_
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
#include "WatchedThread.h"
#include "Supervisor.h"
#include <cstdint>

///
namespace Kit {
///
namespace System {
///
namespace Watchdog {

// ============================================================================
// Watchdog Macros for Raw Threads
// ============================================================================

/** Compile-time macros for instrumenting raw threads with watchdog monitoring.
    These macros can be enabled/disabled at compile time and support both
    direct function calls and pointer-based calls for flexibility.
 */

#ifdef USE_KIT_SYSTEM_WATCHDOG

/** Macro to start watchdog monitoring for a raw thread.
    Usage: KIT_SYSTEM_WATCHDOG_START_RAWTHREAD(threadPtr)

    @param threadPtr The RawThread instance pointer
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

    @param threadPtr The RawThread instance pointer
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

    @param threadPtr The RawThread instance pointer
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
#define KIT_SYSTEM_WATCHDOG_START_RAWTHREAD( threadPtr )

/// Watchdog framework disabled at compile time
#define KIT_SYSTEM_WATCHDOG_STOP_RAWTHREAD( threadPtr )

/// Watchdog framework disabled at compile time
#define KIT_SYSTEM_WATCHDOG_KICK_RAWTHREAD( threadPtr )

#endif  // USE_KIT_SYSTEM_WATCHDOG

/** RawThread - A concrete implementation for raw threads that provides
    wrapper methods for manual watchdog management via the Supervisor.
 */
class RawThread : public WatchedThread
{
public:
    /** Constructor
        @param wdogTimeoutMs The watchdog timeout period for this thread in milliseconds
     */
    RawThread( uint32_t wdogTimeoutMs ) noexcept
        : WatchedThread( wdogTimeoutMs )
    {
    }

    /** Starts watchdog monitoring for this thread by registering it with the Supervisor.
     */
    void startWatching() noexcept
    {
        Supervisor::beginWatching( *this );
    }

    /** Stops watchdog monitoring for this thread by unregistering it from the Supervisor.
     */
    void stopWatching() noexcept
    {
        Supervisor::endWatching( *this );
    }

    /** Kicks/reloads the watchdog timer for this thread via the Supervisor.
        This should be called periodically to indicate the thread is healthy.
     */
    void kickWatchdog() noexcept
    {
        Supervisor::reloadThread( *this );
    }
};

}  // end namespace Watchdog
}  // end namespace System
}  // end namespace Kit

#endif  // end header latch