#ifndef KIT_PERSISTENCE_RECORD_JOURNAL_IRESETREQUEST_H
#define KIT_PERSISTENCE_RECORD_JOURNAL_IRESETREQUEST_H
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Itc/SAP.h"
#include "Kit/Itc/RequestMessage.h"
#include "Kit/Itc/ResponseMessage.h"
#include "Kit/Persistence/Record/IPayload.h"
#include "Kit/Persistence/Record/Journal/IEntry.h"

///
namespace Kit {
///
namespace Persistence {
///
namespace Record {
///
namespace Journal {


////////////////////////////////////////////////////////////////////////////////
/** This abstract class define ITC message type and payload for the application
    to clear/delete all entries

    See the Kit/Itc/README.txt file for the semantics for the 'ownership' of the
    payload contents.

    NOTE: This interface can/should NOT be used synchronously.  The application
          is required to only use asynchronous semantics.
 */
class LogicalResetRequest
{
public:
    /// SAP for this API
    typedef Kit::Itc::SAP<LogicalResetRequest> SAP;

public:
    /// Payload for Message
    class Payload
    {
    public:
        /** Clear results (response field)
            true  = all entries where successfully cleared
            false = an error occurred
         */
        bool m_success;

    public:
        /// Constructor. Use for resetHead() message
        Payload()
            : m_success( false )
        {
        }
    };


    /// Message Type: LogicalReset
    typedef Kit::Itc::RequestMessage<LogicalResetRequest, Payload> LogicalResetMsg;

    /// Request: LogicalReset message
    virtual void request( LogicalResetMsg& msg ) = 0;

public:
    /// Virtual Destructor
    virtual ~LogicalResetRequest() {}
};

/** This abstract class define ITC message type and payload for asynchronous
    response (to the application) of a LogicalReset message.

    The Application is responsible for implementing the response method(s).
 */
class LogicalResetResponse
{
public:
    /// Response Message Type
    typedef Kit::Itc::ResponseMessage<LogicalResetResponse,
                                      LogicalResetRequest,
                                      LogicalResetRequest::Payload>
        LogicalResetMsg;

public:
    /// Response
    virtual void response( LogicalResetMsg& msg ) = 0;

public:
    /// Virtual destructor
    virtual ~LogicalResetResponse() {}
};


}  // end namespaces
}
}
}
#endif  // end header latch
