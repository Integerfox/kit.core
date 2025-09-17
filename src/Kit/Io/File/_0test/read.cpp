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
#include "Kit/System/Trace.h"
#include "Kit/Io/LineReader.h"
#include "Kit/Io/File/Input.h"
#include "Kit/Text/FString.h"


#define SECT_ "_0test"

/** Hack to get around the fact the SCM systems will convert newline
    characters in Text Files based on the target host, i.e. on Windows my
    newline character will be 2 chars, whereas on posix/linux it will be
    a single character -->hence delta in the file length.
 */
#define WIN32_TESTINPUT_TXT_FILE_LENGTH   106
#define WIN32_TESTINPUT_TEXT_HELLO_OFFEST 0x5D
#define POSIX_TESTINPUT_TXT_FILE_LENGTH   101
#define POSIX_TESTINPUT_TEXT_HELLO_OFFEST 0x58


///
using namespace Kit::Io::File;


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "read" )
{
    KIT_SYSTEM_TRACE_FUNC( SECT_ );
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();

    KIT_SYSTEM_TRACE_MSG( SECT_, "Reading Lines..." );
    Input               fd( "testinput.txt" );
    Kit::Io::LineReader reader( fd );
    REQUIRE( fd.isOpened() );

    Kit::Text::FString<6> line;
    REQUIRE( reader.available() == true );
    reader.readln( line );
    KIT_SYSTEM_TRACE_MSG( SECT_, "line=[%s]", line.getString() );
    REQUIRE( line == "line 1" );

    REQUIRE( reader.readln( line ) );
    KIT_SYSTEM_TRACE_MSG( SECT_, "line=[%s]", line.getString() );
    REQUIRE( line == "line 2" );

    REQUIRE( reader.readln( line ) );
    KIT_SYSTEM_TRACE_MSG( SECT_, "line=[%s]", line.getString() );
    line.removeTrailingSpaces();
    REQUIRE( line.isEmpty() );

    Kit::Io::ByteCount_T len;
    REQUIRE( fd.length( len ) == true );
    REQUIRE( ( len == WIN32_TESTINPUT_TXT_FILE_LENGTH || len == POSIX_TESTINPUT_TXT_FILE_LENGTH ) );

    REQUIRE( reader.readln( line ) );
    KIT_SYSTEM_TRACE_MSG( SECT_, "line=[%s]", line.getString() );
    REQUIRE( ( line == "line 4" || line == "" ) );

    REQUIRE( reader.readln( line ) );
    KIT_SYSTEM_TRACE_MSG( SECT_, "line=[%s]", line.getString() );
    REQUIRE( ( line == "line 5" || line == "" ) );

    reader.close();
    REQUIRE( fd.isOpened() == false );
    REQUIRE( reader.readln( line ) == false );
    REQUIRE( fd.read( line ) == false );

    //
    Input fd2( "testinput.txt" );
    REQUIRE( fd2.isOpened() );
    char dummyChar = 29;
    REQUIRE( ( fd2.setAbsolutePos( WIN32_TESTINPUT_TEXT_HELLO_OFFEST ) || fd2.setAbsolutePos( POSIX_TESTINPUT_TEXT_HELLO_OFFEST ) ) );
    REQUIRE( fd2.length( len ) == true );
    REQUIRE( ( len == WIN32_TESTINPUT_TXT_FILE_LENGTH || len == POSIX_TESTINPUT_TXT_FILE_LENGTH ) );
    REQUIRE( fd2.read( dummyChar ) == true );
    REQUIRE( ( dummyChar == 'A' || dummyChar == 'o' ) );

    Kit::Text::FString<10> buffer( "bob" );
    REQUIRE( fd2.read( buffer ) == true );
    printf( "buff=%s", buffer.getString() );
    REQUIRE( ( buffer == "Hello Worl" || buffer == " World." ) );

    fd2.close();
    REQUIRE( fd2.isOpened() == false );
    REQUIRE( fd2.read( dummyChar ) == false );

    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}

TEST_CASE( "read/close" )
{
    KIT_SYSTEM_TRACE_FUNC( SECT_ );
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();

    Input fd( "testinput.txt" );
    REQUIRE( fd.isOpened() );
    fd.close();
    char dummyChar = 29;
    REQUIRE( fd.read( dummyChar ) == false );
    REQUIRE( dummyChar == 29 );

    REQUIRE( fd.available() == false );

    REQUIRE( fd.isEof() == true );
    Kit::Io::ByteCount_T pos;
    REQUIRE( fd.currentPos( pos ) == false );
    REQUIRE( fd.setAbsolutePos( 1 ) == false );
    REQUIRE( fd.setRelativePos( 1 ) == false );
    REQUIRE( fd.setToEof() == false );
    Kit::Io::ByteCount_T len = 22;
    REQUIRE( fd.length( len ) == false );

    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}
