#ifndef KIT_ITC_MAILBOX_H_
#define KIT_ITC_MAILBOX_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Itc/IMessage.h"
#include "Kit/EventQueue/IQueue.h"
#include "Kit/Container/SList.h"
#include "Kit/System/ISignable.h"
#include <type_traits>

/// Compile time check for the EventQueue being configured 'correctly'
static_assert( std::is_base_of<Kit::EventQueue::IMsgNotification, Kit::EventQueue::IQueue>::value,
               "IQueue must inherit from IMsgNotification" );

///
namespace Kit {
///
namespace Itc {

/** This mailbox class implements an Inter Thread Communications message
    queue. There is no limit to the number of messages that can be stored in
    the queue at any given time since the FIFO queue and the messages uses the
    intrusive container mechanisms from the Kit::Container namespace.
 */

class Mailbox : public Kit::EventQueue::IQueue,
                public Kit::Container::SList<IMessage>
{
public:
    /** Constructor.  The 'myEventQueue' is the event queue for Runnable object
        (aka thread) where the messages posted to the Mailbox are RETRIEVED and
        their process() method is called to execute them.
     */
    Mailbox( Kit::System::ISignable& myEventThread ) noexcept
        : m_eventThread( myEventThread )
    {
    }


public:
    /// See Kit::EventQueue::IMsgNotification
    void post( IMessage& msg ) noexcept override;

    /// See Kit::EventQueue::IMsgNotification
    void postSync( IMessage& msg ) noexcept override;


protected:
    /** This operation is used process any pending messages.
     */
    virtual void processMessages() noexcept;


    /** This method IS thread safe.

        This method returns true if there is at least one queued ITC message
     */
    bool isPendingMessage() noexcept;

protected:
    /// The Event Queue that I wait-on/dispatch-msgs-from
    Kit::System::ISignable& m_eventThread;
};


}  // end namespaces
}
#endif  // end header latch