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
    /** Constructor.  The 'myEventLoop' is the event loop for Runnable object
        (aka the Event Loop) where the messages posted to the Mailbox are
        RETRIEVED and their process() methods execute in.
     */
    Mailbox( Kit::System::ISignable& myEventLoop ) noexcept;

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
    /// The Event Loop/thread that I wait-on/dispatch-msgs-from
    Kit::System::ISignable& m_eventLoop;
};


}  // end namespaces
}
#endif  // end header latch