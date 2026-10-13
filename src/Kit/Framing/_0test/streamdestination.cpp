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
#include "Kit/Framing/StreamDestination.h"
#include "Kit/Io/Ram/InputOutputAllocate.h"
#include "Kit/System/Trace.h"
#include <string.h>

#define SECT_ "_0test"

///
using namespace Kit::System;
using namespace Kit::Framing;

namespace {

/// Output stream that captures the waitForWriteSpace() arguments
class MockOutput : public Kit::Io::IOutput
{
public:
    Kit::Type::SSize_T m_lastNumBytes;
    uint32_t           m_lastTimeoutMs;
    unsigned           m_callCount;
    bool               m_resultWaitForWriteSpace;

public:
    MockOutput() noexcept
        : m_lastNumBytes( 0 )
        , m_lastTimeoutMs( 0 )
        , m_callCount( 0 )
        , m_resultWaitForWriteSpace( true )
    {
    }

public:
    /// Pull in overloaded methods from base class
    using Kit::Io::IOutput::write;

    bool write( const void*, Kit::Type::SSize_T maxBytes, Kit::Type::SSize_T& bytesWritten ) noexcept override
    {
        bytesWritten = maxBytes;
        return true;
    }

    bool waitForWriteSpace( Kit::Type::SSize_T numBytes, uint32_t timeoutMs ) noexcept override
    {
        m_lastNumBytes  = numBytes;
        m_lastTimeoutMs = timeoutMs;
        m_callCount++;
        return m_resultWaitForWriteSpace;
    }

    void flush() noexcept override {}
    bool isEos() noexcept override { return false; }
    void close() noexcept override {}
};

}  // end anonymous namespace

////////////////////////////////////
TEST_CASE( "StreamDestination" )
{
    ShutdownUnitTesting::clearAndUseCounter();
    Kit::Io::Ram::InputOutputAllocate<20> dst;
    StreamDestination                     uut( dst );
    char                                  buffer[10];
    Kit::Type::SSize_T                    bytesRead;

    REQUIRE( uut.getStream() == &dst );
    REQUIRE( uut.startOutput() == true );
    REQUIRE( uut.appendOutput( "Hello", 5 ) == true );
    REQUIRE( dst.available() == true );
    REQUIRE( dst.read( buffer, 5, bytesRead ) == true );
    REQUIRE( bytesRead == 5 );
    REQUIRE( strncmp( buffer, "Hello", 5 ) == 0 );
    REQUIRE( uut.endOutput() == true );

    REQUIRE( uut.startOutput() == true );
    REQUIRE( uut.appendOutput( "Kitty!", 6 ) == true );
    REQUIRE( uut.appendOutput( nullptr, 6 ) == false );
    REQUIRE( dst.available() == true );
    REQUIRE( dst.read( buffer, 6, bytesRead ) == true );
    REQUIRE( bytesRead == 6 );
    REQUIRE( strncmp( buffer, "Kitty!", 6 ) == 0 );
    REQUIRE( uut.endOutput() == true );

    dst.close();
    REQUIRE( uut.startOutput() == true );
    REQUIRE( uut.appendOutput( "Kitty!", 6 ) == false );
    REQUIRE( uut.endOutput() == true );

    StreamDestination uut2;
    REQUIRE( uut2.getStream() == nullptr );
    REQUIRE( uut2.startOutput() == false );
    REQUIRE( uut2.appendOutput( "Kitty!", 6 ) == false );
    REQUIRE( uut2.endOutput() == false );

    StreamDestination uut3;
    uut3.setOutput( dst );
    REQUIRE( uut3.getStream() == &dst );
    REQUIRE( uut3.startOutput() == true );
    REQUIRE( uut3.appendOutput( "Kitty!", 6 ) == false );
    REQUIRE( uut3.endOutput() == true );

    REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
}

////////////////////////////////////
TEST_CASE( "StreamDestination::waitForOutputSpace" )
{
    ShutdownUnitTesting::clearAndUseCounter();

    SECTION( "delegates to the stream" )
    {
        MockOutput        dst;
        StreamDestination uut( dst );

        REQUIRE( uut.waitForOutputSpace( 12, 500 ) == true );
        REQUIRE( dst.m_callCount == 1 );
        REQUIRE( dst.m_lastNumBytes == 12 );
        REQUIRE( dst.m_lastTimeoutMs == 500 );

        dst.m_resultWaitForWriteSpace = false;
        REQUIRE( uut.waitForOutputSpace( 3, 0 ) == false );
        REQUIRE( dst.m_callCount == 2 );
        REQUIRE( dst.m_lastNumBytes == 3 );
        REQUIRE( dst.m_lastTimeoutMs == 0 );
    }

    SECTION( "no output stream set" )
    {
        StreamDestination uut;
        REQUIRE( uut.waitForOutputSpace( 12, 500 ) == true );
        REQUIRE( uut.startOutput() == false );

        // Once a stream is set, the call is delegated
        MockOutput dst;
        uut.setOutput( dst );
        dst.m_resultWaitForWriteSpace = false;
        REQUIRE( uut.waitForOutputSpace( 7, 100 ) == false );
        REQUIRE( dst.m_callCount == 1 );
        REQUIRE( dst.m_lastNumBytes == 7 );
        REQUIRE( dst.m_lastTimeoutMs == 100 );
    }

    SECTION( "stream with no waitForWriteSpace() support" )
    {
        // The Ram stream does not override waitForWriteSpace() -->always true
        Kit::Io::Ram::InputOutputAllocate<20> dst;
        StreamDestination                     uut( dst );
        REQUIRE( uut.waitForOutputSpace( 1000, 0 ) == true );
    }

    REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
}
