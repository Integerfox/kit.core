#ifndef ITC_ASYNCCANCEL_IFLASH_REQUEST_H_
#define ITC_ASYNCCANCEL_IFLASH_REQUEST_H_
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
namespace Itc {
///
namespace AsyncCancel {

/// Defines a 'flash pattern'
struct FlashPattern_T
{
    uint32_t onTimeMs;
    uint32_t offTimeMs;
    uint32_t repeatCount;
};


/** This abstract class define message types and payloads for a set of ITC
    services. The request() method(s) are to be implemented by a 'service'
 */
class IFlashRequest
{
public:
    /// SAP for this API
    typedef Kit::Itc::SAP<IFlashRequest> SAP;

public:
    /** Payload for Flash Message.
        IN --> data passed to the service
        OUT--> data returned from the service
     */
    struct FlashPayload_T
    {
        /// IN: The flash pattern to execute.
        FlashPattern_T flashPattern;

        /** OUT: Pass/Fail result.  Returns true if the requested number
                 of duty cycles requested was successfully executed; else
                 false is returned (e.g. returns false when onFlashTimeMs
                 and offFlashTimeMs are both zero)
         */
        bool success;

        /// Constructor
        FlashPayload_T( FlashPattern_T flashPattern = {500, 500, 1} )
            : flashPattern( flashPattern )
            , success( false )
        {
        }
    };

    /// Message Type: Flash
    typedef Kit::Itc::RequestMessage<IFlashRequest, FlashPayload_T> FlashMsg;

    /// ITC Request: Flash
    virtual void request( FlashMsg& msg ) noexcept = 0;

public:
    /** Payload for Flash CANCEL Message.
        IN --> data passed to the service
        OUT--> data returned from the service
     */
    struct FlashCancelPayload_T
    {
        /// IN: Flash Request message to cancel
        FlashMsg* flashMsgToCancel;

        /** OUT: Returns true the message was found and returned; else
                 false is returned
         */
        bool canceled;

        /// Constructor
        FlashCancelPayload_T( FlashMsg* flashMsgToCancel = nullptr )
            : flashMsgToCancel( flashMsgToCancel )
            , canceled( false )
        {
        }
    };

    /// Message Type: Flash Cancel
    typedef Kit::Itc::RequestMessage<IFlashRequest, FlashCancelPayload_T> FlashCancelMsg;

    /// ITC Request: Flash Cancel
    virtual void request( FlashCancelMsg& msg ) noexcept = 0;

public:
    ///
    virtual ~IFlashRequest() = default;
};


/** This abstract class define message types and payloads for a set of ITC
    services. The response() method are to be implemented by a 'client'
 */
class IFlashResponse
{
public:
    /// Response Message Type: Flash
    typedef Kit::Itc::ResponseMessage<IFlashResponse,
                                      IFlashRequest,
                                      IFlashRequest::FlashPayload_T>
        FlashMsg;

    /// Response: FlashMsg
    virtual void response( FlashMsg& msg ) noexcept = 0;


public:
    /// Response Message Type: Flash Cancel
    typedef Kit::Itc::ResponseMessage<IFlashResponse,
                                      IFlashRequest,
                                      IFlashRequest::FlashCancelPayload_T>
        FlashCancelMsg;

    /// Response: FlashCancelMsg
    virtual void response( FlashCancelMsg& msg ) noexcept = 0;

public:
    /// Virtual destructor
    virtual ~IFlashResponse() = default;
};


}  // end namespaces
}
#endif  // end header latch