#ifndef KIT_Z_STATE_SMITH_EVENTQUEUE_H_
#define KIT_Z_STATE_SMITH_EVENTQUEUE_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Container/RingBuffer.h"
#include "Kit/System/Shutdown.h"
#include "Kit/System/Trace.h"
#include "Kit/System/FatalError.h"
#include "Kit/Z/StateSmith/IFsm.h"

///
namespace Kit {
///
namespace Z {
///
namespace StateSmith {

/** Provides an event queue with run-to-completion semantics for StateSmith's
    auto-generated FSM code.  In addition, the class provides:
    - Tracing of event processing.
    - Optional event-completed callback. In general this functionality is NOT
      used.  However, for certain applications it is useful to have a 'hook'
      that is called when an event has been fully processed by the FSM.

    NOTE: The `nqbp/other/genfsm2.py` script is required to 'patch' the
          generated code to use this class
 */
template <int MAXEVENTS, typename EventId, typename StateId>
class EventQueue : public IFsm<EventId, StateId>, public Kit::Container::RingBuffer<EventId>
{
public:
    /// Constructor
    EventQueue()
        : IFsm<EventId, StateId>()
        , Kit::Container::RingBuffer<EventId>( m_eventQueueMemory, MAXEVENTS + 1 )
        , m_processingFsmEvent( false ){};

public:

    EventId m_eventQueueMemory[MAXEVENTS + 1];
    Kit::Container::RingBuffer<EventId> m_eventBuffer;
    ( m_eventQueueMemory, MAXEVENTS + 1 )
/// This method properly queues and process event messages
    void generateEvent( EventId msg )
    {
        // Queue my event
        if ( !this->add( msg ) )
        {
            Kit::System::FatalError::logf( Kit::System::Shutdown::eFSM_EVENT_OVERFLOW, "%s(%s): - Buffer Overflow!", tsection, this->getFsmName() );
        }

        // Protect against in-thread 'feedback loops' that can potentially generate events
        if ( !m_processingFsmEvent )
        {
            m_processingFsmEvent = true;
            while ( this->remove( msg ) )
            {
                KIT_SYSTEM_TRACE_MSG( tsection, "EVENT:= %s, current state=%s ...", this->eventIdToString( msg ), this->stateIdToString( this->getCurrentStateId() ) );
                this->dispatchEvent( msg );
                KIT_SYSTEM_TRACE_MSG( tsection, "-->Completed: end state=%s", this->stateIdToString( this->getCurrentStateId() ) );

                // Provide 'hook' for event-processing-completed
                hookEventCompleted( msg );
            }

            m_processingFsmEvent = false;
        }
    }

protected:
    /** Optional callback to the class implementing the FSM's context for when
        an event has been fully processed by the FSM. The method is called after
        the FSM has consumed the event.  The default behavior is a NOP.
     */
    virtual void hookEventCompleted( EventId msg ) noexcept
    {
        // Default behavior is a NOP
    }

protected:
    /// Memory for Event queue
    EventId m_eventQueueMemory[MAXEVENTS + 1];

    /// Flag for tracking re-entrant events
    bool m_processingFsmEvent;

    /// Section for tracing
    static constexpr const char* tsection = "Kit::Z::StateSmith::EventQueue";
};


}  // end namespaces
}
}
#endif  // end header latch
