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

class MyUtt : public EncoderWriter
{
public:
    char   m_outputBuffer[100];
    size_t m_bytesWritten;
    size_t m_nextByteIndex;
    bool   m_resultStartOutput;
    bool   m_resultAppendOutput;
    bool   m_resultEndOutput;
public:
    MyUtt( char startOfFrame, char endOfFrame, char escapeByte, bool skipSendingSof = false ) noexcept
        : EncoderWriter( startOfFrame, endOfFrame, escapeByte, skipSendingSof )
        , m_bytesWritten( 0 )
        , m_nextByteIndex( 0 )
        , m_resultStartOutput( true )
        , m_resultAppendOutput( true )
        , m_resultEndOutput( true )
    {
    }

public:
    bool outputString( const char* srcString )
    {
        return output( srcString, strlen( srcString ) );
    }

protected:
    bool startOutput() noexcept
    {
        m_nextByteIndex = 0;
        return m_resultStartOutput;
    }

    bool appendOutput( uint8_t srcByte ) noexcept
    {
        m_outputBuffer[m_nextByteIndex] = srcByte;
        m_nextByteIndex++;
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

TEST_CASE( "EncoderWriter" )
{
    ShutdownUnitTesting::clearAndUseCounter();
    MyUtt uut( '.', ';', '~' );

    SECTION( "Encode" )
    {
        uut.startFrame();
        uut.outputString( "abcd" );
        uut.endFrame();
        REQUIRE( strcmp( uut.m_outputBuffer, ".abcd;" ) == 0 );

        uut.startFrame();
        uut.outputString( "a");
        uut.endFrame();
        REQUIRE( strcmp( uut.m_outputBuffer, ".a;" ) == 0 );

        uut.startFrame();
        uut.outputString( "b");
        uut.endFrame();
        REQUIRE( strcmp( uut.m_outputBuffer, ".b;" ) == 0 );

        uut.startFrame();
        uut.endFrame();
        REQUIRE( strcmp( uut.m_outputBuffer, ".;" ) == 0 );

        uut.startFrame();
        uut.outputString( "a;bcd");
        uut.endFrame();
        REQUIRE( strcmp( uut.m_outputBuffer, ".a~;bcd;" ) == 0 );

        uut.startFrame();
        uut.outputString( "~" );
        uut.endFrame();
        REQUIRE( strcmp( uut.m_outputBuffer, ".~~;" ) == 0 );

        uut.startFrame();
        uut.outputString( "a.bcd" );
        uut.endFrame();
        REQUIRE( strcmp( uut.m_outputBuffer, ".a~.bcd;" ) == 0 );
    }

    SECTION( "errors")
    {
        uut.startFrame();
        uut.outputString( "abc");
        uut.startFrame();
        uut.outputString( "ZYX");
        uut.endFrame();
        REQUIRE( strcmp( uut.m_outputBuffer, ".ZYX;" ) == 0 );

        uut.startFrame();
        REQUIRE( uut.outputString( "abc") == true );
        uut.m_resultAppendOutput = false;
        REQUIRE( uut.outputString( "def") == false );
        REQUIRE( uut.endFrame() == false );

        uut.m_resultAppendOutput = true;
        uut.startFrame();
        REQUIRE( uut.outputString( "abc") == true );
        uut.m_resultEndOutput = false;
        REQUIRE( uut.endFrame() == false );

        uut.m_resultEndOutput = true;
        uut.startFrame();
        uut.outputString( "a.bcd" );
        uut.m_resultAppendOutput = false;
        REQUIRE( uut.endFrame() == false );

        uut.m_resultAppendOutput = true;
        uut.startFrame();
        uut.m_resultEndOutput = false;
        REQUIRE( uut.startFrame() == false );
    }

    REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
}
