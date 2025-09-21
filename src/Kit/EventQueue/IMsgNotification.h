#ifndef KIT_EVENT_QUEUE_IMSG_NOTIFICATION_H_
#define KIT_EVENT_QUEUE_IMSG_NOTIFICATION_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */


/// Forward reference to break dependency on the Itc namespace
namespace Kit {
namespace Itc {
class IMessage;
}
}

///
namespace Kit {
///
namespace EventQueue {

/** This abstract class represents the interface used to send ITC messages to a
    ITC mailbox. The ITC messaging has a client-server paradigm which means that
    this interface is always invoked by the client.
 */
class IMsgNotification
{
public:
    /** This operation is called by clients, which wish to send a message
        to the owner of this mailbox. The message is threaded into the
        mailbox queue for the mailbox owner to receive, and the operation
        returns immediately. The client relinquishes ownership of the memory
        until the message's returnToSender() function is invoked.
     */
    virtual void post( Kit::Itc::IMessage& msg ) noexcept = 0;

    /** This operation is called by clients which wish to send a message
        to the owner of this mailbox, and then block on the client's thread
        semaphore. First, the message is threaded into the mailbox queue
        for the mailbox owner to receive. Next, the client waits on its
        thread semaphore until the semaphore is signaled.
     */
    virtual void postSync( Kit::Itc::IMessage& msg ) noexcept = 0;


public:
    /// Virtual destructor
    virtual ~IMsgNotification() = default;
};


}  // end namespaces
}
#endif  // end header latch
