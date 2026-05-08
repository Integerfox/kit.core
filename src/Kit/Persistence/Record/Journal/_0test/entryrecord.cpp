/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Persistence/Record/Journal/IEntry.h"
#include "Kit/Persistence/Types.h"
#include "catch2/catch_test_macros.hpp"
#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include "Kit/System/Trace.h"
#include "Kit/System/Assert.h"
#include "Kit/Persistence/Record/Chunk/Crc.h"
#include "Kit/Checksum/Crc16CcittFast.h"
#include "Kit/Persistence/Record/Media/FileAdapter.h"
#include "Kit/Persistence/Record/Journal/EntryRecord.h"
#include "Kit/Persistence/Record/Journal/HeadRecord.h"
#include "Kit/EventQueue/Server.h"
#include "Kit/Io/File/System.h"
#include "Kit/Io/File/Output.h"
#define SECT_ "_0test"

///
using namespace Kit::Persistence::Record::Journal;
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
    Kit::Checksum::Crc16CcittFast entryCrc;
    Chunk::Crc                    entryChunk( entryFd1, entryCrc );
    Kit::Checksum::Crc16CcittFast headCrc;
    Chunk::Crc                    headChunk( headFd2, headCrc );
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

     SECTION( "reset head" )
    {
        REQUIRE( uut.start( mockEventQueue ) );

        IEntry::Marker_T marker;
        bool             result = uut.getLatest( appPayload, marker );
        REQUIRE( result == true );
        REQUIRE( marker.timestamp >= 3) ;

        uut.resetHead();
        result = uut.getLatest( appPayload, marker );
        REQUIRE( result == false );

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

    SECTION( "getNext - skip corrupt entries" )
    {
        Kit::Io::File::System::remove( MEDIA_FILE_NAME );
        Kit::Io::File::System::remove( MEDIA_IDX_FILE_NAME );

        // Use a local UUT with maxConsecutiveCorruptSkip=2 so the failure boundary
        // is easy to trigger deterministically (3 consecutive corrupt entries fail)
        EntryRecord uutSkip( entryChunk, ENTRY_MAX_SIZE, entryFd1, headRecord,
                             OPTION_KIT_PERSISTENCE_JOURNAL_ENTRY_RECORD_MAX_CORRUPT_SCAN,
                             2 );
        REQUIRE( uutSkip.start( mockEventQueue ) );

        // Add 6 entries.  With a fresh start, addEntry() increments from offset 0, so:
        //   ts=1 -> 1*expectedEntrySize
        //   ts=2 -> 2*expectedEntrySize
        //   ts=3 -> 3*expectedEntrySize
        //   ts=4 -> 4*expectedEntrySize
        //   ts=5 -> 5*expectedEntrySize
        //   ts=6 -> 0   (wraps)
        for ( int i = 0; i < 6; i++ )
        {
            appPayload.appSet( ENTRY1 );
            REQUIRE( uutSkip.addEntry( appPayload ) == true );
        }
        uutSkip.stop();

        // Starting marker: the entry at ts=1, offset=1*expectedEntrySize
        IEntry::Marker_T startMarker;
        startMarker.mediaOffset = 1 * expectedEntrySize;
        startMarker.timestamp   = 1;

        // Corrupt the 2 entries immediately following startMarker (ts=2 and ts=3)
        uint8_t garbage[MEDIA_MAX_SIZE];
        memset( garbage, 0xFF, expectedEntrySize );
        REQUIRE( entryFd1.write( 2 * expectedEntrySize, garbage, expectedEntrySize ) == true );
        REQUIRE( entryFd1.write( 3 * expectedEntrySize, garbage, expectedEntrySize ) == true );

        // Restart: head record still points to ts=6 at offset 0 (not corrupted),
        // so verifyIndex() accepts it and no full scan is triggered
        REQUIRE( uutSkip.start( mockEventQueue ) );

        // getNext should skip the 2 consecutive corrupt entries and return ts=4
        IEntry::Marker_T resultMarker;
        REQUIRE( uutSkip.getNext( 1, startMarker, appPayload, resultMarker ) == true );
        REQUIRE( resultMarker.timestamp == 4 );

        uutSkip.stop();

        // Now corrupt a 3rd consecutive entry (ts=4 at 4*expectedEntrySize).
        // Entries at 2*es, 3*es, 4*es are all corrupt: 3 consecutive, which
        // exceeds the limit of 2, so getNext must return false.
        REQUIRE( entryFd1.write( 4 * expectedEntrySize, garbage, expectedEntrySize ) == true );

        REQUIRE( uutSkip.start( mockEventQueue ) );

        REQUIRE( uutSkip.getNext( 1, startMarker, appPayload, resultMarker ) == false );

        uutSkip.stop();
    }

    SECTION( "getPrevious - skip corrupt entries" )
    {
        Kit::Io::File::System::remove( MEDIA_FILE_NAME );
        Kit::Io::File::System::remove( MEDIA_IDX_FILE_NAME );

        // Use a local UUT with maxConsecutiveCorruptSkip=2 so the failure boundary
        // is easy to trigger deterministically (3 consecutive corrupt entries fail)
        EntryRecord uutSkip( entryChunk, ENTRY_MAX_SIZE, entryFd1, headRecord,
                             OPTION_KIT_PERSISTENCE_JOURNAL_ENTRY_RECORD_MAX_CORRUPT_SCAN,
                             2 );
        REQUIRE( uutSkip.start( mockEventQueue ) );

        // Add 6 entries.  Entry layout after a fresh start:
        //   ts=1 -> 1*expectedEntrySize
        //   ts=2 -> 2*expectedEntrySize
        //   ts=3 -> 3*expectedEntrySize
        //   ts=4 -> 4*expectedEntrySize
        //   ts=5 -> 5*expectedEntrySize
        //   ts=6 -> 0   (wraps - this is the head/latest)
        for ( int i = 0; i < 6; i++ )
        {
            appPayload.appSet( ENTRY1 );
            REQUIRE( uutSkip.addEntry( appPayload ) == true );
        }
        uutSkip.stop();

        // Starting marker: the latest entry at ts=6, offset=0
        IEntry::Marker_T startMarker;
        startMarker.mediaOffset = 0;
        startMarker.timestamp   = 6;

        // Corrupt the 2 entries immediately preceding startMarker (ts=5 and ts=4)
        uint8_t garbage[MEDIA_MAX_SIZE];
        memset( garbage, 0xFF, expectedEntrySize );
        REQUIRE( entryFd1.write( 5 * expectedEntrySize, garbage, expectedEntrySize ) == true );
        REQUIRE( entryFd1.write( 4 * expectedEntrySize, garbage, expectedEntrySize ) == true );

        // Restart: head record still points to ts=6 at offset 0 (not corrupted),
        // so verifyIndex() accepts it and no full scan is triggered
        REQUIRE( uutSkip.start( mockEventQueue ) );

        // getPrevious should skip the 2 consecutive corrupt entries and return ts=3
        IEntry::Marker_T resultMarker;
        REQUIRE( uutSkip.getPrevious( 6, startMarker, appPayload, resultMarker ) == true );
        REQUIRE( resultMarker.timestamp == 3 );

        uutSkip.stop();

        // Now corrupt a 3rd consecutive entry (ts=3 at 3*expectedEntrySize).
        // Entries at 5*es, 4*es, 3*es are all corrupt: 3 consecutive, which
        // exceeds the limit of 2, so getPrevious must return false.
        REQUIRE( entryFd1.write( 3 * expectedEntrySize, garbage, expectedEntrySize ) == true );

        REQUIRE( uutSkip.start( mockEventQueue ) );

        REQUIRE( uutSkip.getPrevious( 6, startMarker, appPayload, resultMarker ) == false );

        uutSkip.stop();
    }


    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}