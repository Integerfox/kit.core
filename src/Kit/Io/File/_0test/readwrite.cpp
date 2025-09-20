/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Io/Types.h"
#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include "catch2/catch_test_macros.hpp"
#include "Kit/System/Trace.h"
#include "Kit/Io/LineReader.h"
#include "Kit/Io/LineWriter.h"
#include "Kit/Io/File/InputOutput.h"
#include "Kit/Text/FString.h"



#define SECT_     "_0test"

/// 
using namespace Kit::Io::File;



////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "readwrite" )
{
    KIT_SYSTEM_TRACE_FUNC( SECT_ );
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();

    //
    Kit::Text::FString<256> sum;
    Kit::Text::FString<10>  buffer( "bob" );
    char                    myBuffer[10] = { 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29 };
    int                     bytesWritten;
    InputOutput             fd( "output3.txt", true, true );
    REQUIRE( fd.isOpened() );
    REQUIRE( fd.write( 'a' ) );
    sum = 'a';
    REQUIRE( fd.write( "bob's your uncle" ) );
    sum += "bob's your uncle";
    REQUIRE( fd.write( buffer ) );
    sum += buffer;
    REQUIRE( fd.write( buffer, "Hello %s", "World" ) );
    REQUIRE( buffer == "Hello Worl" );
    sum += buffer;
    REQUIRE( fd.write( myBuffer, sizeof( myBuffer ) ) );
    for ( size_t i=0; i < sizeof( myBuffer ); i++ ) { sum += myBuffer[i]; }
    REQUIRE( fd.write( myBuffer, sizeof( myBuffer ), bytesWritten ) );
    REQUIRE( (size_t) bytesWritten == sizeof( myBuffer ) );
    for ( int i=0; i < bytesWritten; i++ ) { sum += myBuffer[i]; }
    fd.flush();

    REQUIRE( fd.isOpened() );
    Kit::Text::FString<256> inbuffer;
    REQUIRE( fd.setAbsolutePos( 0 ) );
    fd.read( inbuffer );
    REQUIRE( inbuffer == sum );
    REQUIRE( fd.read( inbuffer ) == false ); // Ensure EOF is hit
    REQUIRE( fd.isEof() );
    fd.close();
    REQUIRE( fd.isOpened() == false );
    REQUIRE( fd.write( 'a' ) == false );
    char dummy;
    REQUIRE( fd.read( dummy ) == false );
    REQUIRE( fd.isEof() );
    REQUIRE( fd.isOpened() == false );

    //
    InputOutput fd2( "output4.txt", true, true );
    Kit::Io::LineWriter writer( fd2 );
    REQUIRE( writer.println() );
    REQUIRE( writer.println( "Hello World" ) );
    REQUIRE( writer.print( "Hello" ) );
    REQUIRE( writer.print( "World" ) );
    REQUIRE( writer.print( " again!" ) );
    REQUIRE( writer.println() );

    REQUIRE( fd2.isOpened() );
    REQUIRE( fd2.setAbsolutePos( 0 ) );
    Kit::Io::LineReader reader( fd2 );
    REQUIRE( reader.readln( inbuffer ) );
    REQUIRE( inbuffer.isEmpty() );
    REQUIRE( reader.readln( inbuffer ) );
    REQUIRE( inbuffer == "Hello World" );
    REQUIRE( reader.readln( inbuffer ) );
    REQUIRE( inbuffer == "HelloWorld again!" );
    REQUIRE( reader.readln( inbuffer ) == false );
    REQUIRE( fd2.isEof() );
    reader.close();
    REQUIRE( fd2.isOpened() == false );
    writer.close();
    REQUIRE( writer.println() == false );
    REQUIRE( fd2.write( 'a' ) == false );

    //
    InputOutput fd3( "output3.txt", false, false );
    inbuffer.clear();
    REQUIRE( fd3.isOpened() );
    REQUIRE( fd3.setAbsolutePos( 15 ) );
    Kit::Io::ByteCount_T pos;
    REQUIRE( fd3.currentPos(pos));
    REQUIRE( pos == 15 );
    REQUIRE( fd3.setRelativePos( -14 ) );
    REQUIRE( fd3.currentPos(pos));
    REQUIRE( pos == 1 );
    REQUIRE( fd3.read( inbuffer, 3) );
    REQUIRE( inbuffer == "bob" );
    Kit::Io::ByteCount_T len;
    REQUIRE( fd3.length(len) );
    REQUIRE( len == sum.length());
    fd3.close();

    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}
