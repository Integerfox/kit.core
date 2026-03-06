#ifndef Cpl_Dm_Persistent_FlushRequest_h_
#define Cpl_Dm_Persistent_FlushRequest_h_
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
#include "Kit/Itc/ResponseMessage.h"
#include "Kit/Itc/SAP.h"

///
namespace Kit {
///
namespace Dm {
///
namespace Persistent {


/** This abstract class define ITC message type and payload for the application
    to request flushing the Record to persistent storage

    See the Kit/Itc/README.txt file for the semantics for the 'ownership' of the
    payload contents.

    NOTE: This interface can/should NOT be used synchronously.  The application
          is required to only use asynchronous semantics.
 */
class FlushRequest
{
public:
    /// SAP for this API
    typedef Kit::Itc::SAP<FlushRequest> SAP;

public:
    /// Payload for Message: GetLatest
    class Payload
    {
    public:
        /** Flush results (response field)
            true  = the Record was successfully written to persistent storage
            false = an error occurred and the state of record in persistent storage is unknown
         */
        bool m_success;

    public:
        /// Constructor. 
        Payload() :m_success( false ) {}
    };


public:
    /// Message Type: GetLatest
    typedef Kit::Itc::RequestMessage<FlushRequest, Payload> FlushMsg;

    /// Request: Flush message
    virtual void request( FlushMsg& msg ) = 0;

public:
    /// Virtual Destructor
    virtual ~FlushRequest() {}
};


/** This abstract class define ITC message type and payload for asynchronous
    response (to the application) of a Flush message.

    The Application is responsible for implementing the response method(s).
 */
class FlushResponse
{
public:
    /// Response Message Type
    typedef Kit::Itc::ResponseMessage<FlushResponse,
                                      FlushRequest,
                                      FlushRequest::Payload> FlushMsg;

public:
    /// Response
    virtual void response( FlushMsg& msg ) = 0;


public:
    /// Virtual destructor
    virtual ~FlushResponse() {}
};


};      // end namespaces
};
};
#endif  // end header latch
