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

#define RAM_MEDIA_SIZE 128

struct Counts
{
    unsigned reads  = 0;
    unsigned writes = 0;
};

/// RAM based media that counts the IMedia read/write calls
class RamMedia : public IMedia
{
public:
    uint8_t m_data[RAM_MEDIA_SIZE];
    Counts& m_counts;

    RamMedia( Counts& counts )
        : m_counts( counts )
    {
        memset( m_data, 0xFF, sizeof( m_data ) );  // Erased state
    }

    bool start( Kit::EventQueue::IQueue& myEventQueue ) noexcept override { return true; }
    void stop() noexcept override {}

    bool write( Size_T offset, const void* srcData, Size_T srcLen ) noexcept override
    {
        m_counts.writes++;
        if ( srcLen > RAM_MEDIA_SIZE || offset > RAM_MEDIA_SIZE - srcLen )
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
        if ( bytesToRead > RAM_MEDIA_SIZE || offset > RAM_MEDIA_SIZE - bytesToRead )
        {
            return KIT_PERSISTENCE_SIZE_MAX;
        }
        memcpy( dstBuffer, m_data + offset, bytesToRead );
        return bytesToRead;
    }

    Size_T getMaxSize() const noexcept override { return RAM_MEDIA_SIZE; }
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
                Counts&                  counts,
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

    counts = Counts();
    REQUIRE( uut.loadData( payload ) );
    REQUIRE( payload.loadedMatches( storedLen, storedFill ) );
    REQUIRE( counts.reads == expectedReads );
    REQUIRE( counts.writes == 0 );

    counts = Counts();
    REQUIRE( uut.updateData( payload ) );
    REQUIRE( counts.reads == 0 );
    REQUIRE( counts.writes == expectedWrites );

    uut.stop();
}

}  // end anonymous namespace


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Chunk size changes" )
{
    KIT_SYSTEM_TRACE_SCOPE( SECT_, "Chunk size changes" );
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();

    Kit::EventQueue::Server   mockEventQueue;
    Kit::Checksum::Fletcher16 edc;
    Counts                    counts;

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
            counts = Counts();
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
            counts = Counts();
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
