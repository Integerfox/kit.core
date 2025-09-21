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
#include "Kit/EventQueue/Server.h"
#include "Kit/System/Thread.h"

///
using namespace Kit::Itc;
using namespace Kit::System;

////////////////////////////////////////////////////////////////////////////////
// Anonymous namespace
namespace {

// Dummy service to test the OpenSync class
class MockService : public OpenCloseSync
{
public:
    bool     isOpened       = false;
    bool     resultOpenMsg  = true;
    void*    argsOpenMsg    = nullptr;
    bool     resultCloseMsg = true;
    void*    argsCloseMsg   = nullptr;

    MockService( Kit::EventQueue::IQueue& myEventQueue ) noexcept
        : OpenCloseSync( myEventQueue )
        , isOpened( false )
    {
    }

public:
    /// See Kit::Itc::IOpenRequest
    void request( OpenMsg& msg ) noexcept override
    {
        msg.getPayload().success = resultOpenMsg;
        argsOpenMsg              = msg.getPayload().args;
        isOpened                 = true;
        msg.returnToSender();
    }

    /// See Kit::Itc::IOpenRequest
    void request( CloseMsg& msg ) noexcept override
    {
        msg.getPayload().success = resultCloseMsg;
        argsCloseMsg             = msg.getPayload().args;
        isOpened                 = false;
        msg.returnToSender();
    }
};


}  // end anonymous namespace


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "OpenClose" )
{
    ShutdownUnitTesting::clearAndUseCounter();

    Kit::EventQueue::Server uutEventLoop;
    MockService             uut( uutEventLoop );

    // Create test thread
    auto testThread = Kit::System::Thread::create( uutEventLoop, "TestThread" );
    REQUIRE( testThread != nullptr );

    SECTION( "happy-path" )
    {
        bool result = uut.open();
        REQUIRE( result == true );
        REQUIRE( uut.isOpened == true );
        REQUIRE( uut.argsOpenMsg == nullptr );

        result = uut.close();
        REQUIRE( result == true );
        REQUIRE( uut.isOpened == false );
        REQUIRE( uut.argsCloseMsg == nullptr );
    }

    SECTION( "failed-open/close" )
    {
        uut.resultOpenMsg  = false;
        uut.resultCloseMsg = false;
        bool result = uut.open();
        REQUIRE( result == false );

        result = uut.close();
        REQUIRE( result == false );
    }

    SECTION( "args" )
    {
        bool result = uut.open( (void*)1234 );
        REQUIRE( result == true );
        REQUIRE( uut.isOpened == true );
        REQUIRE( uut.argsOpenMsg == (void*)1234 );

        result = uut.close( (void*)12 );
        REQUIRE( result == true );
        REQUIRE( uut.isOpened == false );
        REQUIRE( uut.argsCloseMsg == (void*)12 );
    }

    // Shutdown threads
    uutEventLoop.pleaseStop();
    Kit::System::Thread::destroy( *testThread );

    REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
}
