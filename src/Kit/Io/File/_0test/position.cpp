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
#include "Kit/Io/File/Input.h"
#include "Kit/Io/File/InputOutput.h"
#include "Kit/Io/File/Output.h"
#include "Kit/Text/FString.h"


#define SECT_       "_0test"

#define FILE_NAME   "output1.txt.tmp"
#define OUTPUT_TXT  "Hello world!"
#define OUTPUT_TXT2 "gd-by"
#define OUTPUT_TXT3 "gd-by world!"
#define OUTPUT_TXT4 " world!"


///
using namespace Kit::Io::File;

TEST_CASE( "iposition output" )
{
    KIT_SYSTEM_TRACE_FUNC( SECT_ );
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();

    //
    Kit::Text::FString<20> buffer( "bob" );
    Output                 fd( FILE_NAME, true, true );
    REQUIRE( fd.isOpened() );

    SECTION( "output: absolute" )
    {
        Kit::Io::ByteCount_T pos;
        REQUIRE( fd.currentPos( pos ) == true );
        REQUIRE( pos == 0 );
        REQUIRE( fd.write( OUTPUT_TXT ) );
        REQUIRE( fd.currentPos( pos ) == true );
        REQUIRE( pos == strlen( OUTPUT_TXT ) );

        fd.setAbsolutePos( 0 );
        REQUIRE( fd.write( OUTPUT_TXT2 ) );
        fd.flush();
        Kit::Io::ByteCount_T len;
        REQUIRE( fd.length( len ) == true );
        REQUIRE( len == strlen( OUTPUT_TXT ) );
        REQUIRE( fd.currentPos( pos ) == true );
        REQUIRE( pos == strlen( OUTPUT_TXT2 ) );
        REQUIRE( fd.setToEof() == true );
        REQUIRE( fd.currentPos( pos ) == true );
        REQUIRE( pos == strlen( OUTPUT_TXT ) );
        fd.close();

        Input infd( FILE_NAME );
        REQUIRE( infd.isOpened() );
        REQUIRE( infd.length( len ) == true );
        REQUIRE( len == strlen( OUTPUT_TXT ) );
        REQUIRE( infd.read( buffer ) == true );
        REQUIRE( infd.isEof() == false );
        REQUIRE( buffer == OUTPUT_TXT3 );
        REQUIRE( infd.read( buffer ) == false );
        REQUIRE( infd.isEof() == true );
        infd.close();
    }

    SECTION( "output: relative" )
    {
        Kit::Io::ByteCount_T pos;
        REQUIRE( fd.currentPos( pos ) == true );
        REQUIRE( pos == 0 );
        REQUIRE( fd.write( OUTPUT_TXT ) );
        REQUIRE( fd.currentPos( pos ) == true );
        REQUIRE( pos == strlen( OUTPUT_TXT ) );

        long newpos = strlen( OUTPUT_TXT );
        fd.setRelativePos( -newpos );
        REQUIRE( fd.write( OUTPUT_TXT2 ) );
        fd.flush();
        fd.close();

        Input infd( FILE_NAME );
        REQUIRE( infd.isOpened() );
        Kit::Io::ByteCount_T len;
        REQUIRE( infd.length( len ) == true );
        REQUIRE( len == strlen( OUTPUT_TXT ) );
        REQUIRE( infd.read( buffer ) == true );
        REQUIRE( infd.isEof() == false );
        REQUIRE( buffer == OUTPUT_TXT3 );
        REQUIRE( infd.read( buffer ) == false );
        REQUIRE( infd.isEof() == true );
        infd.close();
    }

    SECTION( "in/out: absolute" )
    {
        Kit::Io::ByteCount_T pos;
        REQUIRE( fd.currentPos( pos ) == true );
        REQUIRE( pos == 0 );
        REQUIRE( fd.write( OUTPUT_TXT ) );
        REQUIRE( fd.currentPos( pos ) == true );
        REQUIRE( pos == strlen( OUTPUT_TXT ) );

        fd.setAbsolutePos( 0 );
        REQUIRE( fd.write( OUTPUT_TXT2 ) );
        fd.flush();
        Kit::Io::ByteCount_T len;
        REQUIRE( fd.length( len ) == true );
        REQUIRE( len == strlen( OUTPUT_TXT ) );
        REQUIRE( fd.currentPos( pos ) == true );
        REQUIRE( pos == strlen( OUTPUT_TXT2 ) );
        REQUIRE( fd.setToEof() == true );
        REQUIRE( fd.currentPos( pos ) == true );
        REQUIRE( pos == strlen( OUTPUT_TXT ) );
        fd.close();

        Input infd( FILE_NAME );
        REQUIRE( infd.isOpened() );
        REQUIRE( infd.length( len ) == true );
        REQUIRE( len == strlen( OUTPUT_TXT ) );
        REQUIRE( infd.read( buffer ) == true );
        REQUIRE( infd.isEof() == false );
        REQUIRE( buffer == OUTPUT_TXT3 );
        REQUIRE( infd.read( buffer ) == false );
        REQUIRE( infd.isEof() == true );
        infd.close();
    }

    SECTION( "in/out: relative" )
    {
        Kit::Io::ByteCount_T pos;
        REQUIRE( fd.currentPos( pos ) == true );
        REQUIRE( pos == 0 );
        REQUIRE( fd.write( OUTPUT_TXT ) );
        REQUIRE( fd.currentPos( pos ) == true );
        REQUIRE( pos == strlen( OUTPUT_TXT ) );

        long newpos = strlen( OUTPUT_TXT );
        fd.setRelativePos( -newpos );
        REQUIRE( fd.write( OUTPUT_TXT2 ) );
        fd.flush();
        fd.close();

        Input infd( FILE_NAME );
        REQUIRE( infd.isOpened() );
        Kit::Io::ByteCount_T len;
        REQUIRE( infd.length( len ) == true );
        REQUIRE( len == strlen( OUTPUT_TXT ) );
        REQUIRE( infd.read( buffer ) == true );
        REQUIRE( infd.isEof() == false );
        REQUIRE( buffer == OUTPUT_TXT3 );
        REQUIRE( infd.read( buffer ) == false );
        REQUIRE( infd.isEof() == true );
        infd.close();
    }

    SECTION( "in: absolute" )
    {
        Kit::Io::ByteCount_T pos;
        REQUIRE( fd.currentPos( pos ) == true );
        REQUIRE( pos == 0 );
        REQUIRE( fd.write( OUTPUT_TXT ) );
        REQUIRE( fd.currentPos( pos ) == true );
        REQUIRE( pos == strlen( OUTPUT_TXT ) );
        fd.close();

        Input infd( FILE_NAME );
        REQUIRE( infd.setAbsolutePos( strlen( OUTPUT_TXT2 ) ) == true );
        Kit::Io::ByteCount_T curpos;
        REQUIRE( infd.currentPos( curpos ) == true );
        REQUIRE( curpos == strlen( OUTPUT_TXT2 ) );
        REQUIRE( infd.read( buffer ) == true );
        REQUIRE( infd.isEof() == false );
        REQUIRE( buffer == OUTPUT_TXT4 );
        REQUIRE( infd.read( buffer ) == false );
        REQUIRE( infd.isEof() == true );
        infd.close();
    }

    SECTION( "in: relative" )
    {
        Kit::Io::ByteCount_T pos;
        REQUIRE( fd.currentPos( pos ) == true );
        REQUIRE( pos == 0 );
        REQUIRE( fd.write( OUTPUT_TXT ) );
        REQUIRE( fd.currentPos( pos ) == true );
        REQUIRE( pos == strlen( OUTPUT_TXT ) );
        fd.close();

        Input infd( FILE_NAME );
        REQUIRE( infd.setToEof() == true );
        REQUIRE( infd.setRelativePos( (long)( strlen( OUTPUT_TXT2 ) ) - (long)( strlen( OUTPUT_TXT ) ) ) == true );
        Kit::Io::ByteCount_T curpos;
        REQUIRE( infd.currentPos( curpos ) == true );
        REQUIRE( curpos == strlen( OUTPUT_TXT2 ) );

        REQUIRE( infd.read( buffer ) == true );
        REQUIRE( infd.isEof() == false );
        REQUIRE( buffer == OUTPUT_TXT4 );
        REQUIRE( infd.read( buffer ) == false );
        REQUIRE( infd.isEof() == true );
        infd.close();
    }


    fd.close();
    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}