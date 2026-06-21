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
#include "Kit/Framing/EncoderWriter.h"
#include "Kit/System/Trace.h"
#include <string.h>

#define SECT_ "_0test"

///
using namespace Kit::System;
using namespace Kit::Framing;

namespace {

class MyDestination : public IDestination
{
public:
    char   m_outputBuffer[100];
    size_t m_bytesWritten;
    size_t m_nextByteIndex;
    bool   m_resultStartOutput;
    bool   m_resultAppendOutput;
    bool   m_resultEndOutput;
public:
    MyDestination() noexcept
        : m_bytesWritten( 0 )
        , m_nextByteIndex( 0 )
        , m_resultStartOutput( true )
        , m_resultAppendOutput( true )
        , m_resultEndOutput( true )
    {
    }

protected:
    bool startOutput() noexcept
    {
        m_nextByteIndex = 0;
        return m_resultStartOutput;
    }

    bool appendOutput( const void* srcBuffer, Kit::Type::SSize_T numBytes ) noexcept
    {
        const uint8_t* buffer = static_cast<const uint8_t*>(srcBuffer);
        for ( Kit::Type::SSize_T i = 0; i < numBytes; ++i )
        {
            m_outputBuffer[m_nextByteIndex] = buffer[i];
            m_nextByteIndex++;
        }
        return m_resultAppendOutput;
    }

    bool endOutput() noexcept
    {
        m_outputBuffer[m_nextByteIndex] = '\0';
        KIT_SYSTEM_TRACE_MSG( SECT_, "FRAME: [%s]", m_outputBuffer );
        return m_resultEndOutput;
    }
};

}  // end anonymous namespace
////////////////////////////////////////////////////////////////////////////////

#define OUTPUT_STRING( str ) uut.output( str , strlen( str ) )

TEST_CASE( "EncoderWriter" )
{
    ShutdownUnitTesting::clearAndUseCounter();
    MyDestination dst;
    EncoderWriter uut( dst, '.', ';', '~' );

    SECTION( "Encode" )
    {
        uut.startFrame();
        OUTPUT_STRING( "abcd" );
        uut.endFrame();
        REQUIRE( strcmp( dst.m_outputBuffer, ".abcd;" ) == 0 );

        uut.startFrame();
        OUTPUT_STRING( "a");
        uut.endFrame();
        REQUIRE( strcmp( dst.m_outputBuffer, ".a;" ) == 0 );

        uut.startFrame();
        OUTPUT_STRING( "b");
        uut.endFrame();
        REQUIRE( strcmp( dst.m_outputBuffer, ".b;" ) == 0 );

        uut.startFrame();
        uut.endFrame();
        REQUIRE( strcmp( dst.m_outputBuffer, ".;" ) == 0 );

        uut.startFrame();
        OUTPUT_STRING( "a;bcd");
        uut.endFrame();
        REQUIRE( strcmp( dst.m_outputBuffer, ".a~;bcd;" ) == 0 );

        uut.startFrame();
        OUTPUT_STRING( "~" );
        uut.endFrame();
        REQUIRE( strcmp( dst.m_outputBuffer, ".~~;" ) == 0 );

        uut.startFrame();
        OUTPUT_STRING( "a.bcd" );
        uut.endFrame();
        REQUIRE( strcmp( dst.m_outputBuffer, ".a~.bcd;" ) == 0 );
    }

    SECTION( "errors")
    {
        uut.startFrame();
        OUTPUT_STRING( "abc");
        uut.startFrame();
        OUTPUT_STRING( "ZYX");
        uut.endFrame();
        REQUIRE( strcmp( dst.m_outputBuffer, ".ZYX;" ) == 0 );

        uut.startFrame();
        REQUIRE( OUTPUT_STRING( "abc") == true );
        dst.m_resultAppendOutput = false;
        REQUIRE( OUTPUT_STRING( "def") == false );
        REQUIRE( uut.endFrame() == false );

        dst.m_resultAppendOutput = true;
        uut.startFrame();
        REQUIRE( OUTPUT_STRING( "abc") == true );
        dst.m_resultEndOutput = false;
        REQUIRE( uut.endFrame() == false );

        dst.m_resultEndOutput = true;
        uut.startFrame();
        OUTPUT_STRING( "a.bcd" );
        dst.m_resultAppendOutput = false;
        REQUIRE( uut.endFrame() == false );

        dst.m_resultAppendOutput = true;
        uut.startFrame();
        dst.m_resultEndOutput = false;
        REQUIRE( uut.startFrame() == false );

        dst.m_resultEndOutput = true;
        REQUIRE( uut.startFrame() );
        REQUIRE( uut.output( nullptr, 5 ) == false );
        REQUIRE( uut.output( "abc", -1 ) == false );
        REQUIRE( uut.output( "abc", 0 ) == true );
        REQUIRE( uut.endFrame() == true );
        REQUIRE( strcmp( dst.m_outputBuffer, ".;" ) == 0 );
    }

    REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
}
