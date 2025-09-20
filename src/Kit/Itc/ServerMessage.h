#ifndef KIT_ITC_SERVER_MESSAGE_h_
#define KIT_ITC_SERVER_MESSAGE_h_
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
#include "Kit/Itc/IReturnHandler.h"

///
namespace Kit {
///
namespace Itc {

/** This partially concrete class represents a defined message, which is posted
    to a mailbox-server as a request. It includes members which enable the client
    to be notified when the server thread has completed the request.
 */
class ServerMessage : public IMessage
{
public:
    /// Constructor
    ServerMessage( IReturnHandler& rh ) noexcept
        : m_rh( rh )
    {
    }

    /** This operation is invoked by the server when it has completed the
        operation implemented as a part of this message. Use of this operation
        relinquishes the ownership of the message from the server (which invokes
        the operation) to the client.
     */
    inline void returnToSender() noexcept
    {
        m_rh.rts();
    }

protected:
    /** References the handler which is invoked by the ITC framework when the
        Server/Service returns ownership of the message to the client.
     */
    IReturnHandler& m_rh;
};


}  // end namespaces
}
#endif  // end header latch
