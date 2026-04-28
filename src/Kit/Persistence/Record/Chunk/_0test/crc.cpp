/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "catch2/catch_test_macros.hpp"
#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include "Kit/System/Trace.h"
#include "Kit/System/Assert.h"
#include "Kit/Persistence/Record/Chunk/Crc.h"
#include "Kit/Checksum/Fletcher16.h"
#include "Kit/Checksum/Crc32EthernetFast.h"
#include "Kit/Persistence/Record/Media/FileAdapter.h"
#include "Kit/EventQueue/Server.h"
#include "Kit/Io/File/System.h"
#include "Kit/Io/File/Output.h"

#define SECT_ "_0test"

///
using namespace Kit::Persistence::Record::Chunk;
using namespace Kit::Persistence::Record;
using namespace Kit::Persistence;


////////////////////////////////////////////////////////////////////////////////
namespace {

class MyUut : public Crc
{
public:
    int m_startCount;
    int m_stopCount;

    MyUut( IMedia&              media,
           Kit::Checksum::IEdc& edc )
        : Crc( media, edc )
        , m_startCount( 0 )
        , m_stopCount( 0 )
    {
    }

    void start( Kit::EventQueue::IQueue& myEventQueue ) noexcept
    {
        Crc::start( myEventQueue );
        m_startCount++;
    }

    /// See Cpl::Persistent::Chunk
    void stop() noexcept
    {
        Crc::stop();
        m_stopCount++;
    }
};

class MyPayload : public IPayload
{
public:
    char        m_buffer[1024];
    int         m_getCount;
    int         m_putCount;
    bool        m_putResult;
    const char* m_getString;

    MyPayload( const char* getString )
        : m_getCount( 0 )
        , m_putCount( 0 )
        , m_putResult( true )
        , m_getString( getString )
    {
    }

    Size_T copyTo( void* dst, Size_T maxDstLen ) noexcept override
    {
        Size_T len = strlen( m_getString ) + 1;
        KIT_SYSTEM_ASSERT( maxDstLen >= len );
        m_getCount++;

        memcpy( dst, m_getString, len );
        return len;
    }

    bool copyFrom( const void* src, Size_T srcLen ) noexcept override
    {
        KIT_SYSTEM_ASSERT( srcLen <= sizeof( m_buffer ) );
        m_putCount++;
        memcpy( m_buffer, src, srcLen );
        return m_putResult;
    };
};


}  // end anonymous namespace

static MyPayload payload1_( "Hello" );
static MyPayload payload2_( "World" );

#define MEDIA_FILE_NAME  "media.bin"
#define MEDIA_FILE_NAME2 "media2.bin"
#define MEDIA_MAX_SIZE   128

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "CrcChunk" )
{
    KIT_SYSTEM_TRACE_SCOPE( SECT_, "CRC test" );
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();

    Kit::EventQueue::Server   mockEventQueue;
    Media::FileAdapter        fd1( MEDIA_FILE_NAME, MEDIA_MAX_SIZE );
    Kit::Checksum::Fletcher16 crc;
    MyUut                     uut( fd1, crc );
    payload1_.m_putCount = 0;
    payload1_.m_getCount = 0;
    payload2_.m_putCount = 0;
    payload2_.m_getCount = 0;

    SECTION( "start/stop" )
    {
        REQUIRE( uut.m_startCount == 0 );
        REQUIRE( uut.m_stopCount == 0 );
        uut.start( mockEventQueue );
        REQUIRE( uut.m_startCount == 1 );
        REQUIRE( uut.m_stopCount == 0 );
        uut.stop();
        REQUIRE( uut.m_startCount == 1 );
        REQUIRE( uut.m_stopCount == 1 );

        REQUIRE( uut.getMetadataLength() == ( sizeof( Size_T ) + crc.getEdcSize() ) );  // Expected: Data Len Field size + CRC size
    }

    SECTION( "Load/update" )
    {
        // Delete files
        Kit::Io::File::System::remove( MEDIA_FILE_NAME );
        uut.start( mockEventQueue );

        bool result = uut.loadData( payload1_ );
        REQUIRE( result == false );
        REQUIRE( payload1_.m_putCount == 0 );

        REQUIRE( payload1_.m_getCount == 0 );
        uut.updateData( payload1_ );
        REQUIRE( payload1_.m_getCount == 1 );

        result = uut.loadData( payload1_ );
        REQUIRE( result == true );
        REQUIRE( payload1_.m_putCount == 1 );
        REQUIRE( strcmp( payload1_.m_buffer, payload1_.m_getString ) == 0 );

        REQUIRE( payload2_.m_getCount == 0 );
        uut.updateData( payload2_ );
        REQUIRE( payload2_.m_getCount == 1 );
        REQUIRE( payload1_.m_putCount == 1 );

        result = uut.loadData( payload2_ );
        REQUIRE( result == true );
        REQUIRE( payload2_.m_putCount == 1 );
        REQUIRE( payload1_.m_putCount == 1 );
        printf( "buffer=[%s], expected=[%s]\n", payload2_.m_buffer, payload2_.m_getString );
        REQUIRE( strcmp( payload2_.m_buffer, payload2_.m_getString ) == 0 );

        uut.stop();
    }


    SECTION( "corrupt CRC" )
    {
        uut.start( mockEventQueue );

        // Read the data
        bool result = uut.loadData( payload2_ );
        REQUIRE( result == true );
        REQUIRE( payload2_.m_getCount == 0 );
        REQUIRE( payload2_.m_putCount == 1 );
        REQUIRE( strcmp( payload2_.m_buffer, payload2_.m_getString ) == 0 );

        // Corrupt region 2
        Kit::Io::File::Output fd( MEDIA_FILE_NAME );
        REQUIRE( fd.isOpened() );
        fd.write( "junk" );
        fd.close();

        // Read the data (should be bad)
        result = uut.loadData( payload2_ );
        REQUIRE( result == false );
        REQUIRE( payload2_.m_getCount == 0 );
        REQUIRE( payload2_.m_putCount == 1 );

        uut.stop();
    }

    SECTION( "Erase" )
    {
        // Delete files
        Kit::Io::File::System::remove( MEDIA_FILE_NAME );
        payload1_.m_putCount = 0;
        payload1_.m_getCount = 0;
        uut.start( mockEventQueue );

        bool result = uut.loadData( payload1_ );
        REQUIRE( result == false );
        REQUIRE( payload1_.m_putCount == 0 );

        REQUIRE( payload1_.m_getCount == 0 );
        uut.updateData( payload1_ );
        REQUIRE( payload1_.m_getCount == 1 );

        result = uut.loadData( payload1_ );
        REQUIRE( result == true );
        REQUIRE( payload1_.m_putCount == 1 );
        REQUIRE( strcmp( payload1_.m_buffer, payload1_.m_getString ) == 0 );

        REQUIRE( payload1_.m_getCount == 1 );
        uut.updateData( payload1_, 0, true );  // Erase the record
        REQUIRE( payload1_.m_getCount == 2 );

        result = uut.loadData( payload1_ );
        REQUIRE( result == false );
        REQUIRE( payload1_.m_putCount == 1 );

        uut.stop();
    }

    SECTION( "different CRC" )
    {
        // Delete files
        Kit::Io::File::System::remove( MEDIA_FILE_NAME2 );

        Kit::Checksum::Crc32EthernetFast crc2;
        Media::FileAdapter               fd2( MEDIA_FILE_NAME2, MEDIA_MAX_SIZE );
        MyUut                            uut2( fd2, crc2 );
        REQUIRE( uut2.getMetadataLength() == ( sizeof( Size_T ) + crc2.getEdcSize() ) );  // Expected: Data Len Field size + CRC size

        uut2.start( mockEventQueue );

        bool result = uut2.loadData( payload1_ );
        REQUIRE( result == false );
        REQUIRE( payload1_.m_putCount == 0 );

        REQUIRE( payload1_.m_getCount == 0 );
        uut2.updateData( payload1_ );
        REQUIRE( payload1_.m_getCount == 1 );

        result = uut2.loadData( payload1_ );
        REQUIRE( result == true );
        REQUIRE( payload1_.m_putCount == 1 );
        REQUIRE( strcmp( payload1_.m_buffer, payload1_.m_getString ) == 0 );

        REQUIRE( payload2_.m_getCount == 0 );
        uut2.updateData( payload2_ );
        REQUIRE( payload2_.m_getCount == 1 );
        REQUIRE( payload1_.m_putCount == 1 );

        result = uut2.loadData( payload2_ );
        REQUIRE( result == true );
        REQUIRE( payload2_.m_putCount == 1 );
        REQUIRE( payload1_.m_putCount == 1 );
        printf( "buffer=[%s], expected=[%s]\n", payload2_.m_buffer, payload2_.m_getString );
        REQUIRE( strcmp( payload2_.m_buffer, payload2_.m_getString ) == 0 );

        uut2.stop();
    }


    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}