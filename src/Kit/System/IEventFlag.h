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
    service routines. There is no inherent ordering of Event Flag callbacks
    when multiple Event Flags are signaled.

    NOTE: Individual Event Flag instances can 'register' to be called back for
          multiple 'events'.  In addition, multiple Event Flag instances can
          register for the same event.

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

        'eventMask' is a bit mask with one or more bits set indicating
        which events were signaled.  The 'eventMask' will only contain 'set'
        bits for bit positions as returned by getEventFlagsMask().
     */
    virtual void notified( uint32_t eventMask ) noexcept = 0;

public:
    /// Virtual Destructor
    virtual ~IEventFlag() = default;
};

/** This is helper class that perform an auto-registration of the Event Flag
    instance with a list of Event Flags

    The interface is NOT inherently thread safe.
 */
class EventFlagBase : public IEventFlag
{
protected:
    /// Constructor
    EventFlagBase( Kit::Container::SList<IEventFlag>& eventList,
                   uint32_t eventFlagsMask ) noexcept
        : m_eventFlagsMask( eventFlagsMask )    
    {
        eventList.put( *this );
    }

    /// Default implementation
    uint32_t getEventFlagsMask() const noexcept
    {
        return m_eventFlagsMask;
    }
    
protected:
    /// Event Flags mask for this instance
    uint32_t m_eventFlagsMask;
};

}  // end namespaces
}
#endif  // end header latch
