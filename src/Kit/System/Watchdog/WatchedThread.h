#ifndef KIT_SYSTEM_WATCHDOG_WATCHED_THREAD_H_
#define KIT_SYSTEM_WATCHDOG_WATCHED_THREAD_H_
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
#include <stdint.h>

///
namespace Kit {
///
namespace System {
///
namespace Watchdog {

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
    WatchedThread( uint32_t wdogTimeoutMs ) noexcept;

public:
    /** Current countdown timer in milliseconds. This value is decremented by the
        Supervisor during each monitoring cycle. When it reaches zero, the thread
        is considered to have failed. Only accessed by the Supervisor.
     */
    uint32_t m_currentCountMs;

    /** Watchdog timeout period in milliseconds. This is the initial value that
        m_currentCountMs is reset to when the thread checks in. Only accessed by the Supervisor.
     */
    uint32_t m_wdogTimeoutMs;
};

}  // end namespace Watchdog
}  // end namespace System
}  // end namespace Kit
#endif  // end header latch