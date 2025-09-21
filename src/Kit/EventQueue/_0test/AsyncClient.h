#ifndef TEST_ITC_ASYNCCLIENT_H_
#define TEST_ITC_ASYNCCLIENT_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Itc/_0test/ITestRequest.h"
#include "Kit/Itc/OpenCloseSync.h"
#include "Kit/System/ISignable.h"
#include "Kit/System/Trace.h"
#include "Kit/System/Semaphore.h"

/// Example of Asynchronous ITC client
class AsyncClient : public Kit::Itc::OpenCloseSync, public ITestResponse
{
public:
    static constexpr const char* MYSECT = "_0test";
    ITestRequest::SAP&           m_serverSAP;
    ITestRequest::ReadData_T     m_readPayload;
    ITestResponse::ReadMsg       m_readResponseMsg;
    ITestRequest::WriteData_T    m_writePayload;
    ITestResponse::WriteMsg      m_writeResponseMsg;
    Kit::System::ISignable&      m_mainThread;
    uint8_t                      m_data;
    bool                         m_opened;
    bool                         m_testAbandon;
    bool                         m_readRspReceived;
    bool                         m_writeRspReceived;

public:
    /// Constructor
    AsyncClient( Kit::EventQueue::IQueue& clientsEventQueue,
                 ITestRequest::SAP&       serverSAP,
                 Kit::System::ISignable&  testCaseThread,
                 bool                     testAbandon = false ) noexcept
        : Kit::Itc::OpenCloseSync( clientsEventQueue )
        , m_serverSAP( serverSAP )
        , m_readPayload()
        , m_readResponseMsg( *this, clientsEventQueue, serverSAP, m_readPayload )
        , m_writePayload( 0 )
        , m_writeResponseMsg( *this, clientsEventQueue, serverSAP, m_writePayload )
        , m_mainThread( testCaseThread )
        , m_data( 0 )
        , m_opened( false )
        , m_testAbandon( testAbandon )
        , m_readRspReceived( false )
        , m_writeRspReceived( false )
    {
    }

    /// Destructor
    ~AsyncClient() noexcept
    {
        close();
        m_readResponseMsg.abandon();
        m_writeResponseMsg.abandon();
    }

public:
    void request( OpenMsg& msg ) noexcept
    {
        KIT_SYSTEM_TRACE_FUNC( MYSECT );

        if ( !m_opened )
        {
            m_opened                 = true;
            msg.getPayload().success = true;
            m_data                   = (uint8_t)( (size_t)( msg.getPayload().args ) );

            m_writeResponseMsg.getRequestMsg().getPayload().srcData = m_data;
            m_serverSAP.post( m_writeResponseMsg.getRequestMsg() );
        }

        msg.returnToSender();
    }

    void request( CloseMsg& msg ) noexcept
    {
        KIT_SYSTEM_TRACE_FUNC( MYSECT );

        if ( m_opened )
        {
            m_opened                 = false;
            msg.getPayload().success = true;
        }

        msg.returnToSender();
    }

public:
    /// ReadData Response
    void response( ReadMsg& msg ) noexcept
    {
        auto& payload = msg.getPayload();
        m_data        = payload.dstData;
        KIT_SYSTEM_TRACE_MSG( MYSECT, "RSP: Read: dstByte=%u, success=%d", payload.dstData, payload.success );
        m_readRspReceived = true;
        m_mainThread.signal();  // Wake up the test case
    }

    /// Write Response
    void response( WriteMsg& msg ) noexcept
    {
        auto& payload = msg.getPayload();
        KIT_SYSTEM_TRACE_MSG( MYSECT, "RSP: Write: srcByte=%u, success=%d", payload.srcData, payload.success );
        m_writeRspReceived = true;
        m_mainThread.signal();  // Wake up the test case

        if ( m_testAbandon )
        {
            // Abandon the read response message
            m_readResponseMsg.abandon();
        }

        // Issue a read request
        m_serverSAP.post( m_readResponseMsg.getRequestMsg() );
    }
};


#endif  // end header latch