#ifndef KIT_DM_PERSISTENCE_IMANAGEREQUEST_H_
#define KIT_DM_PERSISTENCE_IMANAGEREQUEST_H_
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
namespace Persistence {


/** This abstract class define ITC messages for additional management operations
    on a Persistent DM Record

    See the Kit/Itc/README.txt file for the semantics for the 'ownership' of the
    payload contents.

    NOTE: Because these request WILL BLOCK until the persistent storage operations
          are completed - it is strongly recommended the interface is used
          asynchronously.
 */
class IManageRequest
{
public:
    /// SAP for this API
    typedef Kit::Itc::SAP<IManageRequest> SAP;

public:
    /// Payload for Message: Flush
    class FlushPayload
    {
    public:
        /** Flush results (response field)
            true  = the Record was successfully written to persistent storage
            false = an error occurred and the state of record in persistent storage is unknown
         */
        bool m_success;

    public:
        /// Constructor.
        FlushPayload()
            : m_success( false ) {}
    };

public:
    /// Payload for Message: Erase
    class ErasePayload
    {
    public:
        /** Erase results (response field)
            true  = the Record was successfully erased/invalidate in persistent storage
            false = an error occurred and the state of record in persistent storage is unknown
         */
        bool m_success;

    public:
        /// Constructor.
        ErasePayload()
            : m_success( false ) {}
    };

public:
    /// Message Type: Flush
    typedef Kit::Itc::RequestMessage<IManageRequest, FlushPayload> FlushMsg;

    /// Message Type: Erase
    typedef Kit::Itc::RequestMessage<IManageRequest, ErasePayload> EraseMsg;

public:
    /// Request: Flush message
    virtual void request( FlushMsg& msg ) = 0;

    /// Request: Erase message
    virtual void request( EraseMsg& msg ) = 0;

public:
    /// Virtual Destructor
    virtual ~IManageRequest() {}
};


/** This abstract class define ITC messages type and payload for asynchronous
    response (to the application) of a IManageRequest messages.

    The Application is responsible for implementing the response method(s).
 */
class IManageResponse
{
public:
    /// Response Message Type
    typedef Kit::Itc::ResponseMessage<IManageResponse,
                                      IManageRequest,
                                      IManageRequest::FlushPayload>
        FlushMsg;

public:
    /// Response
    virtual void response( FlushMsg& msg ) = 0;


public:
    /// Virtual destructor
    virtual ~IManageResponse() {}
};


};  // end namespaces
};
};
#endif  // end header latch
