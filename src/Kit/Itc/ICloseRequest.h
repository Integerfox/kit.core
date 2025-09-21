#ifndef KIT_ITC_ICLOSE_REQUEST_h_
#define KIT_ITC_ICLOSE_REQUEST_h_
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
#include "Kit/EventQueue/Server.h"
#include <type_traits>


/// Compile time check for the EventQueue being configured 'correctly'
static_assert( std::is_base_of<Kit::EventQueue::IMsgNotification, Kit::EventQueue::IQueue>::value,
               "IQueue must inherit from IMsgNotification" );


///
namespace Kit {
///
namespace Itc {

/** This abstract class define message types and payloads for a set of ITC
    services. The request() method(s) are to be implemented by a 'server'
 */
class ICloseRequest
{
public:
    /// SAP for this API
    typedef Kit::Itc::SAP<ICloseRequest> SAP;

public:
    /// Close Payload
    struct ClosePayload_T
    {
        /// IN: Optional arguments
        void* args;

        /// OUT: Pass/Fail result of the close request
        bool success;

        /// Constructor
        ClosePayload_T( void* args = nullptr )
            : args( args )
            , success( true )
        {
        }
    };


    /// Message Type: Close
    typedef RequestMessage<ICloseRequest, ClosePayload_T> CloseMsg;

public:
    /// ITC Request: Close
    virtual void request( CloseMsg& msg ) noexcept = 0;

public:
    ///
    virtual ~ICloseRequest() = default;
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
                            ICloseRequest,
                            ICloseRequest::OpenPayload>
        CloseMsg;


public:
    /// Response: CloseMsg
    virtual void response( CloseMsg& msg ) = 0;


public:
    /// Virtual destructor
    virtual ~OpenResponse() {}
};
#endif

}  // end namespaces
}
#endif  // end header latch
