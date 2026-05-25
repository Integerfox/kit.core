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
#include "Kit/Persistence/Record/Journal/IReaderRequest.h"
#include "Kit/Persistence/Record/Journal/IResetRequest.h"
#include "Kit/Persistence/Types.h"
#include "catch2/catch_test_macros.hpp"
#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include "Kit/System/Trace.h"
#include "Kit/System/Thread.h"
#include "Kit/Persistence/Record/Chunk/Crc.h"
#include "Kit/Checksum/Crc16CcittFast.h"
#include "Kit/Persistence/Record/Media/FileAdapter.h"
#include "Kit/Persistence/Record/Journal/EntryRecord.h"
#include "Kit/Persistence/Record/Journal/HeadRecord.h"
#include "Kit/Persistence/Record/Journal/Server.h"
#include "Kit/Dm/ModelDatabase.h"
#include "Kit/Dm/Mp/Uint32.h"
#include "Kit/Container/RingBufferMP.h"
#include "Kit/EventQueue/Server.h"
#include "Kit/Io/File/System.h"
#include "Kit/_support/testing/helpers.h"

#define SECT_ "_0test"

///
using namespace Kit::Persistence::Record::Journal;
using namespace Kit::Persistence::Record;
using namespace Kit::Persistence;

#define ENTRY_MAX_SIZE      7
#define ENTRY_REGION_SIZE   512

#define MEDIA_FILE_NAME     "media.bin"
#define MEDIA_MAX_SIZE      128
#define MEDIA_IDX_FILE_NAME "media.idx.bin"
#define MEDIA_IDX_MAX_SIZE  128

#define ENTRY1              "Hello"
#define ENTRY2              "World"
#define ENTRY3              "GoodBy"

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


// Allocate/create my Model Database
static Kit::Dm::ModelDatabase modelDb_( "ignoreThisParameter_usedToInvokeTheStaticConstructor" );

// Allocate my Model Points
static Kit::Dm::Mp::Uint32 mp_elemCount_( modelDb_, "count" );


static Kit::Persistence::Record::Media::FileAdapter indexFd1( MEDIA_IDX_FILE_NAME, 128 );
static Kit::Persistence::Record::Media::FileAdapter entriesFd( MEDIA_FILE_NAME, ENTRY_REGION_SIZE );
static Kit::Checksum::Crc16CcittFast                indexCrc;
static Kit::Checksum::Crc16CcittFast                entriesCrc;
static Kit::Persistence::Record::Chunk::Crc         indexRecChunk( indexFd1, indexCrc );
static Kit::Persistence::Record::Chunk::Crc         entriesChunk( entriesFd, entriesCrc );

#define MAX_BUFFER_SIZE 10
static AppEntryPayload                               memoryEntryBuffer_[MAX_BUFFER_SIZE];
static Kit::Container::RingBufferMP<AppEntryPayload> entriesBuffer_( mp_elemCount_, memoryEntryBuffer_, MAX_BUFFER_SIZE, false ); // NOTE: Very important to set 'initializeMemory' to false since AppEntryPayload is a class type with vtable pointer

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Server" )
{
    KIT_SYSTEM_TRACE_SCOPE( SECT_, "Server test" );
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();

    Kit::EventQueue::Server persistentEventQueue;
    AppEntryPayload         appPayload;
    HeadRecord              headRecord( indexRecChunk );
    EntryRecord             entryRecord( entriesChunk, ENTRY_MAX_SIZE, entriesFd, headRecord );
    Server<AppEntryPayload> uut( persistentEventQueue, entryRecord, entriesBuffer_ );
    Kit::System::Thread*    t1 = Kit::System::Thread::create( persistentEventQueue, "UUT" );
    REQUIRE( t1 );

    size_t actualEntrySize = entryRecord.getSize();

    SECTION( "no persistent data" )
    {
        // Delete files
        Kit::Io::File::System::remove( MEDIA_IDX_FILE_NAME );
        Kit::Io::File::System::remove( MEDIA_FILE_NAME );

        // No persistent data
        uut.open();

        // Add entries
        appPayload.appSet( ENTRY1 );
        REQUIRE( entriesBuffer_.add( appPayload ) );
        appPayload.appSet( ENTRY2 );
        REQUIRE( entriesBuffer_.add( appPayload ) );
        appPayload.appSet( ENTRY3 );
        REQUIRE( entriesBuffer_.add( appPayload ) );

        // Wait for changes...
        REQUIRE( minWaitOnModelPoint<Kit::Dm::Mp::Uint32, uint32_t>( mp_elemCount_, 0, 1000 ) );

        appPayload.appSet( "" );
        IEntry::Marker_T marker;
        bool result = uut.retrieveLatest( appPayload, marker );
        REQUIRE( result == true );
        REQUIRE( marker.mediaOffset == actualEntrySize * 3 );
        REQUIRE( marker.timestamp == 3 );
        REQUIRE( strncmp( appPayload.m_buffer, ENTRY3, strlen( ENTRY3 ) ) == 0 );

        uut.close();
    }
    SECTION( "startup - with data" )
    {

        uut.open();

        appPayload.appSet( "" );
        IEntry::Marker_T marker;
        bool result = uut.retrieveLatest( appPayload, marker );
        REQUIRE( result == true );
        REQUIRE( marker.mediaOffset == actualEntrySize * 3 );
        REQUIRE( marker.timestamp == 3 );
        REQUIRE( strncmp( appPayload.m_buffer, ENTRY3, strlen( ENTRY3 ) ) == 0 );

        appPayload.appSet( "" );
        IEntry::Marker_T marker2;
        result = uut.retrieveNext( marker.timestamp, marker, appPayload, marker2 );
        REQUIRE( result == false );

        appPayload.appSet( "" );
        IEntry::Marker_T marker3;
        result = uut.retrieveNext( marker2.timestamp, marker2, appPayload, marker3 );
        REQUIRE( result == false );

        appPayload.appSet( "" );
        IEntry::Marker_T marker4;
        result = uut.retrievePrevious( marker.timestamp, marker, appPayload, marker4 );
        REQUIRE( result == true );
        REQUIRE( marker4.mediaOffset == actualEntrySize * 2 );
        REQUIRE( marker4.timestamp == 2 );
        REQUIRE( strncmp( appPayload.m_buffer, ENTRY2, strlen( ENTRY2 ) ) == 0 );

        size_t idx = 0;
        appPayload.appSet( "" );
        IEntry::Marker_T marker5;
        result = uut.retrieveByEntryIndex( idx, appPayload, marker5 );
        REQUIRE( result == true );
        REQUIRE( marker5.mediaOffset == ( actualEntrySize * idx + actualEntrySize ) );
        REQUIRE( marker5.timestamp == 1 );
        REQUIRE( strncmp( appPayload.m_buffer, ENTRY1, strlen( ENTRY1 ) ) == 0 );
        Size_T expectedMaxIndex = ( ENTRY_REGION_SIZE / actualEntrySize ) - 1;
        REQUIRE( uut.maxIndex() == expectedMaxIndex );

        uut.close();
    }

    SECTION( "Reset" )
    {
        uut.open();
        REQUIRE( uut.logicalReset() == true );

        IEntry::Marker_T marker;
        bool result = uut.retrieveLatest( appPayload, marker );
        REQUIRE( result == false );

        uut.close();
    }

    persistentEventQueue.pleaseStop();
    Kit::System::Thread::destroy( *t1 );
    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}