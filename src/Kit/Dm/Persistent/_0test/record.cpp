/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Catch/catch.hpp"
#include "Kit/System/_testsupport/Shutdown_TS.h"
#include "Kit/System/Trace.h"
#include "Kit/System/Thread.h"
#include "Kit/System/Api.h"
#include "Kit/Dm/ModelDatabase.h"
#include "Kit/Dm/Persistent/Record.h"
#include "Kit/Dm/Mp/Uint32.h"
#include "Kit/Persistent/RecordServer.h"
#include "Kit/Persistent/MirroredChunk.h"
#include "Kit/Persistent/FileAdapter.h"
#include "Kit/Io/File/Api.h"
#include "Kit/Itc/SyncReturnHandler.h"


#define SECT_   "_0test"

#define FILE_NAME_REGION1   "region1.nvram"
#define FILE_NAME_REGION2   "region2.nvram"

using namespace Kit::Dm::Persistent;

// Allocate/create my Model Database
static Kit::Dm::ModelDatabase    modelDb_( "ignoreThisParameter_usedToInvokeTheStaticConstructor" );

// Allocate my Model Points
static Kit::Dm::Mp::Uint32       mp_apple_( modelDb_, "APPLE1" );
static Kit::Dm::Mp::Uint32       mp_orange_( modelDb_, "ORANGE1" );
static Kit::Dm::Mp::Uint32       mp_cherry_( modelDb_, "CHERRY1" );
static Kit::Dm::Mp::Uint32       mp_plum_( modelDb_, "PLUM1" );

#define DEFAULT_APPLE        0xAAAA5555 
#define DEFAULT_ORANGE       0xBBBB7777
#define DEFAULT_PLUM         0xFFFF9999
#define DEFAULT_CHERRY       0xDDDD3333


class MyRecord : public Record
{
public:
    MyRecord( Kit::Persistent::Chunk& chunkHandler, uint8_t major, uint8_t minor, uint32_t delayMs=0, uint32_t maxDelayMs=0 ) noexcept
        : Record( m_itemList, chunkHandler, major, minor, delayMs, maxDelayMs )
        , m_resetDataCount( 0 )
        , m_schemaChangeCount( 0 )
        , m_resetDataResult( true )
        , m_dataChangedCount( 0 )
        , m_updateNVRAMCount( 0 )
        , m_deltaMs( 0 )
        , m_timeChange( 0 )
    {
        m_itemList[0] ={ &mp_apple_, CPL_DM_PERISTENCE_RECORD_USE_SUBSCRIBER };
        m_itemList[1] ={ &mp_orange_, CPL_DM_PERISTENCE_RECORD_USE_SUBSCRIBER };
        m_itemList[2] ={ &mp_plum_, CPL_DM_PERISTENCE_RECORD_NO_SUBSCRIBER };
        m_itemList[3] ={ 0,0 };
    }

    bool resetData() noexcept
    {
        mp_apple_.write( DEFAULT_APPLE );
        mp_orange_.write( DEFAULT_ORANGE );
        mp_plum_.write( DEFAULT_PLUM );
        m_resetDataCount++;
        return m_resetDataResult;
    }

    bool schemaChange( uint8_t      previousSchemaMajorIndex,
                       uint8_t      previousSchemaMinorIndex,
                       const void*  src,
                       size_t       srcLen ) noexcept
    {
        m_schemaChangeCount++;
        return false;
    }

    void dataChanged( Kit::Dm::ModelPoint& point, Kit::Dm::SubscriberApi& observer ) noexcept
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


public:

    Item_T m_itemList[3 + 1];
    int m_resetDataCount;
    int m_schemaChangeCount;
    bool m_resetDataResult;
    int m_dataChangedCount;
    int m_updateNVRAMCount;
    uint32_t m_deltaMs;
    uint32_t m_timeChange;
};



static Kit::Persistent::FileAdapter fd1( FILE_NAME_REGION1, 0, 128 );
static Kit::Persistent::FileAdapter fd2( FILE_NAME_REGION2, 0, 128 );
static Kit::Persistent::MirroredChunk chunk( fd1, fd2 );

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "record" )
{
    KIT_SYSTEM_TRACE_SCOPE( SECT_, "RECORD Test" );
    Kit::System::Shutdown_TS::clearAndUseCounter();

    MyRecord uut( chunk, 0, 0 );
    Kit::Persistent::Record* records[2] ={ &uut, 0 };

    Kit::Persistent::RecordServer recordServer( records );
    Kit::System::Thread* t1 = Kit::System::Thread::create( recordServer, "UUT" );
    REQUIRE( t1 );

    mp_apple_.setInvalid();
    mp_orange_.setInvalid();
    mp_plum_.setInvalid();

    SECTION( "no persistent data" )
    {
        // Delete files
        Kit::Io::File::Api::remove( FILE_NAME_REGION1 );
        Kit::Io::File::Api::remove( FILE_NAME_REGION2 );

        size_t recSize = uut.getRecordSize();
        REQUIRE( recSize == 2 + 5 * 3 );

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
        Kit::System::Api::sleep( 1000 );

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
        Kit::System::Api::sleep( 1000 );

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
        Kit::System::Api::sleep( 1000 );

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
        REQUIRE( uut.flush( recordServer ) );

        // Allow time for the changes to propagate and the data to be saved 
        Kit::System::Api::sleep( 1000 );

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
        REQUIRE( uut.erase( recordServer ) );

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
    Kit::System::Api::sleep( 1000 );
    REQUIRE( Kit::System::Shutdown_TS::getAndClearCounter() == 0u );
}


TEST_CASE( "record-badmajor" )
{
    KIT_SYSTEM_TRACE_SCOPE( SECT_, "RECORD2 - bad major index Test" );
    Kit::System::Shutdown_TS::clearAndUseCounter();

    MyRecord uut( chunk, 2, 2 );
    Kit::Persistent::Record* records[2] ={ &uut, 0 };

    Kit::Persistent::RecordServer recordServer( records );
    Kit::System::Thread* t1 = Kit::System::Thread::create( recordServer, "UUT2" );
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
        Kit::System::Api::sleep( 1000 );

        recordServer.close();
    }

    Kit::System::Thread::destroy( *t1 );
    Kit::System::Api::sleep( 1000 );
    REQUIRE( Kit::System::Shutdown_TS::getAndClearCounter() == 0u );
}

TEST_CASE( "record-badminor" )
{
    KIT_SYSTEM_TRACE_SCOPE( SECT_, "RECORD2 - bad major index Test" );
    Kit::System::Shutdown_TS::clearAndUseCounter();

    MyRecord uut( chunk, 2, 1 );
    Kit::Persistent::Record* records[2] ={ &uut, 0 };

    Kit::Persistent::RecordServer recordServer( records );
    Kit::System::Thread* t1 = Kit::System::Thread::create( recordServer, "UUT2" );
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
        Kit::System::Api::sleep( 1000 );

        recordServer.close();
    }

    Kit::System::Thread::destroy( *t1 );
    Kit::System::Api::sleep( 1000 );
    REQUIRE( Kit::System::Shutdown_TS::getAndClearCounter() == 0u );
}



TEST_CASE( "record-verify" )
{
    KIT_SYSTEM_TRACE_SCOPE( SECT_, "RECORD2 - bad major index Test" );
    Kit::System::Shutdown_TS::clearAndUseCounter();

    MyRecord uut( chunk, 2, 1 );
    Kit::Persistent::Record* records[2] ={ &uut, 0 };

    Kit::Persistent::RecordServer recordServer( records );
    Kit::System::Thread* t1 = Kit::System::Thread::create( recordServer, "UUT2" );
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
    Kit::System::Api::sleep( 1000 );
    REQUIRE( Kit::System::Shutdown_TS::getAndClearCounter() == 0u );
}

TEST_CASE( "nvram update delayed" )
{
    KIT_SYSTEM_TRACE_SCOPE( SECT_, "RECORD Test" );
    Kit::System::Shutdown_TS::clearAndUseCounter();

    // Delete files
    Kit::Io::File::Api::remove( FILE_NAME_REGION1 );
    Kit::Io::File::Api::remove( FILE_NAME_REGION2 );

    MyRecord uut( chunk, 0, 0, 700, 1000 );
    Kit::Persistent::Record* records[2] ={ &uut, 0 };

    Kit::Persistent::RecordServer recordServer( records );
    Kit::System::Thread* t1 = Kit::System::Thread::create( recordServer, "UUT" );
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
        Kit::System::Api::sleep( 100 );
        mp_apple_.increment();
        Kit::System::Api::sleep( 100 );
        mp_plum_.increment();
        Kit::System::Api::sleep( 100 );
        mp_orange_.increment();
        Kit::System::Api::sleep( 100 );
        mp_apple_.increment();
        Kit::System::Api::sleep( 100 );
        mp_plum_.increment();
        Kit::System::Api::sleep( 100 );
        mp_orange_.increment();

        // Allow time for the changes to propagate and the data to be saved
        Kit::System::Api::sleep( 1000+300 );
        REQUIRE( uut.m_dataChangedCount >= 3 );
        REQUIRE( uut.m_updateNVRAMCount == 2 ); 
        REQUIRE( uut.m_deltaMs > 700 );

        recordServer.close();
    }

    Kit::System::Thread::destroy( *t1 );
    Kit::System::Api::sleep( 1000 );
    REQUIRE( Kit::System::Shutdown_TS::getAndClearCounter() == 0u );
}