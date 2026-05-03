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
#include "Kit/Persistence/Record/Media/FileAdapter.h"
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
#define PAYLOAD           "Hello World"
#define PAYLOAD_LEN       ( strlen( PAYLOAD ) )

#define PAYLOAD_MIXED     "Hello Kit"
#define PAYLOAD_MIXED_LEN ( strlen( PAYLOAD_MIXED ) )

TEST_CASE( "FileAdapter" )
{
    KIT_SYSTEM_TRACE_SCOPE( SECT_, "FileAdapter test" );
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();

    Kit::EventQueue::Server mockEventQueue;
    FileAdapter             uut( "out.txt", 12 );
    char                    buffer[13];

    REQUIRE( uut.start( mockEventQueue ) == true );
    uut.stop();
    REQUIRE( uut.getMaxSize() == 12 );


    REQUIRE( uut.write( 0, PAYLOAD, PAYLOAD_LEN ) == true );
    REQUIRE( uut.read( 0, buffer, PAYLOAD_LEN ) == PAYLOAD_LEN );
    REQUIRE( strncmp( buffer, PAYLOAD, PAYLOAD_LEN ) == 0 );
    REQUIRE( uut.read( 6, buffer, 5 ) == 5 );
    REQUIRE( strncmp( buffer, "World", 5 ) == 0 );
    REQUIRE( uut.write( 6, "Kit", 3 ) == true );
    REQUIRE( uut.read( 0, buffer, PAYLOAD_LEN ) == PAYLOAD_LEN );
    REQUIRE( strncmp( buffer, PAYLOAD_MIXED, PAYLOAD_MIXED_LEN ) == 0 );

    // Error cases
    REQUIRE( uut.write( 0, nullptr, 0 ) == false );
    REQUIRE( uut.read( 0, nullptr, 0 ) == KIT_PERSISTENCE_SIZE_MAX );
    REQUIRE( uut.read( 0, buffer, 13 ) == KIT_PERSISTENCE_SIZE_MAX );

    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}