#ifndef KIT_SYSTEM_ITIMING_SOURCE_H_
#define KIT_SYSTEM_ITIMING_SOURCE_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/System/ICounter.h"

///
namespace Kit {
///
namespace System {

/** This abstract class defines the interface for a timing source.  The timing
    is ultimately responsible for decrementing the 'software timer' counters
 */
class ITimingSource
{
public:
    /// Register a software counter for notification
    virtual void attach( ICounter& clientToCallback ) noexcept = 0;

    /** Unregister for notification.  Returns true if the client was currently
        attached/registered; else false is returned.
     */
    virtual bool detach( ICounter& clientToCallback ) noexcept = 0;

    /// This method converts the requested duration in milliseconds to counter ticks.
    virtual uint32_t msecToCounts( uint32_t durationInMsecs ) const noexcept = 0;
};

}  // end namespaces
}
#endif  // end header latch
