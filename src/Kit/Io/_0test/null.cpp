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
#include "Kit/Io/LineWriter.h"
#include "Kit/Io/LineReader.h"
#include "Kit/Text/FString.h"
#include "Kit/Io/TeeOutput.h"
#include "Kit/Io/Null.h"
#include <string.h>
#include <inttypes.h>


#define SECT_     "_0test"

/// 
using namespace Kit::Io;


////////////////////////////////////////////////////////////////////////////////
/// Use anonymous namespace to make my class local-to-the-file in scope
namespace {

class MyContext
{
public:
    int m_count;

public:
    MyContext():m_count( 0 ) {}

public:
    bool testOutputs( IOutput& fd )
    {
        m_count++;
        return fd.write( "Hello" );
    }
};
}; // end namespace

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "null" )
{
    KIT_SYSTEM_TRACE_FUNC( SECT_ );
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();

    //     
    Null fd;
    char dummyChar = 29;

    REQUIRE( fd.read( dummyChar ) == false );
    REQUIRE( dummyChar == 29 );

    Kit::Text::FString<10> buffer( "bob" );
    REQUIRE( fd.read( buffer ) == false );
    REQUIRE( buffer == "" );

    int myBuffer[10] = { 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29 };
    int bytesRead    = 1;
    REQUIRE( fd.read( myBuffer, sizeof( myBuffer ), bytesRead ) == false );
    REQUIRE( bytesRead == 0 );
    REQUIRE( fd.available() == false );

    //
    int bytesWritten;
    REQUIRE( fd.write( 'a' ) );
    REQUIRE( fd.write( "bob's your uncle" ) );
    REQUIRE( fd.write( buffer ) );
    REQUIRE( fd.write( buffer, "Hello %s", "World" ) );
    REQUIRE( buffer == "Hello Worl" );
    REQUIRE( fd.write( myBuffer, sizeof( myBuffer ) ) );
    REQUIRE( fd.write( myBuffer, sizeof( myBuffer ), bytesWritten ) );
    REQUIRE( (size_t) bytesWritten == sizeof( myBuffer ) );
    fd.flush();
    fd.close();
    REQUIRE( fd.write( buffer ) == false ); // Fails because close() was called!


    // Ensure the diamond inheritance/Container Item stuff works
    Null apple;
    Null orange;
    Null cherry;

    KIT_SYSTEM_TRACE_MSG( SECT_, "apple=%p,  inPtr=%p, outPtr=%p, inOutPtr=%p", &apple, (IInput*) &apple, (IOutput*) &apple, (IInputOutput*) &apple) ;
    KIT_SYSTEM_TRACE_MSG( SECT_, "orange=%p, inPtr=%p, outPtr=%p, inOutPtr=%p", &orange, (IInput*) &orange, (IOutput*) &orange, (IInputOutput*) &orange) ;
    KIT_SYSTEM_TRACE_MSG( SECT_, "cherry=%p, inPtr=%p, outPtr=%p, inOutPtr=%p", &cherry, (IInput*) &cherry, (IOutput*) &cherry, (IInputOutput*) &cherry) ;


    // I don't have a useful concrete streams to test the following -->but I can at least make sure they compile & link
    Null fd2;
    LineWriter writer( fd2 );
    REQUIRE( writer.println( "Hello World" ) );
    LineReader reader( fd2 );
    REQUIRE( reader.readln( buffer ) == false );

    // I don't have a useful concrete streams to test the following -->but I can at least make sure they compile & link
    TeeOutput many( apple, cherry );
    REQUIRE( many.write( "[Hello (apple, cherry)]" ) );
    many.add( orange );
    REQUIRE( many.write( "[World! (apple, cherry, orange)]" ) );
    REQUIRE( many.remove( apple ) );
    REQUIRE( many.write( "[Goodbye! (cherry, orange)]" ) );
    REQUIRE( many.remove( apple ) == false );
    REQUIRE( many.remove( orange ) );
    REQUIRE( many.remove( cherry ) );
    REQUIRE( many.remove( fd ) == false );
    REQUIRE( many.write( "[One more try! (none)]" ) );
    many.close();
    REQUIRE( many.write( "[Should fail!]" ) == false );


    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}

TEST_CASE( "close" )
{
    KIT_SYSTEM_TRACE_FUNC( SECT_ );
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();

    //     
    Null fd;
    char dummyChar = 29;

    fd.close();
    REQUIRE( fd.read( dummyChar ) == false );
    REQUIRE( dummyChar == 29 );

    //
    REQUIRE( fd.write( 'a' ) == false );

    // I don't have a useful concrete streams to test the following -->but I can at least make sure they compile & link
    Null fd2;
    fd2.close();
    LineWriter writer( fd2 );
    REQUIRE( writer.println( "Hello World" ) == false );
    LineReader reader( fd2 );
    Kit::Text::FString<10> buffer( "bob" );
    REQUIRE( reader.readln( buffer ) == false );


    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}

TEST_CASE( "TeeOutput" )
{
    KIT_SYSTEM_TRACE_FUNC( SECT_ );
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();

    SECTION( "basic" )
    {
        // I don't have a useful concrete streams to test the following -->but I can at least make sure they compile & link
        Null apple;
        Null cherry;
        Null orange;
        apple.close();
        cherry.close();
        TeeOutput many( apple, cherry );
        many.add( orange );
        REQUIRE( many.write( "[World! (apple, cherry, orange)]" ) == false );
        REQUIRE( many.firstFailed() == &apple );
        REQUIRE( many.nextFailed( apple ) == &cherry );
        REQUIRE( many.remove( apple ) );
        REQUIRE( many.write( "[Goodbye! (cherry, orange)]" ) == true );
        REQUIRE( many.remove( cherry ) );
        REQUIRE( many.firstFailed() == 0 );
        REQUIRE( many.write( "[One more try! (none)]" ) == true );
        many.close();
        REQUIRE( many.write( "[Should fail!]" ) == false );
    }

    SECTION( "basic2" )
    {
        // I don't have a useful concrete streams to test the following -->but I can at least make sure they compile & link
        Null apple;
        Null cherry;
        Null orange;
        apple.close();
        cherry.close();
        TeeOutput many( apple, cherry );
        many.add( orange );
        REQUIRE( many.write( "[World! (apple, cherry, orange)]" ) == false );
        many.flush();
        many.close();
        REQUIRE( many.write( "[Should fail!]" ) == false );
    }

    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}
