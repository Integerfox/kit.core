/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */


#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include "catch2/catch_test_macros.hpp"
#include "Kit/Itc/OpenSync.h"
#include "Kit/EventQueue/Server.h"
#include "Kit/System/Thread.h"

    ///
    using namespace Kit::Itc;
using namespace Kit::System;

////////////////////////////////////////////////////////////////////////////////
// Anonymous namespace
namespace {

// Dummy service to test the OpenSync class
class MockService : public OpenSync
{
public:
    bool     isOpened      = false;
    bool     resultOpenMsg = true;
    unsigned countOpenMsg  = 0;
    void*    argsOpenMsg   = nullptr;

    MockService( Kit::EventQueue::Server& myEventLoop ) noexcept
        : OpenSync( myEventLoop )
        , isOpened( false )
    {
    }

public:
    /// See Kit::Itc::IOpenRequest
    void request( OpenMsg& msg ) noexcept override
    {
        countOpenMsg++;
        msg.getPayload().success = resultOpenMsg;
        argsOpenMsg              = msg.getPayload().args;
        isOpened                 = true;
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
        REQUIRE( uut.countOpenMsg == 1u );
        REQUIRE( uut.argsOpenMsg == nullptr );
    }

    // Shutdown threads
    uutEventLoop.pleaseStop();
    Kit::System::Thread::destroy( *testThread );

    REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
}
