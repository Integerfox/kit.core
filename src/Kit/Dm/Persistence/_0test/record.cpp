/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Checksum/Fletcher16.h"
#include "Kit/Dm/IObserver.h"
#include "Kit/Dm/ObserverCallback.h"
#include "Kit/_support/testing/ManualEventQueue.h"
#include "Kit/System/ElapsedTime.h"
#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include "catch2/catch_test_macros.hpp"
#include "Kit/System/Trace.h"
#include "Kit/System/Api.h"
#include "Kit/Dm/ModelDatabase.h"
#include "Kit/Dm/Persistence/Record.h"
#include "Kit/Dm/Mp/Uint32.h"
#include "Kit/Persistence/Record/Server.h"
#include "Kit/Persistence/Record/Chunk/Crc.h"
#include "Kit/Persistence/Record/Media/FileAdapter.h"
#include "Kit/Io/File/System.h"


#define SECT_                      "_0test"

#define FILE_NAME_REGION1          "region1.nvram"

#define EXPECTED_DM_RECORD_METALEN 2

using namespace Kit::Dm::Persistence;

// Allocate/create my Model Database
static Kit::Dm::ModelDatabase modelDb_( "ignoreThisParameter_usedToInvokeTheStaticConstructor" );

// Allocate my Model Points
static Kit::Dm::Mp::Uint32 mp_apple_( modelDb_, "APPLE1" );
static Kit::Dm::Mp::Uint32 mp_orange_( modelDb_, "ORANGE1" );
static Kit::Dm::Mp::Uint32 mp_cherry_( modelDb_, "CHERRY1" );
static Kit::Dm::Mp::Uint32 mp_plum_( modelDb_, "PLUM1" );

#define DEFAULT_APPLE  0xAAAA5555
#define DEFAULT_ORANGE 0xBBBB7777
#define DEFAULT_PLUM   0xFFFF9999
#define DEFAULT_CHERRY 0xDDDD3333


class MyRecord : public Record
{
public:
    constexpr static unsigned                       NUM_ITEMS = 3;
    Kit::Dm::ObserverCallback<Kit::Dm::IModelPoint> m_observersList[NUM_ITEMS];
    Item_T                                          m_itemList[NUM_ITEMS];
    int                                             m_resetDataCount;
    int                                             m_schemaChangeCount;
    bool                                            m_resetDataResult;
    int                                             m_dataChangedCount;
    int                                             m_updateNVRAMCount;
    uint32_t                                        m_deltaMs;
    uint32_t                                        m_timeChange;

public:
    MyRecord( Kit::Persistence::Record::IChunk& chunkHandler, uint8_t major, uint8_t minor, uint32_t delayMs = 0, uint32_t maxDelayMs = 0 ) noexcept
        : Record( m_itemList,
                  NUM_ITEMS,
                  chunkHandler,
                  major,
                  minor,
                  delayMs,
                  maxDelayMs )
        , m_resetDataCount( 0 )
        , m_schemaChangeCount( 0 )
        , m_resetDataResult( true )
        , m_dataChangedCount( 0 )
        , m_updateNVRAMCount( 0 )
        , m_deltaMs( 0 )
        , m_timeChange( 0 )
    {
        m_itemList[0] = { &mp_apple_, &m_observersList[0] };
        m_itemList[1] = { &mp_orange_, &m_observersList[1] };
        m_itemList[2] = { &mp_plum_, nullptr };
    }

    bool resetData() noexcept
    {
        mp_apple_.write( DEFAULT_APPLE );
        mp_orange_.write( DEFAULT_ORANGE );
        mp_plum_.write( DEFAULT_PLUM );
        m_resetDataCount++;
        return m_resetDataResult;
    }

    bool schemaChange( uint8_t     previousSchemaMajorIndex,
                       uint8_t     previousSchemaMinorIndex,
                       const void* src,
                       size_t      srcLen ) noexcept
    {
        m_schemaChangeCount++;
        return false;
    }

    void dataChanged( Kit::Dm::IModelPoint& point, Kit::Dm::IObserver& observer ) noexcept
    {
        if ( m_dataChangedCount == 0 )
        {
            m_timeChange = Kit::System::ElapsedTime::milliseconds();
        }
        m_dataChangedCount++;
        Record::dataChanged( point, observer );
    }

    void updateNVRAM() noexcept
    {

        m_deltaMs    = Kit::System::ElapsedTime::deltaMilliseconds( m_timeChange );
        m_timeChange = 0;
        m_updateNVRAMCount++;
        Record::updateNVRAM();
    }
};

class MyRecordBadList : public Record
{
public:
    MyRecordBadList( Kit::Persistence::Record::IChunk& chunkHandler, uint8_t major, uint8_t minor ) noexcept
        : Record( nullptr, 1, chunkHandler, major, minor )
    {
    }
    bool resetData() noexcept
    {
        return false;
    }
};

class MyRecordBadList2 : public Record
{
public:
    constexpr static unsigned                       NUM_ITEMS = 3;
    Kit::Dm::ObserverCallback<Kit::Dm::IModelPoint> m_observersList[NUM_ITEMS];
    Item_T                                          m_itemList[NUM_ITEMS];

public:
    MyRecordBadList2( Kit::Persistence::Record::IChunk& chunkHandler, uint8_t major, uint8_t minor ) noexcept
        : Record( m_itemList, NUM_ITEMS, chunkHandler, major, minor )
    {
        m_itemList[0] = { &mp_apple_, &m_observersList[0] };
        m_itemList[1] = { nullptr, &m_observersList[1] };
        m_itemList[2] = { &mp_plum_, nullptr };
    }

    bool resetData() noexcept
    {
        return false;
    }
};

static Kit::Persistence::Record::Media::FileAdapter fd1( FILE_NAME_REGION1, 128 );
static Kit::Checksum::Fletcher16                    crc;
static Kit::Persistence::Record::Chunk::Crc         chunk_( fd1, crc );

// Note: Record::flush()/erase() are synchronous ITC calls, i.e. they can NOT
//       be used with a ManualEventQueue (no thread to process the request)
static bool flushRecord( ManualEventQueue& eventQueue, Record& record )
{
    IManageRequest::FlushPayload payload;
    IManageRequest::FlushMsg     msg( record, payload, eventQueue.nullReturnHandler() );
    return eventQueue.postAndRun( msg ) && payload.m_success;
}

static bool eraseRecord( ManualEventQueue& eventQueue, Record& record )
{
    IManageRequest::ErasePayload payload;
    IManageRequest::EraseMsg     msg( record, payload, eventQueue.nullReturnHandler() );
    return eventQueue.postAndRun( msg ) && payload.m_success;
}

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Record" )
{
    KIT_SYSTEM_TRACE_SCOPE( SECT_, "RECORD Test" );
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();

    MyRecord                           uut( chunk_, 0, 0 );
    Kit::Persistence::Record::IRecord* records[1] = { &uut };
    ManualEventQueue                   eventQueue;  // No thread: the test thread runs the queue
    Kit::Persistence::Record::Server   recordServer( eventQueue, records, sizeof( records ) / sizeof( records[0] ) );

    mp_apple_.setInvalid();
    mp_orange_.setInvalid();
    mp_plum_.setInvalid();

    SECTION( "no persistent data" )
    {
        // Delete files
        Kit::Io::File::System::remove( FILE_NAME_REGION1 );

        size_t recSize = uut.getSize();
        REQUIRE( recSize == EXPECTED_DM_RECORD_METALEN + 5 * 3 + chunk_.getMetadataLength() );

        // No persistent data - and force NO-UPDATE
        uut.m_resetDataResult = false;
        REQUIRE( eventQueue.requestOpen( recordServer ) );
        REQUIRE( uut.m_schemaChangeCount == 0 );
        uint32_t value;
        bool     valid;
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( value == DEFAULT_APPLE );
        valid = mp_orange_.read( value );
        REQUIRE( valid );
        REQUIRE( value == DEFAULT_ORANGE );
        valid = mp_plum_.read( value );
        REQUIRE( valid );
        REQUIRE( value == DEFAULT_PLUM );

        REQUIRE( uut.m_resetDataCount == 1 );
        REQUIRE( uut.m_updateNVRAMCount == 0 );

        // Update my data (will not be written to the file, i.e. the Record did NOT subscribe for changes)
        mp_apple_.write( 11 );
        mp_orange_.write( 12 );
        mp_plum_.write( 13 );
        REQUIRE( eventQueue.runUntilIdle() );
        REQUIRE( uut.m_dataChangedCount == 0 );
        REQUIRE( uut.m_updateNVRAMCount == 0 );

        REQUIRE( eventQueue.requestClose( recordServer ) );
    }

    SECTION( "no persistent data - update with reset/default values" )
    {
        // No persistent data (note: the file contents should still be 'bad')
        REQUIRE( eventQueue.requestOpen( recordServer ) );
        REQUIRE( uut.m_schemaChangeCount == 0 );
        uint32_t value;
        bool     valid;
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( value == DEFAULT_APPLE );
        valid = mp_orange_.read( value );
        REQUIRE( valid );
        REQUIRE( value == DEFAULT_ORANGE );
        valid = mp_plum_.read( value );
        REQUIRE( valid );
        REQUIRE( value == DEFAULT_PLUM );

        REQUIRE( uut.m_resetDataCount == 1 );
        REQUIRE( uut.m_updateNVRAMCount == 1 );  // The defaults are written

        // Update my data -->one NVRAM update per subscribed MP (plum has no subscriber)
        mp_apple_.increment();
        mp_orange_.increment();
        mp_plum_.increment();
        REQUIRE( eventQueue.runUntilIdle() );
        REQUIRE( uut.m_dataChangedCount == 2 );
        REQUIRE( uut.m_updateNVRAMCount == 3 );

        REQUIRE( eventQueue.requestClose( recordServer ) );
    }

    SECTION( "Read" )
    {
        // Previous data
        REQUIRE( eventQueue.requestOpen( recordServer ) );
        REQUIRE( uut.m_schemaChangeCount == 0 );
        uint32_t value;
        bool     valid;
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( value == DEFAULT_APPLE + 1 );
        valid = mp_orange_.read( value );
        REQUIRE( valid );
        REQUIRE( value == DEFAULT_ORANGE + 1 );
        valid = mp_plum_.read( value );
        REQUIRE( valid );
        REQUIRE( value == DEFAULT_PLUM + 1 );

        REQUIRE( uut.m_resetDataCount == 0 );
        REQUIRE( uut.m_updateNVRAMCount == 0 );

        // Update Plum -->but persistent storage should NOT be update (i.e. no subscription to plum)
        mp_plum_.increment();
        REQUIRE( eventQueue.runUntilIdle() );
        REQUIRE( uut.m_dataChangedCount == 0 );
        REQUIRE( uut.m_updateNVRAMCount == 0 );

        REQUIRE( eventQueue.requestClose( recordServer ) );
    }

    SECTION( "Verify No update" )
    {
        // Previous data
        REQUIRE( eventQueue.requestOpen( recordServer ) );
        REQUIRE( uut.m_schemaChangeCount == 0 );
        uint32_t value;
        bool     valid;
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( value == DEFAULT_APPLE + 1 );
        valid = mp_orange_.read( value );
        REQUIRE( valid );
        REQUIRE( value == DEFAULT_ORANGE + 1 );
        valid = mp_plum_.read( value );
        REQUIRE( valid );
        REQUIRE( value == DEFAULT_PLUM + 1 );

        REQUIRE( eventQueue.requestClose( recordServer ) );
    }

    SECTION( "Flush" )
    {
        // Previous data
        REQUIRE( eventQueue.requestOpen( recordServer ) );
        REQUIRE( uut.m_schemaChangeCount == 0 );
        uint32_t value;
        bool     valid;
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( value == DEFAULT_APPLE + 1 );
        valid = mp_orange_.read( value );
        REQUIRE( valid );
        REQUIRE( value == DEFAULT_ORANGE + 1 );
        valid = mp_plum_.read( value );
        REQUIRE( valid );
        REQUIRE( value == DEFAULT_PLUM + 1 );

        // Update Plum -->but persistent storage should NOT be update (i.e. no subscription to plum)
        mp_plum_.increment();
        REQUIRE( eventQueue.runUntilIdle() );
        REQUIRE( uut.m_dataChangedCount == 0 );
        REQUIRE( uut.m_updateNVRAMCount == 0 );

        // Flush the record -->writes the current values (including plum)
        REQUIRE( flushRecord( eventQueue, uut ) );

        REQUIRE( eventQueue.requestClose( recordServer ) );
    }

    SECTION( "Verify Flush update" )
    {
        // Previous data
        REQUIRE( eventQueue.requestOpen( recordServer ) );
        REQUIRE( uut.m_schemaChangeCount == 0 );
        uint32_t value;
        bool     valid;
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( value == DEFAULT_APPLE + 1 );
        valid = mp_orange_.read( value );
        REQUIRE( valid );
        REQUIRE( value == DEFAULT_ORANGE + 1 );
        valid = mp_plum_.read( value );
        REQUIRE( valid );
        REQUIRE( value == DEFAULT_PLUM + 2 );

        REQUIRE( eventQueue.requestClose( recordServer ) );
    }

    SECTION( "Erase" )
    {
        // Previous data
        uut.m_resetDataCount = 0;
        REQUIRE( eventQueue.requestOpen( recordServer ) );
        REQUIRE( uut.m_resetDataCount == 0 );
        REQUIRE( uut.m_schemaChangeCount == 0 );
        uint32_t value;
        bool     valid;
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( value == DEFAULT_APPLE + 1 );
        valid = mp_orange_.read( value );
        REQUIRE( valid );
        REQUIRE( value == DEFAULT_ORANGE + 1 );
        valid = mp_plum_.read( value );
        REQUIRE( valid );
        REQUIRE( value == DEFAULT_PLUM + 2 );

        // Flush the record
        REQUIRE( eraseRecord( eventQueue, uut ) );

        REQUIRE( eventQueue.requestClose( recordServer ) );
    }

    SECTION( "Verify Erase" )
    {
        // Previous data
        uut.m_resetDataCount = 0;
        REQUIRE( eventQueue.requestOpen( recordServer ) );
        REQUIRE( uut.m_resetDataCount == 1 );
        uint32_t value;
        bool     valid;
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( value == DEFAULT_APPLE );
        valid = mp_orange_.read( value );
        REQUIRE( valid );
        REQUIRE( value == DEFAULT_ORANGE );
        valid = mp_plum_.read( value );
        REQUIRE( valid );
        REQUIRE( value == DEFAULT_PLUM );

        REQUIRE( eventQueue.requestClose( recordServer ) );
    }

    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}

TEST_CASE( "Record-badmajor" )
{
    KIT_SYSTEM_TRACE_SCOPE( SECT_, "RECORD2 - bad major index Test" );
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();

    MyRecord                           uut( chunk_, 2, 2 );
    Kit::Persistence::Record::IRecord* records[1] = { &uut };
    ManualEventQueue                   eventQueue;  // No thread: the test thread runs the queue
    Kit::Persistence::Record::Server   recordServer( eventQueue, records, sizeof( records ) / sizeof( records[0] ) );

    mp_apple_.setInvalid();
    mp_orange_.setInvalid();
    mp_plum_.setInvalid();

    SECTION( "Read" )
    {
        // Previous data
        REQUIRE( eventQueue.requestOpen( recordServer ) );
        REQUIRE( uut.m_schemaChangeCount == 1 );
        REQUIRE( uut.m_resetDataCount == 1 );
        uint32_t value;
        bool     valid;
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( value == DEFAULT_APPLE );
        valid = mp_orange_.read( value );
        REQUIRE( valid );
        REQUIRE( value == DEFAULT_ORANGE );
        valid = mp_plum_.read( value );
        REQUIRE( valid );
        REQUIRE( value == DEFAULT_PLUM );

        REQUIRE( uut.m_updateNVRAMCount == 1 );  // The defaults are written

        // Update my data -->one NVRAM update per subscribed MP (plum has no subscriber)
        mp_apple_.increment();
        mp_orange_.increment();
        mp_plum_.increment();
        REQUIRE( eventQueue.runUntilIdle() );
        REQUIRE( uut.m_dataChangedCount == 2 );
        REQUIRE( uut.m_updateNVRAMCount == 3 );

        REQUIRE( eventQueue.requestClose( recordServer ) );
    }

    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}

TEST_CASE( "Record-badminor" )
{
    KIT_SYSTEM_TRACE_SCOPE( SECT_, "RECORD2 - bad minor index Test" );
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();

    MyRecord                           uut( chunk_, 2, 1 );
    Kit::Persistence::Record::IRecord* records[1] = { &uut };
    ManualEventQueue                   eventQueue;  // No thread: the test thread runs the queue
    Kit::Persistence::Record::Server   recordServer( eventQueue, records, sizeof( records ) / sizeof( records[0] ) );

    mp_apple_.setInvalid();
    mp_orange_.setInvalid();
    mp_plum_.setInvalid();

    SECTION( "Read" )
    {
        // Previous data
        REQUIRE( eventQueue.requestOpen( recordServer ) );
        REQUIRE( uut.m_schemaChangeCount == 1 );
        REQUIRE( uut.m_resetDataCount == 1 );
        uint32_t value;
        bool     valid;
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( value == DEFAULT_APPLE );
        valid = mp_orange_.read( value );
        REQUIRE( valid );
        REQUIRE( value == DEFAULT_ORANGE );
        valid = mp_plum_.read( value );
        REQUIRE( valid );
        REQUIRE( value == DEFAULT_PLUM );

        REQUIRE( uut.m_updateNVRAMCount == 1 );  // The defaults are written

        // Update my data -->one NVRAM update per subscribed MP (plum has no subscriber)
        mp_apple_.increment();
        mp_orange_.increment();
        mp_plum_.increment();
        REQUIRE( eventQueue.runUntilIdle() );
        REQUIRE( uut.m_dataChangedCount == 2 );
        REQUIRE( uut.m_updateNVRAMCount == 3 );

        REQUIRE( eventQueue.requestClose( recordServer ) );
    }

    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}


TEST_CASE( "Record-verify" )
{
    KIT_SYSTEM_TRACE_SCOPE( SECT_, "RECORD2 - bad major index Test" );
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();

    MyRecord                           uut( chunk_, 2, 1 );
    Kit::Persistence::Record::IRecord* records[1] = { &uut };
    ManualEventQueue                   eventQueue;  // No thread: the test thread runs the queue
    Kit::Persistence::Record::Server   recordServer( eventQueue, records, sizeof( records ) / sizeof( records[0] ) );

    mp_apple_.setInvalid();
    mp_orange_.setInvalid();
    mp_plum_.setInvalid();

    SECTION( "Read" )
    {
        // Previous data
        REQUIRE( eventQueue.requestOpen( recordServer ) );
        REQUIRE( uut.m_schemaChangeCount == 0 );
        REQUIRE( uut.m_resetDataCount == 0 );
        uint32_t value;
        bool     valid;
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( value == DEFAULT_APPLE + 1 );
        valid = mp_orange_.read( value );
        REQUIRE( valid );
        REQUIRE( value == DEFAULT_ORANGE + 1 );
        valid = mp_plum_.read( value );
        REQUIRE( valid );
        REQUIRE( value == DEFAULT_PLUM + 1 );

        REQUIRE( eventQueue.requestClose( recordServer ) );
    }

    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}

TEST_CASE( "NVRAM update delayed" )
{
    KIT_SYSTEM_TRACE_SCOPE( SECT_, "RECORD - delayed NVRAM update Test" );
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();

    // Delete files
    Kit::Io::File::System::remove( FILE_NAME_REGION1 );

    mp_apple_.setInvalid();
    mp_orange_.setInvalid();
    mp_plum_.setInvalid();

    // NOTE: The write delay timer runs on the queue's virtual clock (i.e. it is
    //       deterministic), whereas the maximum delay is measured with the
    //       (real) system clock.
    SECTION( "each change restarts the write delay" )
    {
        MyRecord                           uut( chunk_, 0, 0, 700, 60 * 60 * 1000 );  // The max delay never expires
        Kit::Persistence::Record::IRecord* records[1] = { &uut };
        ManualEventQueue                   eventQueue;  // No thread: the test thread runs the queue
        Kit::Persistence::Record::Server   recordServer( eventQueue, records, sizeof( records ) / sizeof( records[0] ) );

        // Start the record (no persistent data -->the defaults are written immediately)
        REQUIRE( eventQueue.requestOpen( recordServer ) );
        REQUIRE( uut.m_updateNVRAMCount == 1 );

        mp_apple_.increment();
        REQUIRE( eventQueue.runUntilIdle() );
        REQUIRE( uut.m_dataChangedCount == 1 );
        REQUIRE( eventQueue.advanceTime( 699 ) );
        REQUIRE( uut.m_updateNVRAMCount == 1 );  // Not yet

        // A change restarts the delay
        mp_orange_.increment();
        REQUIRE( eventQueue.advanceTime( 699 ) );
        REQUIRE( uut.m_dataChangedCount == 2 );
        REQUIRE( uut.m_updateNVRAMCount == 1 );  // Not yet

        // Plum has no subscriber -->does NOT restart the delay
        mp_plum_.increment();
        REQUIRE( eventQueue.advanceTime( 1 ) );
        REQUIRE( uut.m_dataChangedCount == 2 );
        REQUIRE( uut.m_updateNVRAMCount == 2 );  // ONE update for both changes

        // Nothing else happens
        REQUIRE( eventQueue.advanceTime( 5000 ) );
        REQUIRE( uut.m_updateNVRAMCount == 2 );

        REQUIRE( eventQueue.requestClose( recordServer ) );
    }

    SECTION( "the maximum delay forces an immediate update" )
    {
        MyRecord                           uut( chunk_, 0, 0, 700, 50 );
        Kit::Persistence::Record::IRecord* records[1] = { &uut };
        ManualEventQueue                   eventQueue;  // No thread: the test thread runs the queue
        Kit::Persistence::Record::Server   recordServer( eventQueue, records, sizeof( records ) / sizeof( records[0] ) );

        REQUIRE( eventQueue.requestOpen( recordServer ) );
        REQUIRE( uut.m_updateNVRAMCount == 1 );

        // First change -->starts the delay (the max delay starts 'now')
        mp_apple_.increment();
        REQUIRE( eventQueue.runUntilIdle() );
        REQUIRE( uut.m_updateNVRAMCount == 1 );

        // Let (at least) the maximum delay elapse - but NOT the write delay
        Kit::System::sleep( 60 );
        mp_orange_.increment();
        REQUIRE( eventQueue.runUntilIdle() );
        REQUIRE( uut.m_dataChangedCount == 2 );
        REQUIRE( uut.m_updateNVRAMCount == 2 );

        // The pending (delayed) update was cancelled -->no additional update
        REQUIRE( eventQueue.advanceTime( 5000 ) );
        REQUIRE( uut.m_updateNVRAMCount == 2 );

        REQUIRE( eventQueue.requestClose( recordServer ) );
    }

    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}
