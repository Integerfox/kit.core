/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Persistence/Types.h"
#include "catch2/catch_test_macros.hpp"
#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include "Kit/System/Trace.h"
#include "Kit/System/Assert.h"
#include "Kit/Persistence/Record/Chunk/Crc.h"
#include "Kit/Checksum/Fletcher16.h"
#include "Kit/Persistence/Record/Media/FileAdapter.h"
#include "Kit/Persistence/Record/Indexed/HeadRecord.h"
#include "Kit/EventQueue/Server.h"
#include "Kit/Io/File/System.h"
#include "Kit/Io/File/Output.h"
#define SECT_ "_0test"

///
using namespace Kit::Persistence::Record::Indexed;
using namespace Kit::Persistence::Record;
using namespace Kit::Persistence;


////////////////////////////////////////////////////////////////////////////////
namespace {

class MyUut : public HeadRecord
{
public:
    MyUut( Kit::Persistence::Record::IChunk& chunkHandler ) noexcept
        : HeadRecord( chunkHandler )
        , m_noValidDataCount( 0 )
        , m_copyToCount( 0 )
        , m_copyFromCount( 0 )
        , m_lastLatest( 0 )
        , m_lastLatestIndex( 0 )
        , m_failProcessNoValidData( false )
    {
    }

    bool processNoValidData() noexcept
    {
        m_noValidDataCount++;
        if ( m_failProcessNoValidData )
        {
            return false;
        }

        bool result       = HeadRecord::processNoValidData();
        m_lastLatest      = m_latestOffset;
        m_lastLatestIndex = m_latestTimestamp;
        return result;
    }

    Size_T copyTo( void* dst, Size_T maxDstLen ) noexcept
    {
        m_copyToCount++;
        return HeadRecord::copyTo( dst, maxDstLen );
    }

    bool copyFrom( const void* src, Size_T srcLen ) noexcept
    {
        m_copyFromCount++;
        bool result       = HeadRecord::copyFrom( src, srcLen );
        m_lastLatest      = m_latestOffset;
        m_lastLatestIndex = m_latestTimestamp;
        return result;
    }

    void setLatestOffset( Size_T offset, uint64_t indexValue ) noexcept
    {
        HeadRecord::setLatestOffset( offset, indexValue );
        m_lastLatest      = m_latestOffset;
        m_lastLatestIndex = indexValue;
    }


    unsigned m_noValidDataCount;
    unsigned m_copyToCount;
    unsigned m_copyFromCount;
    Size_T   m_lastLatest;
    uint64_t m_lastLatestIndex;
    unsigned m_failProcessNoValidData;
};


}  // end anonymous namespace

#define MEDIA_FILE_NAME "media.bin"
#define MEDIA_MAX_SIZE  128

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "HeadRecord" )
{
    KIT_SYSTEM_TRACE_SCOPE( SECT_, "HeadRecord test" );
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();

    Kit::EventQueue::Server              mockEventQueue;
    Media::FileAdapter                   fd1( MEDIA_FILE_NAME, MEDIA_MAX_SIZE );
    Kit::Checksum::Fletcher16            crc;
    Kit::Persistence::Record::Chunk::Crc chunkHandler( fd1, crc );
    MyUut                                uut( chunkHandler );

    SECTION( "Start/Stop" )
    {
        REQUIRE( uut.start( mockEventQueue ) );
        REQUIRE( uut.start( mockEventQueue ) );
        REQUIRE( uut.m_copyToCount == 0 );
        REQUIRE( uut.m_copyFromCount == 0 );
        REQUIRE( uut.m_noValidDataCount == 0 );
        REQUIRE( uut.m_lastLatest == 0 );
        REQUIRE( uut.m_lastLatestIndex == 0 );

        Size_T expectedPayloadSize = sizeof( uut.m_lastLatest ) + sizeof( uut.m_lastLatestIndex );
        REQUIRE( uut.getMaxPayloadSize() == expectedPayloadSize );
        REQUIRE( uut.getSize() == ( expectedPayloadSize + chunkHandler.getMetadataLength() ) );

        // Stop the uut
        uut.stop();
        uut.stop();
    }

    SECTION( "No valid data" )
    {
        Kit::Io::File::System::remove( MEDIA_FILE_NAME );

        REQUIRE( uut.start( mockEventQueue ) );
        REQUIRE( uut.readFromMedia() == true );
        REQUIRE( uut.m_copyToCount == 0 );
        REQUIRE( uut.m_copyFromCount == 0 );
        REQUIRE( uut.m_noValidDataCount == 1 );
        REQUIRE( uut.m_lastLatest == 0 );
        REQUIRE( uut.m_lastLatestIndex == 0 );

        uut.m_failProcessNoValidData = true;
        REQUIRE( uut.readFromMedia() == false );
        REQUIRE( uut.m_copyToCount == 0 );
        REQUIRE( uut.m_copyFromCount == 0 );
        REQUIRE( uut.m_noValidDataCount == 2 );

        // Write some data
        uut.setLatestOffset( 1, 11 );
        REQUIRE( uut.writeToMedia() == true );
        REQUIRE( uut.m_copyToCount == 1 );
        REQUIRE( uut.m_copyFromCount == 0 );
        REQUIRE( uut.m_noValidDataCount == 2 );
        REQUIRE( uut.m_lastLatest == 1 );
        REQUIRE( uut.m_lastLatestIndex == 11 );
        uut.stop();
    }

    SECTION( "valid data" )
    {
        // Assume data is already written from the previous section
        REQUIRE( uut.start( mockEventQueue ) );
        REQUIRE( uut.m_copyToCount == 0 );
        REQUIRE( uut.m_copyFromCount == 0 );
        REQUIRE( uut.m_noValidDataCount == 0 );

        // Read back the data
        REQUIRE( uut.readFromMedia() == true );
        REQUIRE( uut.m_copyToCount == 0 );
        REQUIRE( uut.m_copyFromCount == 1 );
        REQUIRE( uut.m_noValidDataCount == 0 );
        REQUIRE( uut.m_lastLatest == 1 );
        REQUIRE( uut.m_lastLatestIndex == 11 );

        // Stop the uut
        uut.stop();
    }

    SECTION( "invalidate offsets" )
    {
        // Start the UUT
        REQUIRE( uut.start( mockEventQueue ) );
        REQUIRE( uut.m_copyToCount == 0 );
        REQUIRE( uut.m_copyFromCount == 0 );
        REQUIRE( uut.m_noValidDataCount == 0 );

        Size_T   dummy[2 + 2 * 2];
        Size_T   offset = 0;
        uint64_t indexValue;
        REQUIRE( uut.getLatestOffset( offset, indexValue ) == false );
        REQUIRE( uut.copyTo( dummy, sizeof( dummy ) ) == KIT_PERSISTENCE_SIZE_MAX );
        uut.setLatestOffset( offset, 12 );
        REQUIRE( uut.getLatestOffset( offset, indexValue ) == true );
        REQUIRE( uut.m_lastLatestIndex == 12 );

        // Stop the uut
        uut.stop();
    }

    SECTION("error cases")
    {
        // Start the UUT
        REQUIRE( uut.start( mockEventQueue ) );
        REQUIRE( uut.readFromMedia() == true );
        REQUIRE( uut.m_copyToCount == 0 );
        REQUIRE( uut.m_copyFromCount == 1 );
        REQUIRE( uut.m_noValidDataCount == 0 );
     
        // Invalid copyTo() case
        constexpr Size_T expectedPayloadSize = sizeof( uut.m_lastLatest ) + sizeof( uut.m_lastLatestIndex );
        uint8_t dummy[expectedPayloadSize];
        REQUIRE( uut.copyTo( dummy, sizeof( dummy ) - 1 ) == KIT_PERSISTENCE_SIZE_MAX );
        REQUIRE( uut.copyTo( dummy, sizeof( dummy ) ) == sizeof( dummy ) );

        // Invalid copyFrom() case
        uint8_t src[expectedPayloadSize];
        REQUIRE( uut.copyFrom( src, sizeof( src ) - 1 ) == false );
        REQUIRE( uut.copyFrom( src, sizeof( src ) ) == true );

        // Stop the uut
        uut.stop();
    }

    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}