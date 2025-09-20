#ifndef KIT_ITC_OPEN_ISERVICE_h_
#define KIT_ITC_OPEN_ISERVICE_h_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Itc/RequestMessage.h"
// #include "Kit/Itc/ResponseMessage.h"
#include "Kit/Itc/SAP.h"

///
namespace Kit {
///
namespace Itc {

/** This abstract class define message types and payloads for a set of ITC
    services. The request() method(s) are to be implemented by a 'server'
 */
class IOpenRequest
{
public:
    /// SAP for this API
    typedef Kit::Itc::SAP<IOpenRequest> SAP;

public:
    /** Payload for Message.
        IN --> data passed to the service
        OUT--> data returned from the service
     */
    struct Payload_T
    {
        /// IN: Optional arguments
        void* args;

        /// OUT: Pass/Fail result of the open request
        bool success;
    
        /// Constructor
        Payload_T( void* args = nullptr )
            : args( args )
            , success( true )
        {
        }
    };


    /// Message Type: Open
    typedef RequestMessage<IOpenRequest, Payload_T> OpenMsg;

public:
    /// Request: Open
    virtual void request( OpenMsg& msg ) noexcept = 0;

public:
    ///
    virtual ~IOpenRequest() = default;
};

#if 0
///////////////////////////////////////////////////////////////////////////////
/** This abstract class define response message types for a set of ITC services.
    The response() method(s) are to be implemented by the 'client'

    NOTE: Typically the 'Open Request' is done synchronously.  This interface
          is provided for completeness for the edge case of doing the Open
          Request asynchronously.
 */
class OpenResponse
{
public:
    /// Response Message Type: Open
    typedef ResponseMessage<OpenResponse,
                            IOpenRequest,
                            IOpenRequest::OpenPayload>
        OpenMsg;


public:
    /// Response: OpenMsg
    virtual void response( OpenMsg& msg ) = 0;


public:
    /// Virtual destructor
    virtual ~OpenResponse() {}
};
#endif

}  // end namespaces
}
#endif  // end header latch
