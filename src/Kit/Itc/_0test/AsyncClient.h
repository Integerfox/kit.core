#ifndef TEST_ITC_ASYNCCLIENT_H_
#define TEST_ITC_ASYNCCLIENT_H_
/*-----------------------------------------------------------------------------
@copyright Copyright (C) 2024 Advanced Micro Devices, Inc. All rights reserved.
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
    ;
    bool m_opened;

public:
    /// Constructor
    AsyncClient( Kit::EventQueue::Server& clientsEventLoop,
                 ITestRequest::SAP&       serverSAP,
                 Kit::System::ISignable&  testCaseThread ) noexcept
        : Kit::Itc::OpenCloseSync( clientsEventLoop )
        , m_serverSAP( serverSAP )
        , m_readPayload()
        , m_readResponseMsg( *this, clientsEventLoop, serverSAP, m_readPayload )
        , m_writePayload( 0 )
        , m_writeResponseMsg( *this, clientsEventLoop, serverSAP, m_writePayload )
        , m_mainThread( testCaseThread )
        , m_data( 0 )
        , m_opened( false )
    {
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
        m_mainThread.signal();  // Wake up the test case
    }

    /// Write Response
    void
    response( WriteMsg& msg ) noexcept
    {
        auto& payload = msg.getPayload();
        KIT_SYSTEM_TRACE_MSG( MYSECT, "RSP: Write: srcByte=%u, success=%d", payload.srcData, payload.success );
        m_mainThread.signal();  // Wake up the test case

        // Issue a read request
        m_serverSAP.post( m_readResponseMsg.getRequestMsg() );
    }
};


#endif  // end header latch