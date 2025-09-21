#ifndef KIT_ITC_ASYNC_RETURN_HANDLER_H_
#define KIT_ITC_ASYNC_RETURN_HANDLER_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/EventQueue/IQueue.h"
#include "Kit/Itc/IReturnHandler.h"
#include <type_traits>


/// Compile time check for the EventQueue being configured 'correctly'
static_assert( std::is_base_of<Kit::EventQueue::IMsgNotification, Kit::EventQueue::IQueue>::value,
               "IQueue must inherit from IMsgNotification" );


///
namespace Kit {
///
namespace Itc {


/** This class implements an asynchronous ReturnHandler. When the rts() method
    of the message associated with this class is invoked, the message referenced
    as a member variable of this class is posted to the mailbox referenced as
    a member variable of this class. It is expected that the referenced event
    loop belongs to the client (i.e. the sender of the original message), and that
    the message contains a reference to the original message, such that the
    original message can be released by the client.
 */
class AsyncReturnHandler : public IReturnHandler
{
public:
    /** The constructor initializes the response mailbox and message references.
     */
    AsyncReturnHandler( Kit::EventQueue::IQueue& clientEventQueue, IMessage& msg ) noexcept
        : m_clientEventQueue( clientEventQueue )
        , m_responseMsg( msg )
    {
    }


public:
    /// See Kit::Itc::IReturnHandler
    void rts() noexcept override
    {
        m_clientEventQueue.post( m_responseMsg );
    }

protected:
    /// Refers to the eventLoop/thread to which the response message will be posted.
    EventQueue::IQueue& m_clientEventQueue;

    /// Refers to the response message to be posted to the response mailbox.
    IMessage& m_responseMsg;
};

}  // end namespaces
}
#endif  // end header latch
