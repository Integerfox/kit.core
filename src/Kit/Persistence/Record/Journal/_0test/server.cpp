/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Persistence/Record/Journal/IReaderRequest.h"
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
#include "Kit/Itc/SyncReturnHandler.h"

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
static Kit::Container::RingBufferMP<AppEntryPayload> entriesBuffer_( mp_elemCount_, memoryEntryBuffer_, MAX_BUFFER_SIZE );

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
        RetrieveLatestRequest::Payload           payload( appPayload );
        Kit::Itc::SyncReturnHandler              srh;
        RetrieveLatestRequest::RetrieveLatestMsg msg( uut, payload, srh );
        persistentEventQueue.postSync( msg );
        REQUIRE( payload.m_success == true );
        REQUIRE( payload.m_markerEntryRetrieved.mediaOffset == actualEntrySize * 3 );
        REQUIRE( payload.m_markerEntryRetrieved.timestamp == 3 );
        REQUIRE( strncmp( appPayload.m_buffer, ENTRY3, strlen( ENTRY3 ) ) == 0 );

        uut.close();
    }
#if 0
    SECTION( "startup - with data" )
    {

        uut.open();

        appPayload.appSet( "" );
        GetLatestRequest::Payload      payload( appPayload );
        Kit::Itc::SyncReturnHandler    srh;
        GetLatestRequest::GetLatestMsg msg( uut, payload, srh );
        persistentEventQueue.postSync( msg );
        REQUIRE( payload.m_success == true );
        REQUIRE( payload.m_markerEntryRetrieved.mediaOffset == actualEntrySize * 3 );
        REQUIRE( payload.m_markerEntryRetrieved.timestamp == 3 );
        REQUIRE( strncmp( appPayload.m_buffer, ENTRY3, strlen( ENTRY3 ) ) == 0 );

        appPayload.appSet( "" );
        GetNextRequest::Payload     payload2( appPayload, payload.m_markerEntryRetrieved, payload.m_markerEntryRetrieved.timestamp );
        Kit::Itc::SyncReturnHandler srh2;
        GetNextRequest::GetNextMsg  msg2( uut, payload2, srh2 );
        persistentEventQueue.postSync( msg2 );
        REQUIRE( payload2.m_success == false );

        appPayload.appSet( "" );
        GetPreviousRequest::Payload        payload3( appPayload, payload.m_markerEntryRetrieved, payload.m_markerEntryRetrieved.timestamp );
        Kit::Itc::SyncReturnHandler        srh3;
        GetPreviousRequest::GetPreviousMsg msg3( uut, payload3, srh3 );
        persistentEventQueue.postSync( msg3 );
        REQUIRE( payload3.m_success == true );
        REQUIRE( payload3.m_markerEntryRetrieved.mediaOffset == actualEntrySize * 2 );
        REQUIRE( payload3.m_markerEntryRetrieved.timestamp == 2 );
        REQUIRE( strncmp( appPayload.m_buffer, ENTRY2, strlen( ENTRY2 ) ) == 0 );

        size_t idx = 0;
        appPayload.appSet( "" );
        GetByBufferIndexRequest::Payload             payload4( appPayload, idx );
        Kit::Itc::SyncReturnHandler                  srh4;
        GetByBufferIndexRequest::GetByBufferIndexMsg msg4( uut, payload4, srh4 );
        persistentEventQueue.postSync( msg4 );
        REQUIRE( payload4.m_success == true );
        REQUIRE( payload4.m_markerEntryRetrieved.mediaOffset == ( actualEntrySize * idx + actualEntrySize ) );
        REQUIRE( payload4.m_markerEntryRetrieved.timestamp == 1 );
        REQUIRE( strncmp( appPayload.m_buffer, ENTRY1, strlen( ENTRY1 ) ) == 0 );
        REQUIRE( uut.getMaxIndex() == MAX_ENTRIES - 1 );

        uut.close();
    }


    SECTION( "Clean-all" )
    {
        uut.open();

        ClearAllEntriesRequest::Payload            payload;
        Kit::Itc::SyncReturnHandler                srh;
        ClearAllEntriesRequest::ClearAllEntriesMsg msg( uut, payload, srh );
        persistentEventQueue.postSync( msg );

        uint32_t count;
        mp_elemCount_.read( count );
        REQUIRE( count == 0 );

        for ( size_t idx = 0; idx <= entryRecord.getMaxIndex(); idx++ )
        {
            GetByBufferIndexRequest::Payload             payload4( appPayload, 1 );
            Kit::Itc::SyncReturnHandler                  srh4;
            GetByBufferIndexRequest::GetByBufferIndexMsg msg4( uut, payload4, srh4 );
            persistentEventQueue.postSync( msg4 );
            REQUIRE( payload4.m_success == false );
        }

        uut.close();
    }
#endif

    persistentEventQueue.pleaseStop();
    Kit::System::Thread::destroy( *t1 );
    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}