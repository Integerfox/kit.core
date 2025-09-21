/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */


#include "Kit/Itc/OpenCloseSync.h"
#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include "catch2/catch_test_macros.hpp"
#include "ITestRequest.h"
#include "AsyncClient.h"
#include "Kit/EventQueue/Server.h"
#include "Kit/System/Thread.h"

#define SECT_ "_0test"

///
using namespace Kit::Itc;
using namespace Kit::System;

////////////////////////////////////////////////////////////////////////////////
// Anonymous namespace
namespace {

// Dummy service to test the OpenSync class
class MockService : public OpenCloseSync, public ITestRequest
{
public:
    bool    isOpened = false;
    uint8_t byteVal  = 0;
    MockService( Kit::EventQueue::Server& myEventLoop ) noexcept
        : OpenCloseSync( myEventLoop )
        , isOpened( false )
    {
    }

public:
    /// See Kit::Itc::IOpenRequest
    void request( OpenMsg& msg ) noexcept override
    {
        msg.getPayload().success = true;
        isOpened                 = true;
        msg.returnToSender();
    }

    /// See Kit::Itc::IOpenRequest
    void request( CloseMsg& msg ) noexcept override
    {
        msg.getPayload().success = true;
        isOpened                 = false;
        msg.returnToSender();
    }

    void request( ReadMsg& msg ) noexcept override
    {
 
        msg.getPayload().success = true;
        msg.getPayload().dstData = byteVal;
        KIT_SYSTEM_TRACE_MSG( SECT_, "REQ: Read: dstData=%u, success=%d",  msg.getPayload().dstData, msg.getPayload().success );
        msg.returnToSender();
    }

    virtual void request( WriteMsg& msg ) noexcept override
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "REQ: Write: srcByte=%u, success=%d",  msg.getPayload().srcData, msg.getPayload().success );

        msg.getPayload().success = true;
        byteVal                  = msg.getPayload().srcData + 1;
        msg.returnToSender();
    }
};


}  // end anonymous namespace

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Async" )
{
    ShutdownUnitTesting::clearAndUseCounter();

    Kit::EventQueue::Server serverEventLoop;
    Kit::EventQueue::Server clientEventLoop;
    MockService             uutServer( serverEventLoop );
    ITestRequest::SAP       serverSAP( uutServer, serverEventLoop );
    AsyncClient             uutClient( clientEventLoop, serverSAP, Thread::getCurrent() );

    // Create test threads
    auto serverThread = Kit::System::Thread::create( serverEventLoop, "ServerThread" );
    REQUIRE( serverThread != nullptr );
    auto clientThread = Kit::System::Thread::create( clientEventLoop, "ClientThread" );
    REQUIRE( clientThread != nullptr );

    SECTION( "happy-path" )
    {
        // Open both client and server. Server MUST be opened first due to hold the test is constructed
        bool result = uutServer.open();
        REQUIRE( result == true );
        REQUIRE( uutServer.isOpened == true );
        result = uutClient.open( (void*)42 );
        REQUIRE( result == true );
        REQUIRE( uutClient.m_opened == true );

        // Wait for the test sequence to complete
        Thread::wait();
        Thread::wait();

        REQUIRE( uutServer.byteVal == (42+1) );

        uutServer.close();
        REQUIRE( uutServer.isOpened == false );
        uutClient.close();
        REQUIRE( uutClient.m_opened == false );
    }

    // Shutdown threads
    clientEventLoop.pleaseStop();
    serverEventLoop.pleaseStop();
    Kit::System::Thread::destroy( *clientThread );
    Kit::System::Thread::destroy( *serverThread );

    REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
}
