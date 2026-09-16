/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Persistence/Record/Server.h"
#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include "catch2/catch_test_macros.hpp"
#include "Kit/EventQueue/Server.h"
#include "Kit/System/Thread.h"

#define SECT_ "_0test"

///
using namespace Kit::Persistence;
using namespace Kit::Persistence::Record;
using namespace Kit::System;

////////////////////////////////////////////////////////////////////////////////
// Anonymous namespace
namespace {

// Mock IRecord that tracks start()/stop() invocations
class MockRecord : public IRecord
{
public:
    bool     startResult = true;
    unsigned startCount  = 0;
    unsigned stopCount   = 0;

public:
    /// See Kit::Persistence::Record::IRecord
    bool start( Kit::EventQueue::IQueue& myEventQueue ) noexcept override
    {
        startCount++;
        return startResult;
    }

    /// See Kit::Persistence::Record::IRecord
    void stop() noexcept override
    {
        stopCount++;
    }

    /// See Kit::Persistence::Record::IRecord
    Size_T getSize() const noexcept override { return 0; }
};

}  // end anonymous namespace


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Server" )
{
    ShutdownUnitTesting::clearAndUseCounter();

    Kit::EventQueue::Server uutEventLoop;
    auto                    testThread = Thread::create( uutEventLoop, "TestThread" );
    REQUIRE( testThread != nullptr );

    MockRecord rec0;
    MockRecord rec1;
    MockRecord rec2;

    SECTION( "happy-path: open starts all records, close stops all records" )
    {
        IRecord* records[] = { &rec0, &rec1, &rec2 };
        Server   uut( uutEventLoop, records, sizeof( records ) / sizeof( records[0] ) );

        bool result = uut.open();
        REQUIRE( result == true );
        REQUIRE( rec0.startCount == 1 );
        REQUIRE( rec1.startCount == 1 );
        REQUIRE( rec2.startCount == 1 );
        REQUIRE( rec0.stopCount == 0 );

        // A second open() while already opened is a no-op
        result = uut.open();
        REQUIRE( result == true );
        REQUIRE( rec0.startCount == 1 );

        result = uut.close();
        REQUIRE( result == true );
        REQUIRE( rec0.stopCount == 1 );
        REQUIRE( rec1.stopCount == 1 );
        REQUIRE( rec2.stopCount == 1 );

        // A second close() while already closed is a no-op
        result = uut.close();
        REQUIRE( result == true );
        REQUIRE( rec0.stopCount == 1 );
    }

    SECTION( "close before open is a no-op and still reports success" )
    {
        IRecord* records[] = { &rec0, &rec1 };
        Server   uut( uutEventLoop, records, sizeof( records ) / sizeof( records[0] ) );

        bool result = uut.close();
        REQUIRE( result == true );
        REQUIRE( rec0.stopCount == 0 );
        REQUIRE( rec1.stopCount == 0 );
    }

    SECTION( "a failed start rolls back (stops) every record and open() reports failure" )
    {
        rec1.startResult = false;  // middle record fails to start
        IRecord* records[] = { &rec0, &rec1, &rec2 };
        Server   uut( uutEventLoop, records, sizeof( records ) / sizeof( records[0] ) );

        bool result = uut.open();
        REQUIRE( result == false );
        REQUIRE( rec0.startCount == 1 );
        REQUIRE( rec1.startCount == 1 );
        REQUIRE( rec2.startCount == 0 );  // never reached - loop stops once m_opened is false

        // Rollback: stop() is (unconditionally) invoked on every record in the list
        REQUIRE( rec0.stopCount == 1 );
        REQUIRE( rec1.stopCount == 1 );
        REQUIRE( rec2.stopCount == 1 );

        // A subsequent open() attempt can succeed once the failing record is fixed
        rec1.startResult = true;
        result            = uut.open();
        REQUIRE( result == true );
        REQUIRE( rec0.startCount == 2 );
        REQUIRE( rec1.startCount == 2 );
        REQUIRE( rec2.startCount == 1 );

        uut.close();
    }

    uutEventLoop.pleaseStop();
    Thread::destroy( *testThread );

    REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
}
