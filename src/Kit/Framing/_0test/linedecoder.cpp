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
#include "Kit/Framing/StreamSource.h"
#include "Kit/Io/Ram/InputOutputAllocate.h"
#include "Kit/Framing/LineDecoderAllocate.h"
#include "Kit/System/Trace.h"
#include <string.h>

#define SECT_ "_0test"

///
using namespace Kit::System;
using namespace Kit::Framing;
using namespace Kit::Type;

#define TEST_FRAME         "Hello Kitty!"
#define TEST_IN_TEXT       "Hello\tKitty!"
#define TEST_OUT_FRAME     "Hello*Kitty!"
#define TEST_NON_PRINTABLE "Hello\1Kitty!"

////////////////////////////////////
TEST_CASE( "LineDecoder" )
{
    ShutdownUnitTesting::clearAndUseCounter();
    Kit::Io::Ram::InputOutputAllocate<64> src;  // Must be large enough to hold the test string TWICE
    StreamSource                          decoderSrc( src );
    constexpr SSize_T                     MAX_FRAME_SIZE = 64;
    uint8_t                               frame[MAX_FRAME_SIZE];
    SSize_T                               frameSize;

    SECTION( "Nominal case" )
    {
        LineDecoderAllocate<4> uut( decoderSrc );
        const char*            testString = TEST_FRAME "\n ";
        src.write( testString );
        REQUIRE( uut.scan( MAX_FRAME_SIZE, frame, frameSize ) == true );
        REQUIRE( frameSize == (SSize_T)( strlen( TEST_FRAME ) ) );
        REQUIRE( strcmp( (const char*)frame, TEST_FRAME ) == 0 );
    }

    SECTION( "Convert Tabs" )
    {
        LineDecoderAllocate<10> uut( decoderSrc, '*' );
        const char*             testString = TEST_IN_TEXT "\n ";
        src.write( testString );
        REQUIRE( uut.scan( MAX_FRAME_SIZE, frame, frameSize ) == true );
        REQUIRE( frameSize == (SSize_T)( strlen( TEST_OUT_FRAME ) ) );
        REQUIRE( strcmp( (const char*)frame, TEST_OUT_FRAME ) == 0 );
    }

    SECTION( "non-printable" )
    {
        LineDecoderAllocate<10> uut( decoderSrc, '*' );
        const char*             testString = TEST_NON_PRINTABLE "\n\b" TEST_FRAME "\n";
        src.write( testString );
        REQUIRE( uut.scan( MAX_FRAME_SIZE, frame, frameSize ) == true );
        REQUIRE( frameSize == (SSize_T)( strlen( TEST_FRAME ) ) );
        REQUIRE( strcmp( (const char*)frame, TEST_FRAME ) == 0 );
    }

    src.close();
    REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
}
