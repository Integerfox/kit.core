#ifndef ITC_ASYNCHRONOUS_IRATE_RESPONSE_H_
#define ITC_ASYNCHRONOUS_IRATE_RESPONSE_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Itc/Synchronous/IRateRequest.h"
#include "Kit/Itc/ResponseMessage.h"


///
namespace Itc {
///
namespace Asynchronous {

/** This abstract class define message types and payloads for a set of ITC
    services. The response() method(s) are to be implemented by a 'client'
 */
class IRateResponse
{
public:
    /// Response Message Type: Rate
    typedef Kit::Itc::ResponseMessage<IRateResponse,
                                      Itc::Synchronous::IRateRequest,
                                      Itc::Synchronous::IRateRequest::RatePayload_T>
        RateMsg;


public:
    /// Response: RateMsg
    virtual void response( RateMsg& msg ) noexcept = 0;


public:
    /// Virtual destructor
    virtual ~IRateResponse() = default;
};


}  // end namespaces
}
#endif  // end header latch