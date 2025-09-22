#ifndef TEST_ITC_ITEST_REQUEST_H_
#define TEST_ITC_ITEST_REQUEST_H_
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
#include <stdint.h>

/** This abstract class define message types and payloads for a set of ITC
    services. The request() method(s) are to be implemented by a 'service'
 */
class ITestRequest
{
public:
    /// SAP for this API
    typedef Kit::Itc::SAP<ITestRequest> SAP;

public:
    /** Payload for Message.
        IN --> data passed to the service
        OUT--> data returned from the service
     */
    struct ReadData_T
    {
        /// OUT: Requested byte
        uint8_t dstData;

        /// OUT: Number of bytes written
        bool success;

        /// Constructor
        ReadData_T()
            : dstData( 0 )
            , success( false )
        {
        }
    };


    /// Message Type:
    typedef Kit::Itc::RequestMessage<ITestRequest, ReadData_T> ReadMsg;


public:
    /** Payload for Message.
        IN --> data passed to the service
        OUT--> data returned from the service
     */
    struct WriteData_T
    {
        /// IN: Byte to write
        uint8_t srcData;

        /// OUT: Number of bytes written
        bool success;

        /// Constructor
        WriteData_T( uint8_t dataToWrite )
            : srcData( dataToWrite )
            , success( false )
        {
        }
    };


    /// Message Type:
    typedef Kit::Itc::RequestMessage<ITestRequest, WriteData_T> WriteMsg;


public:
    /// ITC Request: Read
    virtual void request( ReadMsg& msg ) noexcept = 0;

    /// ITC Request: Write
    virtual void request( WriteMsg& msg ) noexcept = 0;

public:
    ///
    virtual ~ITestRequest() = default;
};

///////////////////////////////////////////////////////////////////////////////
/** This abstract class define response message types for a set of ITC services.
    The response() method(s) are to be implemented by the 'client'
 */
class ITestResponse
{
public:
    /// Response Message Type: Read
    typedef Kit::Itc::ResponseMessage<ITestResponse,
                                      ITestRequest,
                                      ITestRequest::ReadData_T>
        ReadMsg;

    /// Response Message Type: Write
    typedef Kit::Itc::ResponseMessage<ITestResponse,
                                      ITestRequest,
                                      ITestRequest::WriteData_T>
        WriteMsg;

public:
    /// Response: ReadMsg
    virtual void response( ReadMsg& msg ) noexcept = 0;

    /// Response: WriteMsg
    virtual void response( WriteMsg& msg ) noexcept = 0;

public:
    /// Virtual destructor
    virtual ~ITestResponse() = default;
};

#endif  // end header latch