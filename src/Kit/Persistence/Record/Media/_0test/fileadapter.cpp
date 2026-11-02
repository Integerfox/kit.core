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
#include "Kit/Io/File/System.h"
#include <string.h>

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

#define ERASED_FILE_NAME "erased.bin"
#define ERASED_MAX_LEN   40  // Larger than 2x FileAdapter's internal pad buffer

static bool isErased( const uint8_t* buffer, size_t len )
{
    for ( size_t i = 0; i < len; i++ )
    {
        if ( buffer[i] != 0xFF )
        {
            return false;
        }
    }
    return true;
}

TEST_CASE( "FileAdapter emulates erased media" )
{
    KIT_SYSTEM_TRACE_SCOPE( SECT_, "FileAdapter erased media test" );
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();

    FileAdapter uut( ERASED_FILE_NAME, ERASED_MAX_LEN );
    uint8_t     buffer[ERASED_MAX_LEN];
    Kit::Io::File::System::remove( ERASED_FILE_NAME );

    // No file -->entire media reads as erased
    memset( buffer, 0, sizeof( buffer ) );
    REQUIRE( uut.read( 0, buffer, ERASED_MAX_LEN ) == ERASED_MAX_LEN );
    REQUIRE( isErased( buffer, ERASED_MAX_LEN ) );
    REQUIRE( Kit::Io::File::System::exists( ERASED_FILE_NAME ) == false );

    // Write past the physical end of the file -->the gap is erased
    REQUIRE( uut.write( 4, "AB", 2 ) == true );
    memset( buffer, 0, sizeof( buffer ) );
    REQUIRE( uut.read( 0, buffer, ERASED_MAX_LEN ) == ERASED_MAX_LEN );
    REQUIRE( isErased( buffer, 4 ) );
    REQUIRE( buffer[4] == 'A' );
    REQUIRE( buffer[5] == 'B' );
    REQUIRE( isErased( buffer + 6, ERASED_MAX_LEN - 6 ) );

    // Read that starts past the physical end of the file
    memset( buffer, 0, sizeof( buffer ) );
    REQUIRE( uut.read( 8, buffer, 4 ) == 4 );
    REQUIRE( isErased( buffer, 4 ) );

    // Gap that spans multiple internal pad buffers
    REQUIRE( uut.write( ERASED_MAX_LEN - 1, "Z", 1 ) == true );
    memset( buffer, 0, sizeof( buffer ) );
    REQUIRE( uut.read( 0, buffer, ERASED_MAX_LEN ) == ERASED_MAX_LEN );
    REQUIRE( buffer[4] == 'A' );
    REQUIRE( buffer[5] == 'B' );
    REQUIRE( isErased( buffer + 6, ERASED_MAX_LEN - 7 ) );
    REQUIRE( buffer[ERASED_MAX_LEN - 1] == 'Z' );

    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}