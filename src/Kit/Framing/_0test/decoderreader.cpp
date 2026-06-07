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
#include "Kit/Framing/DecoderReader.h"
#include <string.h>


///
using namespace Kit::System;
using namespace Kit::Framing;

namespace {

class MyUtt : public DecoderReader
{
public:
    static constexpr size_t WORK_BUFFER_SIZE = 2;
    uint8_t                 m_workBuffer[WORK_BUFFER_SIZE];
    uint8_t                 m_sof;
    uint8_t                 m_eof;
    uint8_t                 m_escape;
    uint8_t                 m_illegalByte;
    const char*             m_inputSource;
    size_t                  m_inputSourceLength;
    size_t                  m_inputSourceIndex;
public:
    MyUtt( const char* inputSource,
           size_t      inputSourceLength,
           char        startOfFrame = '.',
           char        endOfFrame   = ';',
           char        escape       = '~',
           char        illegalByte  = '*' ) noexcept
        : DecoderReader( m_workBuffer, WORK_BUFFER_SIZE )
        , m_sof( startOfFrame )
        , m_eof( endOfFrame )
        , m_escape( escape )
        , m_illegalByte( illegalByte )
        , m_inputSource( inputSource )
        , m_inputSourceLength( inputSourceLength )
        , m_inputSourceIndex( 0 )
    {
    }

public:
    bool isStartOfFrame( uint8_t byte ) noexcept override
    {
        return byte == m_sof;
    }

    bool isEndOfFrame( uint8_t byte ) noexcept override
    {
        return byte == m_eof;
    }
    bool isEscapeByte( uint8_t byte ) noexcept override
    {
        return byte == m_escape;
    }
    bool isLegalByte( uint8_t byte ) noexcept override
    {
        return byte != m_illegalByte;
    }
    bool read( void*               dstBuffer,
               Kit::Type::SSize_T  numBytes,
               Kit::Type::SSize_T& bytesRead ) noexcept override
    {
        if ( m_inputSourceIndex >= m_inputSourceLength )
        {
            bytesRead = 0;
            return false;
        }

        size_t bytesToRead = std::min( static_cast<size_t>( numBytes ), m_inputSourceLength - m_inputSourceIndex );
        memcpy( dstBuffer, m_inputSource + m_inputSourceIndex, bytesToRead );
        m_inputSourceIndex += bytesToRead;
        bytesRead           = bytesToRead;
        return true;
    }
};

}  // end anonymous namespace
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "DecoderReader" )
{
    ShutdownUnitTesting::clearAndUseCounter();
    static constexpr size_t MAX_FRAME_SIZE = 12;
    uint8_t                 frameBuffer[MAX_FRAME_SIZE];
    Kit::Type::SSize_T      frameSize;

    SECTION( "Decode1" )
    {
        MyUtt uut( ".abcde;", 7 );
        bool  result = uut.scan( MAX_FRAME_SIZE, frameBuffer, frameSize );
        REQUIRE( result == true );
        REQUIRE( strncmp( (char*)frameBuffer, "abcde", frameSize ) == 0 );
    }

    SECTION( "Decode2" )
    {
        MyUtt uut( ".a;b.c;", 7 );
        bool  result = uut.scan( MAX_FRAME_SIZE, frameBuffer, frameSize );
        REQUIRE( result == true );
        REQUIRE( strncmp( (char*)frameBuffer, "ac", frameSize ) == 0 );
        result = uut.scan( MAX_FRAME_SIZE, frameBuffer, frameSize );
        REQUIRE( result == true );
        REQUIRE( strncmp( (char*)frameBuffer, "c", frameSize ) == 0 );
    }

    SECTION( "Decode3" )
    {
        MyUtt uut( ".a~;bcd;", 8 );
        bool  result = uut.scan( MAX_FRAME_SIZE, frameBuffer, frameSize );
        REQUIRE( result == true );
        REQUIRE( strncmp( (char*)frameBuffer, "a;bcd", frameSize ) == 0 );
    }

    SECTION( "Decode4" )
    {
        MyUtt uut( ".~~;", 4 );
        bool  result = uut.scan( MAX_FRAME_SIZE, frameBuffer, frameSize );
        REQUIRE( result == true );
        REQUIRE( strncmp( (char*)frameBuffer, "~", frameSize ) == 0 );
    }

    SECTION( "Decode5" )
    {
        MyUtt uut( ".a~.bcd;", 8 );
        bool  result = uut.scan( MAX_FRAME_SIZE, frameBuffer, frameSize );
        REQUIRE( result == true );
        REQUIRE( strncmp( (char*)frameBuffer, "a.bcd", frameSize ) == 0 );
    }

    SECTION( "Decode6" )
    {
        MyUtt uut( "cd~.abx;", 8 );
        bool  result = uut.scan( MAX_FRAME_SIZE, frameBuffer, frameSize );
        REQUIRE( result == true );
        REQUIRE( strncmp( (char*)frameBuffer, "abx", frameSize ) == 0 );
        result = uut.scan( MAX_FRAME_SIZE, frameBuffer, frameSize );
        REQUIRE( result == false );
    }

    SECTION( "oobRead1" )
    {
        MyUtt              uut( "abcde", 5 );
        uint8_t            readBuffer[10];
        Kit::Type::SSize_T bytesRead;
        bool               result = uut.oobRead( readBuffer, sizeof( readBuffer ), bytesRead );
        REQUIRE( result == true );
        REQUIRE( bytesRead == 2 );
        REQUIRE( strncmp( (char*)readBuffer, "ab", bytesRead ) == 0 );
        result = uut.oobRead( readBuffer, sizeof( readBuffer ), bytesRead );
        REQUIRE( result == true );
        REQUIRE( bytesRead == 2 );
        REQUIRE( strncmp( (char*)readBuffer, "cd", bytesRead ) == 0 );
        result = uut.oobRead( readBuffer, sizeof( readBuffer ), bytesRead );
        REQUIRE( result == true );
        REQUIRE( bytesRead == 1 );
        REQUIRE( strncmp( (char*)readBuffer, "e", bytesRead ) == 0 );
    }

    SECTION( "oobRead2" )
    {
        MyUtt uut( "a.bcd;hi.bob;", 13 );
        bool  result = uut.scan( MAX_FRAME_SIZE, frameBuffer, frameSize );
        REQUIRE( result == true );
        REQUIRE( strncmp( (char*)frameBuffer, "bcd", frameSize ) == 0 );

        uint8_t            readBuffer[10];
        Kit::Type::SSize_T bytesRead;
        result = uut.oobRead( readBuffer, sizeof( readBuffer ), bytesRead );
        REQUIRE( result == true );
        REQUIRE( bytesRead == 2 );
        REQUIRE( strncmp( (char*)readBuffer, "hi", bytesRead ) == 0 );

        result = uut.scan( MAX_FRAME_SIZE, frameBuffer, frameSize );
        REQUIRE( result == true );
        REQUIRE( strncmp( (char*)frameBuffer, "bob", frameSize ) == 0 );
    }

    SECTION( "oobRead3" )
    {
        MyUtt              uut( "abc", 3 );
        uint8_t            readBuffer[1];
        Kit::Type::SSize_T bytesRead;
        bool               result = uut.oobRead( readBuffer, sizeof( readBuffer ), bytesRead );
        REQUIRE( result == true );
        REQUIRE( bytesRead == 1 );
        REQUIRE( strncmp( (char*)readBuffer, "a", bytesRead ) == 0 );
        result = uut.oobRead( readBuffer, sizeof( readBuffer ), bytesRead );
        REQUIRE( result == true );
        REQUIRE( bytesRead == 1 );
        REQUIRE( strncmp( (char*)readBuffer, "b", bytesRead ) == 0 );
        result = uut.oobRead( readBuffer, sizeof( readBuffer ), bytesRead );
        REQUIRE( result == true );
        REQUIRE( bytesRead == 1 );
        REQUIRE( strncmp( (char*)readBuffer, "c", bytesRead ) == 0 );
        result = uut.oobRead( readBuffer, sizeof( readBuffer ), bytesRead );
        REQUIRE( result == false );
    }

    SECTION( "IllegalByte" )
    {
        MyUtt uut( ".a*b;.good;", 11 );
        bool  result = uut.scan( MAX_FRAME_SIZE, frameBuffer, frameSize );
        REQUIRE( result == true );
        REQUIRE( strncmp( (char*)frameBuffer, "good", frameSize ) == 0 );
    }

    SECTION( "error1" )
    {
        MyUtt uut( "aab;", 4 );
        bool  result = uut.scan( MAX_FRAME_SIZE, frameBuffer, frameSize );
        REQUIRE( result == false );
        result = uut.scan( MAX_FRAME_SIZE, frameBuffer, frameSize );
        REQUIRE( result == false );
    }

    SECTION( "error2" )
    {
        MyUtt uut( ".ab~;a", 6 );
        bool  result = uut.scan( MAX_FRAME_SIZE, frameBuffer, frameSize );
        REQUIRE( result == false );
        result = uut.scan( MAX_FRAME_SIZE, frameBuffer, frameSize );
        REQUIRE( result == false );
    }

    SECTION( "error3" )
    {
        MyUtt uut( ".very log string that exceeds the framebuffer;", 46 );
        bool  result = uut.scan( MAX_FRAME_SIZE, nullptr, frameSize );
        REQUIRE( result == false );
        result = uut.scan( MAX_FRAME_SIZE, frameBuffer, frameSize );
        REQUIRE( result == false );
    }

    REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
}
