#ifndef KIT_SYSTEM_IEVENT_MANAGER_H_
#define KIT_SYSTEM_IEVENT_MANAGER_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include <stdint.h>

/// 
namespace Kit {
/// 
namespace System {


/** This abstract class defines the interface to signal a 'Event Flag'. Event
    Flags are used to indicate that an 'event' has occurred

    Each EventLoop supports up to 32 individual Event Flags (numbered 0-31).

    Event Flags are used to indicate that an 'event' has occurred.  Individual
    event flags can be viewed as binary semaphores with respect to be signaled/
    waiting (though waiting is done on the EventLoop's entire set of event flags).

    A EventLoop can wait for at least one event to be signaled. When the EventLoop
    is waiting on event(s) and it is then signaled - all of the Events that were
    in the signaled state when the EventLoop was unblocked are cleared.

    The IEventManager interface is inherently thread safe.  
 */
class IEventManager
{
public:
    /** This operation is called by clients to signal one or more Event Flags.
        Each bit in 'events' is different Event Flag.

        This method can ONLY be called from a thread context.
     */
    virtual void signalMultipleEvents( uint32_t events ) noexcept = 0;

    /** This operation is similar to signalEvents(), except that it sets one
        event flags.  The 'eventNumber' is the bit number (zero based) of the
        Event Flag to set.

        This method can ONLY be called from a thread context.
     */
    virtual void signalEvent( uint8_t eventNumber ) noexcept = 0;


public:
    /** This method is same as signalMultipleEvents() EXCEPT this method can
        ONLY be called from supervisor mode and/or ISR contexts.
     */
    virtual void su_signalMultipleEvents( uint32_t events ) noexcept = 0;

    /** This method is same as signalEvent() EXCEPT this method can ONLY be
        called from supervisor mode and/or ISR contexts.
     */
    virtual void su_signalEvent( uint8_t eventNumber ) noexcept = 0;

public:
    /// Virtual Destructor
    virtual ~IEventManager() = default;

};


}       // end namespaces
}
#endif  // end header latch

