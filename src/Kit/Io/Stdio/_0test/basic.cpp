/*-----------------------------------------------------------------------------
 * This file is part of the Colony.Core Project.  The Colony.Core Project is an
 * open source project with a BSD type of licensing agreement.  See the license
 * agreement (license.txt) in the top/ directory or on the Internet at
 * http://integerfox.com/colony.core/license.txt
 *
 * Copyright (c) 2014-2025  John T. Taylor
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/

#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include "catch2/catch_test_macros.hpp"
#include "Kit/System/Trace.h"
#include "Kit/Io/Stdio/StdIn.h"
#include "Kit/Io/Stdio/StdOut.h"
#include "Kit/Io/LineWriter.h"
#include "Kit/Io/LineReader.h"
#include "Kit/Text/FString.h"
#include "Kit/Io/TeeOutput.h"
#include <string.h>
#include <inttypes.h>


#define SECT_ "_0test"

///
using namespace Kit::Io::Stdio;


/////////////////////////////////////////////////
TEST_CASE( "basic" )
{
    KIT_SYSTEM_TRACE_FUNC( SECT_ );
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();

    KIT_SYSTEM_TRACE_MSG( SECT_, "Reading Lines..." );
    StdIn                 in2fd;
    Kit::Io::LineReader   reader( in2fd );
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
    REQUIRE( reader.readln( line ) );
    KIT_SYSTEM_TRACE_MSG( SECT_, "line=[%s]", line.getString() );
    REQUIRE( line == "line 4" );
    REQUIRE( reader.readln( line ) );
    KIT_SYSTEM_TRACE_MSG( SECT_, "line=[%s]", line.getString() );
    REQUIRE( line == "line 5" );
    reader.close();
    REQUIRE( reader.readln( line ) == false );
    REQUIRE( in2fd.read( line ) == false );

    //
    StdIn infd;
    char  dummyChar = 29;

    REQUIRE( infd.read( dummyChar ) == true );
    REQUIRE( dummyChar == 'A' );

    Kit::Text::FString<10> buffer( "bob" );
    REQUIRE( infd.read( buffer ) == true );
    REQUIRE( buffer == "Hello Worl" );

    char myBuffer[10] = { 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29 };
    int  bytesRead    = 1;
    REQUIRE( infd.available() == true );
    REQUIRE( infd.read( myBuffer, 0, bytesRead ) == true );
    REQUIRE( bytesRead == 0 );
    infd.read( myBuffer, sizeof( myBuffer ), bytesRead );
    REQUIRE( bytesRead == 2 );
    REQUIRE( myBuffer[0] == 'd' );
    REQUIRE( myBuffer[1] == '.' );
    bool result = infd.read( myBuffer, sizeof( myBuffer ), bytesRead );
    KIT_SYSTEM_TRACE_MSG( SECT_, "result=%d, bytesRead=%d, myBuffer=[%.*s]", result, bytesRead, bytesRead, myBuffer );
    REQUIRE( result == false );

    infd.close();
    REQUIRE( infd.read( dummyChar ) == false );


    //
    StdOut outfd;
    int    bytesWritten;
    REQUIRE( outfd.write( myBuffer, 0, bytesWritten ) == true );
    REQUIRE( bytesWritten == 0 );
    REQUIRE( outfd.write( 'a' ) );
    REQUIRE( outfd.write( "bob's your uncle" ) );
    REQUIRE( outfd.write( buffer ) );
    REQUIRE( outfd.write( buffer, "Hello %s", "World" ) );
    REQUIRE( buffer == "Hello Worl" );
    REQUIRE( outfd.write( myBuffer, sizeof( myBuffer ) ) );
    REQUIRE( outfd.write( myBuffer, sizeof( myBuffer ), bytesWritten ) );
    REQUIRE( (size_t)bytesWritten == sizeof( myBuffer ) );

    outfd.flush();
    outfd.close();
    REQUIRE( outfd.write( 'a' ) == false); 

    //
    StdOut out2fd;
    REQUIRE( out2fd.write( myBuffer, 0, bytesWritten ) == true );
    REQUIRE( bytesWritten == 0 );
    Kit::Io::LineWriter writer( out2fd );
    REQUIRE( writer.println() );
    REQUIRE( writer.println( "Hello World" ) );
    REQUIRE( writer.print( "Hello" ) );
    REQUIRE( writer.print( "World" ) );
    REQUIRE( writer.print( " again!" ) );
    REQUIRE( writer.println() );
    writer.flush();
    writer.close();
    REQUIRE( writer.println() == false );
    REQUIRE( out2fd.write( 'a' ) == false );

    //
    StdOut             out3fd;
    StdOut             out4fd;
    StdOut             out5fd;
    Kit::Io::TeeOutput outputs( out3fd );

    REQUIRE( outputs.write( "[Hello World (fd3)]\n" ) );
    outputs.add( out4fd );
    REQUIRE( outputs.write( "[Hello World - Again (fd3,fd4)]\n" ) );
    outputs.add( out5fd );
    REQUIRE( outputs.write( "[Hello World - Again - Again! (fd3,fd4,fd5)]\n" ) );
    REQUIRE( outputs.remove( out5fd ) );
    REQUIRE( outputs.write( "[Hello World - Again^2 (fd3,fd4)]\n" ) );
    outputs.close();
    REQUIRE( outputs.write( "[All Should fail]\n" ) == false );
    REQUIRE( outputs.remove( out3fd ) );
    REQUIRE( outputs.write( "[Hello World - Again]\n" ) == false );
    REQUIRE( outputs.remove( out4fd ) );
    REQUIRE( outputs.write( "[Hello World]\n" ) == false );
    REQUIRE( outputs.remove( out5fd ) == false );
    out5fd.close();
    REQUIRE( out5fd.write( "should fail because closed\n" ) == false );
    REQUIRE( out4fd.write( "should fail because closed\n" ) == false );
    REQUIRE( out3fd.write( "should fail because closed\n" ) == false );


    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}

TEST_CASE( "close" )
{
    KIT_SYSTEM_TRACE_FUNC( SECT_ );
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();

    StdIn fd;
    char  dummyChar = 29;

    fd.close();
    REQUIRE( fd.read( dummyChar ) == false );
    REQUIRE( dummyChar == 29 );

    StdOut fd2;
    fd2.close();
    REQUIRE( fd2.write( 'a' ) == false );

    StdOut fd3;
    fd3.close();
    REQUIRE( fd3.write( 'a' ) == false );

    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}
