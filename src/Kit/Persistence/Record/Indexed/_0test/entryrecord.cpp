/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Persistence/Record/Indexed/IEntry.h"
#include "Kit/Persistence/Types.h"
#include "catch2/catch_test_macros.hpp"
#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include "Kit/System/Trace.h"
#include "Kit/System/Assert.h"
#include "Kit/Persistence/Record/Chunk/Crc.h"
#include "Kit/Checksum/Fletcher16.h"
#include "Kit/Persistence/Record/Media/FileAdapter.h"
#include "Kit/Persistence/Record/Indexed/EntryRecord.h"
#include "Kit/Persistence/Record/Indexed/HeadRecord.h"
#include "Kit/EventQueue/Server.h"
#include "Kit/Io/File/System.h"
#include "Kit/Io/File/Output.h"
#define SECT_ "_0test"

///
using namespace Kit::Persistence::Record::Indexed;
using namespace Kit::Persistence::Record;
using namespace Kit::Persistence;

#define ENTRY_MAX_SIZE    7
#define ENTRY_REGION_SIZE 512

#define ENTRY1            "Hello"
#define ENTRY2            "World"
#define ENTRY3            "GoodBy"

////////////////////////////////////////////////////////////////////////////////
namespace {

class AppEntryPayload : public IPayload
{
public:
    AppEntryPayload() { memset( m_buffer, 0, ENTRY_MAX_SIZE ); }

public:
    Size_T copyTo( void* dst, Size_T maxDstLen ) noexcept override
    {
        REQUIRE( maxDstLen >= ENTRY_MAX_SIZE );
        memcpy( dst, m_buffer, ENTRY_MAX_SIZE );
        return ENTRY_MAX_SIZE;
    }

    bool copyFrom( const void* src, Size_T srcLen ) noexcept override
    {
        REQUIRE( ENTRY_MAX_SIZE <= srcLen );
        memcpy( m_buffer, src, ENTRY_MAX_SIZE );
        return true;
    };

    Size_T getMaxPayloadSize() const noexcept override { return ENTRY_MAX_SIZE; }

    //
    void appSet( const char* newValue )
    {
        m_buffer[0] = '\0';
        strncat( m_buffer, newValue, ENTRY_MAX_SIZE - 1 );
    }

public:
    char m_buffer[ENTRY_MAX_SIZE];
};


}  // end anonymous namespace

#define MEDIA_FILE_NAME     "media.bin"
#define MEDIA_MAX_SIZE      128
#define MEDIA_IDX_FILE_NAME "media.idx.bin"
#define MEDIA_IDX_MAX_SIZE  128

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "EntryRecord" )
{
    KIT_SYSTEM_TRACE_SCOPE( SECT_, "EntryRecord test" );
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();

    Kit::EventQueue::Server   mockEventQueue;
    Media::FileAdapter        entryFd1( MEDIA_FILE_NAME, MEDIA_MAX_SIZE );
    Media::FileAdapter        headFd2( MEDIA_IDX_FILE_NAME, MEDIA_IDX_MAX_SIZE );
    Kit::Checksum::Fletcher16 entryCrc;
    Chunk::Crc                entryChunk( entryFd1, entryCrc );
    Kit::Checksum::Fletcher16 headCrc;
    Chunk::Crc                headChunk( headFd2, headCrc );
    HeadRecord                headRecord( headChunk );
    AppEntryPayload           appPayload;
    EntryRecord               uut( entryChunk, ENTRY_MAX_SIZE, entryFd1, headRecord );
    Size_T                    expectedPayloadSize = sizeof( uint64_t ) + ENTRY_MAX_SIZE;
    Size_T                    expectedEntrySize   = expectedPayloadSize + entryChunk.getMetadataLength();
    Size_T                    expectedCopySize    = ENTRY_MAX_SIZE + sizeof( uint64_t );
    Size_T                    expectedMaxEntries  = MEDIA_MAX_SIZE / expectedEntrySize;

    SECTION( "Start/Stop" )
    {
        Kit::Io::File::System::remove( MEDIA_FILE_NAME );
        Kit::Io::File::System::remove( MEDIA_IDX_FILE_NAME );

        REQUIRE( uut.start( mockEventQueue ) );
        REQUIRE( uut.start( mockEventQueue ) );
        REQUIRE( uut.getSize() == ( expectedEntrySize * expectedMaxEntries ) );
        REQUIRE( uut.getMaxPayloadSize() == expectedEntrySize );

        // Stop the uut
        uut.stop();
        uut.stop();
    }

    SECTION( "no persistent data" )
    {
        // Delete files
        Kit::Io::File::System::remove( MEDIA_FILE_NAME );
        Kit::Io::File::System::remove( MEDIA_IDX_FILE_NAME );

        // No persistent data
        REQUIRE( uut.start( mockEventQueue ) );

        IEntry::Marker_T marker;
        bool             result = uut.getLatest( appPayload, marker );
        REQUIRE( result == false );

        result = headRecord.getLatestOffset( marker.mediaOffset, marker.timestamp );
        REQUIRE( result == true );
        REQUIRE( marker.mediaOffset == 0 );
        REQUIRE( marker.timestamp == 0 );

        // Write 3 entries
        appPayload.appSet( ENTRY1 );
        result = uut.addEntry( appPayload );
        REQUIRE( result == true );
        appPayload.appSet( ENTRY2 );
        result = uut.addEntry( appPayload );
        REQUIRE( result == true );
        appPayload.appSet( ENTRY3 );
        result = uut.addEntry( appPayload );
        REQUIRE( result == true );

        result = headRecord.getLatestOffset( marker.mediaOffset, marker.timestamp );
        REQUIRE( result == true );
        REQUIRE( marker.mediaOffset == ( expectedEntrySize * 3 ) );
        REQUIRE( marker.timestamp == 3 );

        uut.stop();
    }

    SECTION( "startup - with data" )
    {
        REQUIRE( uut.start( mockEventQueue ) );


        IEntry::Marker_T marker;
        bool             result = uut.getLatest( appPayload, marker );
        REQUIRE( result == true );
        REQUIRE( marker.mediaOffset == ( expectedEntrySize * 3 ) );
        REQUIRE( marker.timestamp == 3 );
        REQUIRE( strncmp( appPayload.m_buffer, ENTRY3, strlen( ENTRY3 ) ) == 0 );
        result = headRecord.getLatestOffset( marker.mediaOffset, marker.timestamp );
        REQUIRE( result == true );
        REQUIRE( marker.mediaOffset == ( expectedEntrySize * 3 ) );
        REQUIRE( marker.timestamp == 3 );

        uut.stop();
    }

    SECTION( "Corrupt index" )
    {
        // Delete file
        Kit::Io::File::System::remove( MEDIA_IDX_FILE_NAME );

        REQUIRE( uut.start( mockEventQueue ) );

        IEntry::Marker_T marker;
        bool             result = uut.getLatest( appPayload, marker );
        REQUIRE( result == true );
        REQUIRE( marker.timestamp == 3 );
        REQUIRE( marker.mediaOffset == ( expectedEntrySize * 3 ) );

        uut.stop();
    }

    SECTION( "Walk-next/prev" )
    {
        REQUIRE( uut.start( mockEventQueue ) );

        IEntry::Marker_T marker;
        bool             result = uut.getLatest( appPayload, marker );
        REQUIRE( result == true );
        REQUIRE( marker.mediaOffset == expectedEntrySize * 3 );
        REQUIRE( marker.timestamp == 3 );

        appPayload.appSet( "<none>" );
        IEntry::Marker_T marker2;
        result = uut.getNext( marker.timestamp, marker, appPayload, marker2 );
        REQUIRE( result == false );

        appPayload.appSet( "<none>" );
        result = uut.getPrevious( marker.timestamp, marker, appPayload, marker );
        REQUIRE( result == true );
        REQUIRE( strncmp( appPayload.m_buffer, ENTRY2, strlen( ENTRY2 ) ) == 0 );
        uint64_t timestampEntry2 = marker.timestamp;

        appPayload.appSet( "<none>" );
        result = uut.getPrevious( marker.timestamp, marker, appPayload, marker );
        REQUIRE( result == true );
        REQUIRE( strncmp( appPayload.m_buffer, ENTRY1, strlen( ENTRY1 ) ) == 0 );

        appPayload.appSet( "<none>" );
        result = uut.getPrevious( marker.timestamp, marker, appPayload, marker );
        REQUIRE( result == false );

        appPayload.appSet( "<none>" );
        result = uut.getNext( timestampEntry2, marker, appPayload, marker );
        REQUIRE( result == true );
        REQUIRE( strncmp( appPayload.m_buffer, ENTRY3, strlen( ENTRY3 ) ) == 0 );

        uut.stop();
    }

     SECTION( "Walk by index" )
    {
        REQUIRE( uut.start( mockEventQueue ) );

        IEntry::Marker_T marker;
        bool result = uut.getByEntryIndex( 2, appPayload, marker );
        REQUIRE( result == true );
        REQUIRE( strncmp( appPayload.m_buffer, ENTRY3, strlen( ENTRY3 ) ) == 0 );

        appPayload.appSet( "<none>" );
        result = uut.getByEntryIndex( 0, appPayload, marker );
        REQUIRE( result == true );
        REQUIRE( strncmp( appPayload.m_buffer, ENTRY1, strlen( ENTRY1 ) ) == 0 );

        appPayload.appSet( "<none>" );
        result = uut.getByEntryIndex( 3, appPayload, marker );
        REQUIRE( result == false );

        appPayload.appSet( "<none>" );
        result = uut.getByEntryIndex( 1, appPayload, marker );
        REQUIRE( result == true );
        REQUIRE( strncmp( appPayload.m_buffer, ENTRY2, strlen( ENTRY2 ) ) == 0 );

        for ( size_t idx=0; idx <= uut.getMaxIndex() + 1; idx++ )
        {
            IEntry::Marker_T marker;
            bool expectedResult = idx < 3 ? true : false;
            REQUIRE( uut.getByEntryIndex( idx, appPayload, marker ) == expectedResult );
        }
        uut.stop();
    }

    SECTION( "error cases" )
    {
        Kit::Io::File::System::remove( MEDIA_FILE_NAME );
        Kit::Io::File::System::remove( MEDIA_IDX_FILE_NAME );

        // Start the UUT
        REQUIRE( uut.start( mockEventQueue ) );

        // Add an entry
        appPayload.appSet( ENTRY1 );
        REQUIRE( uut.addEntry( appPayload ) == true );

        // Invalid copyTo() case
        uint8_t dummy[512];
        REQUIRE( uut.copyTo( dummy, expectedCopySize - 1 ) == KIT_PERSISTENCE_SIZE_MAX );
        REQUIRE( uut.copyTo( dummy, expectedCopySize ) == expectedCopySize );

        // Invalid copyFrom() case
        uint8_t src[512];
        REQUIRE( uut.copyFrom( src, expectedCopySize - 1 ) == false );
        REQUIRE( uut.copyFrom( src, expectedCopySize ) == true );

        // Stop the uut
        uut.stop();
    }

    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}