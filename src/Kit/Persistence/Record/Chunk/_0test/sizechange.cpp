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
#include "Kit/Persistence/Record/Chunk/Crc.h"
#include "Kit/Persistence/Record/Chunk/Mirrored.h"
#include "Kit/Checksum/Fletcher16.h"
#include "Kit/EventQueue/Server.h"
#include <string.h>

#define SECT_ "_0test"

///
using namespace Kit::Persistence::Record::Chunk;
using namespace Kit::Persistence::Record;
using namespace Kit::Persistence;


////////////////////////////////////////////////////////////////////////////////
namespace {

constexpr Size_T RAM_MEDIA_SIZE = 128;

// Number of bytes returned by a 'short' read fault
constexpr Size_T SHORT_READ_LEN = 2;

struct Counts_T
{
    unsigned reads  = 0;
    unsigned writes = 0;
};

/** RAM based media that counts the IMedia read/write calls. Faults are
    injected per call number: bit N-1 of a mask applies to the N-th call
    (counted since the last arm()).
 */
class RamMedia : public IMedia
{
public:
    uint8_t   m_data[RAM_MEDIA_SIZE];
    Counts_T& m_counts;
    uint32_t  m_failReads;
    uint32_t  m_shortReads;
    uint32_t  m_failWrites;
    unsigned  m_numReads;
    unsigned  m_numWrites;

    explicit RamMedia( Counts_T& counts )
        : m_counts( counts )
    {
        memset( m_data, 0xFF, sizeof( m_data ) );  // Erased state
        arm( 0, 0, 0 );
    }

    void arm( uint32_t failReads, uint32_t shortReads, uint32_t failWrites )
    {
        m_failReads  = failReads;
        m_shortReads = shortReads;
        m_failWrites = failWrites;
        m_numReads   = 0;
        m_numWrites  = 0;
    }

    bool start( Kit::EventQueue::IQueue& myEventQueue ) noexcept override { return true; }
    void stop() noexcept override {}

    bool write( Size_T offset, const void* srcData, Size_T srcLen ) noexcept override
    {
        m_counts.writes++;
        if ( isArmed( m_failWrites, m_numWrites++ ) || srcLen > RAM_MEDIA_SIZE || offset > RAM_MEDIA_SIZE - srcLen )
        {
            return false;
        }
        memcpy( m_data + offset, srcData, srcLen );
        return true;
    }

    // Reads past the end of the media are errors (same as the FileAdapter and NVAdapter)
    Size_T read( Size_T offset, void* dstBuffer, Size_T bytesToRead ) noexcept override
    {
        m_counts.reads++;
        unsigned callIdx = m_numReads++;
        if ( isArmed( m_failReads, callIdx ) || bytesToRead > RAM_MEDIA_SIZE || offset > RAM_MEDIA_SIZE - bytesToRead )
        {
            return KIT_PERSISTENCE_SIZE_MAX;
        }
        if ( isArmed( m_shortReads, callIdx ) && bytesToRead > SHORT_READ_LEN )
        {
            bytesToRead = SHORT_READ_LEN;
        }
        memcpy( dstBuffer, m_data + offset, bytesToRead );
        return bytesToRead;
    }

    Size_T getMaxSize() const noexcept override { return RAM_MEDIA_SIZE; }

    static bool isArmed( uint32_t mask, unsigned callIdx )
    {
        return callIdx < 32 && ( mask & ( 1u << callIdx ) ) != 0;
    }
};

/// Payload whose size is set by the 'current firmware'
class SizedPayload : public IPayload
{
public:
    Size_T  m_size;
    uint8_t m_fill;
    uint8_t m_loaded[RAM_MEDIA_SIZE];
    Size_T  m_loadedLen;

    SizedPayload( Size_T size, uint8_t fill )
        : m_size( size ), m_fill( fill ), m_loadedLen( 0 )
    {
    }

    Size_T copyTo( void* dst, Size_T maxDstLen ) noexcept override
    {
        if ( maxDstLen < m_size )
        {
            return KIT_PERSISTENCE_SIZE_MAX;
        }
        memset( dst, m_fill, m_size );
        return m_size;
    }

    bool copyFrom( const void* src, Size_T srcLen ) noexcept override
    {
        if ( srcLen > sizeof( m_loaded ) )
        {
            return false;
        }
        memcpy( m_loaded, src, srcLen );
        m_loadedLen = srcLen;
        return true;
    }

    Size_T getMaxPayloadSize() const noexcept override { return m_size; }

    bool loadedMatches( Size_T len, uint8_t fill ) const
    {
        if ( m_loadedLen != len )
        {
            return false;
        }
        for ( Size_T i = 0; i < len; i++ )
        {
            if ( m_loaded[i] != fill )
            {
                return false;
            }
        }
        return true;
    }
};

/** Simulates a boot cycle with 'newSize' as the record's current size: loads
    the previously stored record, verifies the number of IMedia reads, and then
    re-writes the record using the new size.
 */
void bootCycle( IChunk&                  uut,
                Counts_T&                counts,
                Kit::EventQueue::IQueue& eventQueue,
                Size_T                   newSize,
                uint8_t                  newFill,
                Size_T                   storedLen,
                uint8_t                  storedFill,
                unsigned                 expectedReads,
                unsigned                 expectedWrites )
{
    SizedPayload payload( newSize, newFill );
    REQUIRE( uut.start( eventQueue ) );

    counts = Counts_T();
    REQUIRE( uut.loadData( payload ) );
    REQUIRE( payload.loadedMatches( storedLen, storedFill ) );
    REQUIRE( counts.reads == expectedReads );
    REQUIRE( counts.writes == 0 );

    counts = Counts_T();
    REQUIRE( uut.updateData( payload ) );
    REQUIRE( counts.reads == 0 );
    REQUIRE( counts.writes == expectedWrites );

    uut.stop();
}

/// Returns the transaction ID stored in a Mirrored chunk at offset zero
uint64_t storedTransId( RamMedia& media )
{
    uint64_t                     transId = 0;
    KIT_PERSISTENCE_MEDIA_CURSOR cursor( media.m_data, sizeof( transId ) );
    cursor.read( transId );
    return transId;
}

}  // end anonymous namespace


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Chunk size changes" )
{
    KIT_SYSTEM_TRACE_SCOPE( SECT_, "Chunk size changes" );
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();

    Kit::EventQueue::Server   mockEventQueue;
    Kit::Checksum::Fletcher16 edc;
    Counts_T                  counts;

    SECTION( "Crc" )
    {
        RamMedia media( counts );

        // Boot #1: Erased media
        {
            Crc          uut( media, edc );
            SizedPayload payload( 10, 'a' );
            REQUIRE( uut.start( mockEventQueue ) );
            REQUIRE( uut.loadData( payload ) == false );
            REQUIRE( counts.reads == 1 );
            counts = Counts_T();
            REQUIRE( uut.updateData( payload ) );
            REQUIRE( counts.writes == 1 );
            uut.stop();
        }

        // Boot #2: Record grew (10 -> 20)
        {
            Crc uut( media, edc );
            bootCycle( uut, counts, mockEventQueue, 20, 'b', 10, 'a', 1, 1 );
        }

        // Boot #3: Same size
        {
            Crc uut( media, edc );
            bootCycle( uut, counts, mockEventQueue, 20, 'c', 20, 'b', 1, 1 );
        }

        // Boot #4: Record shrank (20 -> 8). Stored chunk is larger than expected -->extra read
        {
            Crc uut( media, edc );
            bootCycle( uut, counts, mockEventQueue, 8, 'd', 20, 'c', 2, 1 );
        }

        // Boot #5: Adapted to the new (smaller) size
        {
            Crc uut( media, edc );
            bootCycle( uut, counts, mockEventQueue, 8, 'e', 8, 'd', 1, 1 );
        }

        // Boot #6: Grow to the largest size the media supports (expected chunk ends at the end of the media)
        Size_T maxLen = RAM_MEDIA_SIZE - ( sizeof( Size_T ) + edc.getEdcSize() );
        {
            Crc uut( media, edc );
            bootCycle( uut, counts, mockEventQueue, maxLen, 'f', 8, 'e', 1, 1 );
        }

        // Boot #7: Same (max) size
        {
            Crc uut( media, edc );
            bootCycle( uut, counts, mockEventQueue, maxLen, 'g', maxLen, 'f', 1, 1 );
        }
    }

    SECTION( "Mirrored" )
    {
        RamMedia mediaA( counts );
        RamMedia mediaB( counts );

        // Boot #1: Erased media
        {
            Mirrored     uut( mediaA, mediaB, edc );
            SizedPayload payload( 10, 'a' );
            REQUIRE( uut.start( mockEventQueue ) );
            REQUIRE( uut.loadData( payload ) == false );
            REQUIRE( counts.reads == 2 );
            counts = Counts_T();
            REQUIRE( uut.updateData( payload ) );
            REQUIRE( counts.writes == 2 );
            uut.stop();
        }

        // Boot #2: Record grew (10 -> 20)
        {
            Mirrored uut( mediaA, mediaB, edc );
            bootCycle( uut, counts, mockEventQueue, 20, 'b', 10, 'a', 2, 2 );
        }

        // Boot #3: Same size
        {
            Mirrored uut( mediaA, mediaB, edc );
            bootCycle( uut, counts, mockEventQueue, 20, 'c', 20, 'b', 2, 2 );
        }

        // Boot #4: Record shrank (20 -> 8). Both stored chunks are larger than expected -->extra read per media
        {
            Mirrored uut( mediaA, mediaB, edc );
            bootCycle( uut, counts, mockEventQueue, 8, 'd', 20, 'c', 4, 2 );
        }

        // Boot #5: Adapted to the new (smaller) size
        {
            Mirrored uut( mediaA, mediaB, edc );
            bootCycle( uut, counts, mockEventQueue, 8, 'e', 8, 'd', 2, 2 );
        }

        // Boot #6: Grow to the largest size the media supports
        Size_T maxLen = RAM_MEDIA_SIZE - ( sizeof( uint64_t ) + sizeof( Size_T ) + edc.getEdcSize() );
        {
            Mirrored uut( mediaA, mediaB, edc );
            bootCycle( uut, counts, mockEventQueue, maxLen, 'f', 8, 'e', 2, 2 );
        }

        // Boot #7: Record shrank (max -> 12) AND the update was interrupted after only media B was written
        {
            uint8_t savedA[RAM_MEDIA_SIZE];
            memcpy( savedA, mediaA.m_data, sizeof( savedA ) );
            Mirrored uut( mediaA, mediaB, edc );
            bootCycle( uut, counts, mockEventQueue, 12, 'g', maxLen, 'f', 4, 2 );
            memcpy( mediaA.m_data, savedA, sizeof( savedA ) );
        }

        // Boot #8: Media B is newest (and smaller than A) -->B, A (+1 for the larger A), re-read B
        {
            Mirrored uut( mediaA, mediaB, edc );
            bootCycle( uut, counts, mockEventQueue, 12, 'h', 12, 'g', 4, 2 );
        }

        // Boot #9: Both media adapted to the new size AND media A is the newest copy again
        {
            Mirrored uut( mediaA, mediaB, edc );
            bootCycle( uut, counts, mockEventQueue, 12, 'i', 12, 'h', 2, 2 );
        }
    }

    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Chunk fault handling" )
{
    KIT_SYSTEM_TRACE_SCOPE( SECT_, "Chunk fault handling" );
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();

    Kit::EventQueue::Server   mockEventQueue;
    Kit::Checksum::Fletcher16 edc;
    Counts_T                  counts;
    SizedPayload              payloadA( 10, 'a' );

    SECTION( "Mirrored: write failures" )
    {
        RamMedia mediaA( counts );
        RamMedia mediaB( counts );
        Mirrored uut( mediaA, mediaB, edc );
        REQUIRE( uut.start( mockEventQueue ) );
        REQUIRE( uut.loadData( payloadA ) == false );
        REQUIRE( uut.updateData( payloadA ) );
        REQUIRE( storedTransId( mediaA ) == 2 );
        REQUIRE( storedTransId( mediaB ) == 1 );

        // First write (to the oldest copy, B) fails -->nothing else is written
        SizedPayload payloadB( 10, 'b' );
        mediaB.arm( 0, 0, 0x1 );
        counts = Counts_T();
        REQUIRE( uut.updateData( payloadB ) == false );
        REQUIRE( counts.writes == 1 );
        REQUIRE( storedTransId( mediaA ) == 2 );
        REQUIRE( storedTransId( mediaB ) == 1 );

        // Recovers: B is still the oldest copy
        mediaB.arm( 0, 0, 0 );
        REQUIRE( uut.updateData( payloadB ) );
        REQUIRE( storedTransId( mediaA ) == 4 );
        REQUIRE( storedTransId( mediaB ) == 3 );

        // Second write (A) fails -->B holds the newest copy
        SizedPayload payloadC( 10, 'c' );
        mediaA.arm( 0, 0, 0x1 );
        counts = Counts_T();
        REQUIRE( uut.updateData( payloadC ) == false );
        REQUIRE( counts.writes == 2 );
        REQUIRE( storedTransId( mediaA ) == 4 );
        REQUIRE( storedTransId( mediaB ) == 5 );

        // Next update writes the (now oldest) A copy first, and A ends up newest
        mediaA.arm( 0, 0, 0 );
        REQUIRE( uut.updateData( payloadC ) );
        REQUIRE( storedTransId( mediaA ) == 7 );
        REQUIRE( storedTransId( mediaB ) == 6 );
        uut.stop();

        // Reboot: newest copy is found with 2 reads
        Mirrored     uut2( mediaA, mediaB, edc );
        SizedPayload loaded( 10, 'x' );
        REQUIRE( uut2.start( mockEventQueue ) );
        counts = Counts_T();
        REQUIRE( uut2.loadData( loaded ) );
        REQUIRE( counts.reads == 2 );
        REQUIRE( loaded.loadedMatches( 10, 'c' ) );
        uut2.stop();
    }

    SECTION( "Mirrored: newest copy (B) fails to re-read" )
    {
        RamMedia mediaA( counts );
        RamMedia mediaB( counts );
        {
            Mirrored uut( mediaA, mediaB, edc );
            REQUIRE( uut.start( mockEventQueue ) );
            REQUIRE( uut.loadData( payloadA ) == false );
            REQUIRE( uut.updateData( payloadA ) );
            mediaA.arm( 0, 0, 0x2 );  // A's 2nd write (i.e. during the 3rd update) fails -->B is newest
            REQUIRE( uut.updateData( payloadA ) );
            REQUIRE( uut.updateData( payloadA ) == false );
            REQUIRE( storedTransId( mediaB ) > storedTransId( mediaA ) );
            uut.stop();
        }

        // All reads of B after its first read fail
        Mirrored uut( mediaA, mediaB, edc );
        REQUIRE( uut.start( mockEventQueue ) );
        mediaA.arm( 0, 0, 0 );
        mediaB.arm( ~0x1u, 0, 0 );
        SizedPayload loaded( 10, 'x' );
        REQUIRE( uut.loadData( loaded ) == false );
        REQUIRE( loaded.m_loadedLen == 0 );
        uut.stop();
    }

    SECTION( "readChunk edge cases" )
    {
        RamMedia media( counts );
        Crc      uut( media, edc );
        REQUIRE( uut.start( mockEventQueue ) );
        REQUIRE( uut.loadData( payloadA ) == false );
        REQUIRE( uut.updateData( payloadA ) );

        // Index at/past the end of the media, or too little space for the meta data -->no media reads
        counts = Counts_T();
        REQUIRE( uut.loadData( payloadA, RAM_MEDIA_SIZE ) == false );
        REQUIRE( uut.loadData( payloadA, RAM_MEDIA_SIZE + 1 ) == false );
        REQUIRE( uut.loadData( payloadA, RAM_MEDIA_SIZE - 3 ) == false );
        REQUIRE( counts.reads == 0 );

        // Full read fails -->header read + remainder read
        SizedPayload loaded( 10, 'x' );
        media.arm( 0x1, 0, 0 );
        counts = Counts_T();
        REQUIRE( uut.loadData( loaded ) );
        REQUIRE( counts.reads == 3 );
        REQUIRE( loaded.loadedMatches( 10, 'a' ) );

        // Full read AND header read fail
        media.arm( 0x3, 0, 0 );
        counts = Counts_T();
        REQUIRE( uut.loadData( loaded ) == false );
        REQUIRE( counts.reads == 2 );

        // Short (less than the header) read -->header read + remainder read
        media.arm( 0, 0x1, 0 );
        counts = Counts_T();
        REQUIRE( uut.loadData( loaded ) );
        REQUIRE( counts.reads == 3 );

        // Stored chunk larger than expected AND the remainder read fails
        SizedPayload smaller( 4, 'x' );
        media.arm( 0x2, 0, 0 );
        counts = Counts_T();
        REQUIRE( uut.loadData( smaller ) == false );
        REQUIRE( counts.reads == 2 );
        uut.stop();
    }

    SECTION( "work buffer limits" )
    {
        RamMedia media( counts );
        RamMedia mediaB( counts );
        Size_T   crcMeta = sizeof( Size_T ) + edc.getEdcSize();
        Size_T   mirMeta = sizeof( uint64_t ) + crcMeta;

        // Work buffer smaller than the chunk's meta data
        uint8_t tinyBuf[sizeof( uint64_t ) + sizeof( Size_T )];
        {
            Crc uut( media, edc, tinyBuf, crcMeta - 1 );
            REQUIRE( uut.start( mockEventQueue ) );
            counts = Counts_T();
            REQUIRE( uut.loadData( payloadA ) == false );
            REQUIRE( uut.updateData( payloadA ) == false );
            REQUIRE( counts.reads == 0 );
            REQUIRE( counts.writes == 0 );
            uut.stop();
        }
        {
            Mirrored uut( media, mediaB, edc, tinyBuf, sizeof( tinyBuf ) );
            REQUIRE( uut.start( mockEventQueue ) );
            counts = Counts_T();
            REQUIRE( uut.loadData( payloadA ) == false );
            REQUIRE( uut.updateData( payloadA ) == false );
            REQUIRE( counts.reads == 0 );
            REQUIRE( counts.writes == 0 );
            uut.stop();
        }

        // Work buffer sized EXACTLY for the payload + meta data
        constexpr Size_T PAYLOAD_LEN = 10;
        uint8_t          exactBuf[PAYLOAD_LEN + sizeof( uint64_t ) + sizeof( Size_T ) + sizeof( uint32_t )];
        {
            Crc uut( media, edc, exactBuf, PAYLOAD_LEN + crcMeta );
            REQUIRE( uut.start( mockEventQueue ) );
            uut.loadData( payloadA );
            REQUIRE( uut.updateData( payloadA ) );
            SizedPayload loaded( PAYLOAD_LEN, 'x' );
            REQUIRE( uut.loadData( loaded ) );
            REQUIRE( loaded.loadedMatches( PAYLOAD_LEN, 'a' ) );

            // Payload is one byte too large -->IPayload::copyTo() fails
            SizedPayload tooBig( PAYLOAD_LEN + 1, 'b' );
            counts = Counts_T();
            REQUIRE( uut.updateData( tooBig ) == false );
            REQUIRE( counts.writes == 0 );
            uut.stop();
        }
        {
            Mirrored uut( media, mediaB, edc, exactBuf, PAYLOAD_LEN + mirMeta );
            REQUIRE( uut.start( mockEventQueue ) );
            uut.loadData( payloadA );
            REQUIRE( uut.updateData( payloadA ) );
            SizedPayload loaded( PAYLOAD_LEN, 'x' );
            REQUIRE( uut.loadData( loaded ) );
            REQUIRE( loaded.loadedMatches( PAYLOAD_LEN, 'a' ) );

            SizedPayload tooBig( PAYLOAD_LEN + 1, 'b' );
            counts = Counts_T();
            REQUIRE( uut.updateData( tooBig ) == false );
            REQUIRE( counts.writes == 0 );
            uut.stop();
        }

        // Stored chunk is one byte too large for the work buffer -->fails to load
        {
            RamMedia mediaC( counts );
            Crc      writer( mediaC, edc, exactBuf, PAYLOAD_LEN + crcMeta );
            REQUIRE( writer.start( mockEventQueue ) );
            writer.loadData( payloadA );
            REQUIRE( writer.updateData( payloadA ) );
            writer.stop();

            Crc uut( mediaC, edc, exactBuf, PAYLOAD_LEN + crcMeta - 1 );
            REQUIRE( uut.start( mockEventQueue ) );
            SizedPayload loaded( PAYLOAD_LEN, 'x' );
            REQUIRE( uut.loadData( loaded ) == false );
            REQUIRE( loaded.m_loadedLen == 0 );
            uut.stop();
        }
    }

    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}

////////////////////////////////////////////////////////////////////////////////
// The golden bytes below assume the default meta data configuration
static_assert( sizeof( Size_T ) == sizeof( uint32_t ), "Golden bytes require a 32bit Size_T" );
static_assert( OPTION_KIT_PERSISTENCE_MEDIA_LITTLE_ENDIAN > 0, "Golden bytes require little endian meta data" );

// [data len][data: 'HHH'][Fletcher16]
static const uint8_t goldenCrc_[] = { 0x03, 0x00, 0x00, 0x00, 0x48, 0x48, 0x48, 0x5D, 0xC6 };

// [trans ID][data len][data: 'HHH'][Fletcher16]
static const uint8_t goldenMirroredA_[] = { 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x48, 0x48, 0x48, 0x3D, 0xE4 };
static const uint8_t goldenMirroredB_[] = { 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x48, 0x48, 0x48, 0x4D, 0xD5 };

TEST_CASE( "Chunk on-media format" )
{
    KIT_SYSTEM_TRACE_SCOPE( SECT_, "Chunk on-media format" );
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();

    Kit::EventQueue::Server   mockEventQueue;
    Kit::Checksum::Fletcher16 edc;
    Counts_T                  counts;
    SizedPayload              payload( 3, 'H' );
    SizedPayload              loaded( 3, 'x' );

    SECTION( "Crc" )
    {
        // Write
        RamMedia media( counts );
        Crc      uut( media, edc );
        REQUIRE( uut.start( mockEventQueue ) );
        uut.loadData( payload );
        REQUIRE( uut.updateData( payload ) );
        REQUIRE( memcmp( media.m_data, goldenCrc_, sizeof( goldenCrc_ ) ) == 0 );
        uut.stop();

        // Read
        RamMedia media2( counts );
        memcpy( media2.m_data, goldenCrc_, sizeof( goldenCrc_ ) );
        Crc uut2( media2, edc );
        REQUIRE( uut2.start( mockEventQueue ) );
        REQUIRE( uut2.loadData( loaded ) );
        REQUIRE( loaded.loadedMatches( 3, 'H' ) );
        uut2.stop();
    }

    SECTION( "Mirrored" )
    {
        // Write
        RamMedia mediaA( counts );
        RamMedia mediaB( counts );
        Mirrored uut( mediaA, mediaB, edc );
        REQUIRE( uut.start( mockEventQueue ) );
        uut.loadData( payload );
        REQUIRE( uut.updateData( payload ) );
        REQUIRE( memcmp( mediaA.m_data, goldenMirroredA_, sizeof( goldenMirroredA_ ) ) == 0 );
        REQUIRE( memcmp( mediaB.m_data, goldenMirroredB_, sizeof( goldenMirroredB_ ) ) == 0 );
        uut.stop();

        // Read
        RamMedia mediaA2( counts );
        RamMedia mediaB2( counts );
        memcpy( mediaA2.m_data, goldenMirroredA_, sizeof( goldenMirroredA_ ) );
        memcpy( mediaB2.m_data, goldenMirroredB_, sizeof( goldenMirroredB_ ) );
        Mirrored uut2( mediaA2, mediaB2, edc );
        REQUIRE( uut2.start( mockEventQueue ) );
        REQUIRE( uut2.loadData( loaded ) );
        REQUIRE( loaded.loadedMatches( 3, 'H' ) );
        uut2.stop();
    }

    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}
