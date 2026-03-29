#ifndef ITC_SYNCHRONOUS_IRATE_REQUEST_H_
#define ITC_SYNCHRONOUS_IRATE_REQUEST_H_
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
#include "Kit/Itc/SAP.h"

///
namespace Itc {
///
namespace Synchronous {

/** This abstract class define message types and payloads for a set of ITC
    services. The request() method(s) are to be implemented by a 'service'
 */
class IRateRequest
{
public:
    /// SAP for this API
    typedef Kit::Itc::SAP<IRateRequest> SAP;

public:
    /** Payload for Message.
        IN --> data passed to the service
        OUT--> data returned from the service
     */
    struct RatePayload_T
    {
        /** IN: The flash rate in milliseconds.  For 1Hz flash rate, this value 
                would be 500ms (i.e. 500ms on, 500ms off).  The valid flash
                rate range is [50ms, 5000ms].  Values outside of this range
                will cause the request to fail.
         */
        uint32_t flashRateMs;

        /** OUT: Pass/Fail result of the rate change (i.e. returns false if the
                 flash rate is too large or too small)
         */
        bool success;

        /// Constructor
        RatePayload_T( uint32_t flashRateMs = 500 ) // Default to 1Hz
            : flashRateMs( flashRateMs )
            , success( false )
        {
        }
    };


    /// Message Type: Rate
    typedef Kit::Itc::RequestMessage<IRateRequest, RatePayload_T> RateMsg;

public:
    /// ITC Request: Rate
    virtual void request( RateMsg& msg ) noexcept = 0;

public:
    ///
    virtual ~IRateRequest() = default;
};



}  // end namespaces
}
#endif  // end header latch