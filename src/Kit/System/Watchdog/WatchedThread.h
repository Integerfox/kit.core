#ifndef Kit_System_Watchdog_WatchedThread_h_
#define Kit_System_Watchdog_WatchedThread_h_
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

#include "Kit/Container/ListItem.h"

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
    WatchedThread(unsigned long wdogTimeoutMs = 1000) noexcept;

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

    /** Flag indicating if this thread is currently being monitored.
        Only accessed by the Supervisor.
     */
    bool isBeingWatched;
};

};      // end namespace Watchdog
};      // end namespace System
};      // end namespace Kit
#endif  // end header latch