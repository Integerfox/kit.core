#ifndef KIT_ITC_REQUEST_MESSAGE_H_
#define KIT_ITC_REQUEST_MESSAGE_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Itc/ServerMessage.h"
#include "Kit/Itc/SAP.h"


///
namespace Kit {
///
namespace Itc {

/** This concrete template class represents a service request message to a
    particular server. The SERVICE argument of the template is the abstract
    interface of the Service for which this Request message is associated with.

    This is the interface that the message uses to invoke the message operation.
    By definition, the SERVICE class must have a member function named "request"
    that returns nothing (void) and has a signature whose argument is a reference
    to the message defined by this template.
 */
template <class SERVICE, class PAYLOAD>
class RequestMessage : public ServerMessage
{
public:
    /// Constructor
    RequestMessage( SERVICE& srv, PAYLOAD& payload, IReturnHandler& returnHandler ) noexcept
        : ServerMessage( returnHandler )
        , m_srv( srv )
        , m_payload( payload )
    {
    }

    /// Constructor
    RequestMessage( SAP<SERVICE>& sapsrv, PAYLOAD& payload, IReturnHandler& returnHandler ) noexcept
        : ServerMessage( returnHandler ), m_srv( sapsrv.getService() ), m_payload( payload )
    {
    }

public:
    /// See Kit::Itc::Message
    void process() noexcept override
    {
        m_srv.request( *this );
    }

public:
    /// Returns the payload associated with this request
    PAYLOAD& getPayload() noexcept 
    {
        return m_payload;
    }

protected:
    /// Reference to the server interface whose 'request' will be called
    SERVICE& m_srv;

    /// Reference to the request payload
    PAYLOAD& m_payload;
};


}  // end namespaces
}
#endif  // end header latch
