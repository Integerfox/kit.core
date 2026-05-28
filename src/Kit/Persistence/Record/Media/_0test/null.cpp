/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "catch2/catch_test_macros.hpp"
#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include "Kit/System/Trace.h"
#include "Kit/System/Assert.h"
#include "Kit/Persistence/Record/Media/Null.h"
#include "Kit/EventQueue/Server.h"

#define SECT_ "_0test"

///
using namespace Kit::Persistence::Record::Media;
using namespace Kit::Persistence::Record;
using namespace Kit::Persistence;


////////////////////////////////////////////////////////////////////////////////
namespace {

};  // end anonymous namespace


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Null" )
{
    KIT_SYSTEM_TRACE_SCOPE( SECT_, "Null test" );
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();

    Kit::EventQueue::Server mockEventQueue;
    Null                    uut( 12 );

    REQUIRE( uut.start( mockEventQueue ) == true );
    uut.stop();
    REQUIRE( uut.getMaxSize() == 12 );

    REQUIRE( uut.write( 0, nullptr, 0 ) == false );
    REQUIRE( uut.write( 0, "data", 4 ) == true );

    char buffer[5] = { 0 };
    REQUIRE( uut.read( 0, nullptr, 0 ) == KIT_PERSISTENCE_SIZE_MAX );
    REQUIRE( uut.read( 0, buffer, sizeof( buffer ) ) == sizeof( buffer ) );
    REQUIRE( uut.read( 0, buffer, 13 ) == KIT_PERSISTENCE_SIZE_MAX );

    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}