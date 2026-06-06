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
#include "Kit/EventQueue/Server.h"
#include "Kit/System/ElapsedTime.h"
#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include "catch2/catch_test_macros.hpp"
#include "Kit/System/Trace.h"
#include "Kit/System/Thread.h"
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

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Record" )
{
    KIT_SYSTEM_TRACE_SCOPE( SECT_, "RECORD Test" );
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();

    MyRecord                           uut( chunk_, 0, 0 );
    Kit::Persistence::Record::IRecord* records[1] = { &uut };
    Kit::EventQueue::Server            eventQueue;
    Kit::Persistence::Record::Server   recordServer( eventQueue, records, sizeof( records ) / sizeof( records[0] ) );
    Kit::System::Thread*               t1 = Kit::System::Thread::create( eventQueue, "UUT" );
    REQUIRE( t1 );

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
        recordServer.open();
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

        // Update my data (will not be written to the file)
        mp_apple_.write( 11 );
        mp_orange_.write( 12 );
        mp_plum_.write( 13 );


        // Allow time for the changes to propagate and the data to be saved
        Kit::System::sleep( 1000 );

        recordServer.close();
    }

    SECTION( "no persistent data - update with reset/default values" )
    {
        // No persistent data (note: the file contents should still be 'bad')
        recordServer.open();
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

        // Update my data
        mp_apple_.increment();
        mp_orange_.increment();
        mp_plum_.increment();

        // Allow time for the changes to propagate and the data to be saved
        Kit::System::sleep( 1000 );

        recordServer.close();
    }

    SECTION( "Read" )
    {
        // Previous data
        recordServer.open();
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

        // Allow time for the changes to propagate and the data to be saved - WHICH won't happen beaus plum does not trigger a change notification
        Kit::System::sleep( 1000 );

        recordServer.close();
    }

    SECTION( "Verify No update" )
    {
        // Previous data
        recordServer.open();
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

        recordServer.close();
    }

    SECTION( "Flush" )
    {
        // Previous data
        recordServer.open();
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

        // Flush the record
        REQUIRE( uut.flush() );

        // Allow time for the changes to propagate and the data to be saved
        Kit::System::sleep( 1000 );

        recordServer.close();
    }

    SECTION( "Verify Flush update" )
    {
        // Previous data
        recordServer.open();
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

        recordServer.close();
    }

    SECTION( "Erase" )
    {
        // Previous data
        uut.m_resetDataCount = 0;
        recordServer.open();
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
        REQUIRE( uut.erase() );

        recordServer.close();
    }

    SECTION( "Verify Erase" )
    {
        // Previous data
        uut.m_resetDataCount = 0;
        recordServer.open();
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

        recordServer.close();
    }

    Kit::System::Thread::destroy( *t1 );
    Kit::System::sleep( 100 );
    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}

TEST_CASE( "Record-badmajor" )
{
    KIT_SYSTEM_TRACE_SCOPE( SECT_, "RECORD2 - bad major index Test" );
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();

    MyRecord                           uut( chunk_, 2, 2 );
    Kit::Persistence::Record::IRecord* records[1] = { &uut };
    Kit::EventQueue::Server            eventQueue;
    Kit::Persistence::Record::Server   recordServer( eventQueue, records, sizeof( records ) / sizeof( records[0] ) );
    Kit::System::Thread*               t1 = Kit::System::Thread::create( eventQueue, "UUT2" );
    REQUIRE( t1 );

    mp_apple_.setInvalid();
    mp_orange_.setInvalid();
    mp_plum_.setInvalid();

    SECTION( "Read" )
    {
        // Previous data
        recordServer.open();
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

        // Update Apple -->but persistent storage should NOT be update (i.e. no subscription to plum)
        mp_apple_.increment();
        mp_orange_.increment();
        mp_plum_.increment();

        // Allow time for the changes to propagate and the data to be saved
        Kit::System::sleep( 1000 );

        recordServer.close();
    }

    Kit::System::Thread::destroy( *t1 );
    Kit::System::sleep( 100 );
    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}

TEST_CASE( "Record-badminor" )
{
    KIT_SYSTEM_TRACE_SCOPE( SECT_, "RECORD2 - bad minor index Test" );
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();

    MyRecord                           uut( chunk_, 2, 1 );
    Kit::Persistence::Record::IRecord* records[1] = { &uut };
    Kit::EventQueue::Server            eventQueue;
    Kit::Persistence::Record::Server   recordServer( eventQueue, records, sizeof( records ) / sizeof( records[0] ) );
    Kit::System::Thread*               t1 = Kit::System::Thread::create( eventQueue, "UUT2" );
    REQUIRE( t1 );

    mp_apple_.setInvalid();
    mp_orange_.setInvalid();
    mp_plum_.setInvalid();

    SECTION( "Read" )
    {
        // Previous data
        recordServer.open();
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

        // Update Apple -->but persistent storage should NOT be update (i.e. no subscription to plum)
        mp_apple_.increment();
        mp_orange_.increment();
        mp_plum_.increment();

        // Allow time for the changes to propagate and the data to be saved
        Kit::System::sleep( 1000 );

        recordServer.close();
    }

    Kit::System::Thread::destroy( *t1 );
    Kit::System::sleep( 100 );
    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}


TEST_CASE( "Record-verify" )
{
    KIT_SYSTEM_TRACE_SCOPE( SECT_, "RECORD2 - bad major index Test" );
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();

    MyRecord                           uut( chunk_, 2, 1 );
    Kit::Persistence::Record::IRecord* records[1] = { &uut };
    Kit::EventQueue::Server            eventQueue;
    Kit::Persistence::Record::Server   recordServer( eventQueue, records, sizeof( records ) / sizeof( records[0] ) );
    Kit::System::Thread*               t1 = Kit::System::Thread::create( eventQueue, "UUT2" );
    REQUIRE( t1 );

    mp_apple_.setInvalid();
    mp_orange_.setInvalid();
    mp_plum_.setInvalid();

    SECTION( "Read" )
    {
        // Previous data
        recordServer.open();
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

        recordServer.close();
    }

    Kit::System::Thread::destroy( *t1 );
    Kit::System::sleep( 100 );
    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}

TEST_CASE( "NVRAM update delayed" )
{
    KIT_SYSTEM_TRACE_SCOPE( SECT_, "RECORD Test" );
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();

    // Delete files
    Kit::Io::File::System::remove( FILE_NAME_REGION1 );

    Kit::System::ShutdownUnitTesting::clearAndUseCounter();

    MyRecord                           uut( chunk_, 0, 0, 700, 1000 );
    Kit::Persistence::Record::IRecord* records[1] = { &uut };
    Kit::EventQueue::Server            eventQueue;
    Kit::Persistence::Record::Server   recordServer( eventQueue, records, sizeof( records ) / sizeof( records[0] ) );
    Kit::System::Thread*               t1 = Kit::System::Thread::create( eventQueue, "UUT2" );
    REQUIRE( t1 );

    mp_apple_.setInvalid();
    mp_orange_.setInvalid();
    mp_plum_.setInvalid();


    SECTION( "update with reset/default values" )
    {
        // Start the record
        recordServer.open();
        REQUIRE( uut.m_updateNVRAMCount == 1 );
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

        // Update my data - with delays between the updates
        mp_apple_.increment();
        mp_orange_.increment();
        mp_plum_.increment();
        Kit::System::sleep( 100 );
        mp_apple_.increment();
        Kit::System::sleep( 100 );
        mp_plum_.increment();
        Kit::System::sleep( 100 );
        mp_orange_.increment();
        Kit::System::sleep( 100 );
        mp_apple_.increment();
        Kit::System::sleep( 100 );
        mp_plum_.increment();
        Kit::System::sleep( 100 );
        mp_orange_.increment();

        // Allow time for the changes to propagate and the data to be saved
        Kit::System::sleep( 1000 + 300 );
        REQUIRE( uut.m_dataChangedCount >= 3 );
        REQUIRE( uut.m_updateNVRAMCount == 2 );
        REQUIRE( uut.m_deltaMs > 700 );

        recordServer.close();
    }
    Kit::System::Thread::destroy( *t1 );
    Kit::System::sleep( 100 );  // allow time for thread(s) to stop BEFORE the runnable object goes out of scope
    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}

TEST_CASE( "Errors" )
{
    KIT_SYSTEM_TRACE_SCOPE( SECT_, "Errors" );
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();

    SECTION( "bad list pointer" )
    {
        MyRecordBadList uut( chunk_, 2, 1 );
        REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 1u );
    }

    SECTION( "bad MP pointer" )
    {
        MyRecordBadList2        uut( chunk_, 2, 1 );
        Kit::EventQueue::Server eventQueue;
        uut.start( eventQueue );
        REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 1u );
    }
}
