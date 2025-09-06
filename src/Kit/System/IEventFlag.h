#ifndef KIT_SYSTEM_IEVENT_FLAG_H_
#define KIT_SYSTEM_IEVENT_FLAG_H_
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
#include "Kit/Container/SList.h"
#include <stdint.h>

/// 
namespace Kit {
/// 
namespace System {


/** This abstract class defines the call-back interface when an Event Flag
    has signaled.  Event Flags can be signaled from thread or from interrupt
    service routines.

    The Event Flag interface IS inherently thread safe.  
 */
class IEventFlag : public Kit::Container::ListItem
{
public:
    /** This method returns a event-flag-mask that indicates which event flag(s)
        are 'processed' by the event flag instance.  At least one bit should be
        set in the returned mask.  The bit positions in the mask correspond to the
        event flag numbers.  For example:
            o If the event flag instance processes only event flag 0, then the
              method should return 0x00000001
            o If the event flag instance processes only event flags 0, 1, and
              4, then the method should return 0x00000013
            o If the event flag instance processes all event flags, then the
              method should return 0xFFFFFFFF
     */
    virtual uint32_t getEventFlagsMask() const noexcept = 0;

    /** This method is called when the Event Flag is signaled.  It executes
        in the context of the thread of the IEventManager that was signaled.

        'eventBitIndex' is the zero based bit index of the event that was
        signaled.

        If the concrete Event Flag instance supports multiple event flags,
        then this method will be called multiple times - once for each event
        flag that was signaled.
     */
    virtual void notified( uint8_t eventBitIndex  ) noexcept = 0;

public:
    /// Virtual Destructor
    virtual ~IEventFlag() = default;

};

/** This is helper class that perform an auto-registration of the Event Flag
    instance with a list of Event Flags 

    The interface is NOT inherently thread safe.  
 */
class EventFlagBase: public IEventFlag
{
protected:
    /// Constructor
    EventFlagBase( Kit::Container::SList<IEventFlag>& eventList ) noexcept
    {
        eventList.put( *this );
    }

};

}       // end namespaces
}
#endif  // end header latch

