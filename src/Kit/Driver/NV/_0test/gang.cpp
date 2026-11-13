/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Driver/NV/Gang.h"
#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include "Kit/System/Trace.h"
#include "catch2/catch_test_macros.hpp"
#include <string.h>

#define SECT_ "_0test"

using namespace Kit::Driver::NV;

namespace {

#define MAX_MOCK_SIZE 64

class MockNvDriver : public IApi
{
public:
    MockNvDriver( size_t numPages, size_t pageSize ) noexcept
        : m_numPages( numPages )
        , m_pageSize( pageSize )
    {
        reset();
    }

    void reset() noexcept
    {
        m_startResult   = true;
        m_writeResult   = true;
        m_readResult    = true;
        m_started       = false;
        m_startArgs     = nullptr;
        m_startCount    = 0;
        m_stopCount     = 0;
        m_lastReadSize  = 0;
        m_lastDstSize   = 0;
        m_lastWriteSize = 0;
        memset( m_memory, 0xFF, sizeof( m_memory ) );
    }

    bool start( void* startArgs = nullptr ) noexcept override
    {
        m_startCount++;
        m_startArgs = startArgs;
        m_started   = m_startResult;
        return m_startResult;
    }

    void stop() noexcept override
    {
        m_stopCount++;
        m_started = false;
    }

    bool write( size_t dstOffset, const void* srcData, size_t numBytesToWrite ) noexcept override
    {
        REQUIRE( dstOffset + numBytesToWrite <= getTotalSize() );
        m_lastWriteSize = numBytesToWrite;
        memcpy( m_memory + dstOffset, srcData, numBytesToWrite );
        return m_writeResult;
    }

    bool read( size_t srcOffset, void* dstData, size_t sizeDstData, size_t numBytesToRead ) noexcept override
    {
        REQUIRE( srcOffset + numBytesToRead <= getTotalSize() );
        m_lastReadSize = numBytesToRead;
        m_lastDstSize  = sizeDstData;
        if ( sizeDstData < numBytesToRead )
        {
            return false;
        }
        memcpy( dstData, m_memory + srcOffset, numBytesToRead );
        return m_readResult;
    }

    size_t getNumPages() const noexcept override
    {
        return m_numPages;
    }

    size_t getPageSize() const noexcept override
    {
        return m_pageSize;
    }

    size_t getTotalSize() const noexcept override
    {
        return m_numPages * m_pageSize;
    }

public:
    size_t   m_numPages;
    size_t   m_pageSize;
    uint8_t  m_memory[MAX_MOCK_SIZE];
    bool     m_startResult;
    bool     m_writeResult;
    bool     m_readResult;
    bool     m_started;
    void*    m_startArgs;
    unsigned m_startCount;
    unsigned m_stopCount;
    size_t   m_lastReadSize;
    size_t   m_lastDstSize;
    size_t   m_lastWriteSize;
};

}  // end anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// Ganged layout: driver0 = [0..15], driver1 = [16..27], driver2 = [28..59]
#define SIZE0      16
#define SIZE1      12
#define SIZE2      32
#define TOTAL_SIZE ( SIZE0 + SIZE1 + SIZE2 )

TEST_CASE( "Gang" )
{
    KIT_SYSTEM_TRACE_FUNC( SECT_ );
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();

    MockNvDriver driver0( 4, 4 );  // 16 bytes
    MockNvDriver driver1( 4, 3 );  // 12 bytes, smallest page size
    MockNvDriver driver2( 4, 8 );  // 32 bytes
    IApi*        drivers[] = { &driver0, &driver1, &driver2 };
    Gang         uut( drivers, sizeof( drivers ) / sizeof( drivers[0] ) );

    uint8_t srcData[TOTAL_SIZE];
    for ( size_t i = 0; i < sizeof( srcData ); i++ )
    {
        srcData[i] = static_cast<uint8_t>( i + 1 );
    }
    uint8_t dstData[TOTAL_SIZE];
    memset( dstData, 0, sizeof( dstData ) );

    SECTION( "geometry" )
    {
        // Geometry is only reported while started
        REQUIRE( uut.getTotalSize() == 0 );
        REQUIRE( uut.getNumPages() == 0 );
        REQUIRE( uut.getPageSize() == 0 );

        REQUIRE( uut.start() );
        REQUIRE( uut.getTotalSize() == TOTAL_SIZE );
        REQUIRE( uut.getPageSize() == 3 );
        REQUIRE( uut.getNumPages() == TOTAL_SIZE / 3 );

        uut.stop();
        REQUIRE( uut.getTotalSize() == 0 );
        REQUIRE( uut.getNumPages() == 0 );
        REQUIRE( uut.getPageSize() == 0 );
    }

    SECTION( "geometry is calculated on demand" )
    {
        // Driver geometry that changes after construction (but before start) is honored
        driver1.m_numPages = 2;
        driver1.m_pageSize = 2;
        REQUIRE( uut.start() );
        REQUIRE( uut.getTotalSize() == SIZE0 + 4 + SIZE2 );
        REQUIRE( uut.getPageSize() == 2 );
        REQUIRE( uut.getNumPages() == ( SIZE0 + 4 + SIZE2 ) / 2 );

        // Page size is not cached
        driver0.m_pageSize = 1;
        REQUIRE( uut.getPageSize() == 1 );
        driver0.m_pageSize = 0;
        REQUIRE( uut.getPageSize() == 0 );
        REQUIRE( uut.getNumPages() == 0 );
        uut.stop();
    }

    SECTION( "constructor asserts" )
    {
        // Violations assert (the unit-test shutdown handler counts instead of terminating)
        Kit::System::ShutdownUnitTesting::clearAndUseCounter();
        Gang noArray( nullptr, 0 );
        REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 2u );

        Gang noDrivers( drivers, 0 );
        REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 1u );

        IApi* withNull[] = { &driver0, nullptr, &driver2 };
        Gang  nullEntry( withNull, 3 );
        REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 1u );
    }

    SECTION( "start/stop" )
    {
        int startArgs = 0;
        REQUIRE( uut.start( &startArgs ) );
        REQUIRE( driver0.m_startArgs == &startArgs );
        REQUIRE( driver1.m_startArgs == &startArgs );
        REQUIRE( driver2.m_startArgs == &startArgs );
        REQUIRE( driver0.m_started );
        REQUIRE( driver2.m_started );

        // Already started -> drivers are not restarted
        REQUIRE( uut.start() );
        REQUIRE( driver0.m_startCount == 1 );

        uut.stop();
        REQUIRE( driver0.m_stopCount == 1 );
        REQUIRE( driver1.m_stopCount == 1 );
        REQUIRE( driver2.m_stopCount == 1 );
        REQUIRE( driver1.m_started == false );

        // Already stopped -> drivers are not re-stopped
        uut.stop();
        REQUIRE( driver0.m_stopCount == 1 );
    }

    SECTION( "start failure" )
    {
        driver1.m_startResult = false;
        REQUIRE( uut.start() == false );
        REQUIRE( uut.getTotalSize() == 0 );
        REQUIRE( driver2.m_startCount == 0 );
        REQUIRE( uut.write( 0, srcData, 1 ) == false );

        // Previously started drivers are rolled back; the failed driver is not stopped
        REQUIRE( driver0.m_stopCount == 1 );
        REQUIRE( driver0.m_started == false );
        REQUIRE( driver1.m_stopCount == 0 );
        REQUIRE( driver2.m_stopCount == 0 );

        // Not started -> stop() is a NOP
        uut.stop();
        REQUIRE( driver0.m_stopCount == 1 );

        driver1.m_startResult = true;
        REQUIRE( uut.start() );
        REQUIRE( driver0.m_started );
        REQUIRE( driver1.m_started );
        REQUIRE( driver2.m_started );
        uut.stop();
    }

    SECTION( "first driver start failure" )
    {
        driver0.m_startResult = false;
        REQUIRE( uut.start() == false );
        REQUIRE( driver0.m_stopCount == 0 );
        REQUIRE( driver1.m_startCount == 0 );
    }

    SECTION( "not started" )
    {
        REQUIRE( uut.write( 0, srcData, 1 ) == false );
        REQUIRE( uut.read( 0, dstData, sizeof( dstData ), 1 ) == false );
    }

    SECTION( "write/read spanning all drivers" )
    {
        REQUIRE( uut.start() );
        REQUIRE( uut.write( 0, srcData, TOTAL_SIZE ) );
        REQUIRE( memcmp( driver0.m_memory, srcData, SIZE0 ) == 0 );
        REQUIRE( memcmp( driver1.m_memory, srcData + SIZE0, SIZE1 ) == 0 );
        REQUIRE( memcmp( driver2.m_memory, srcData + SIZE0 + SIZE1, SIZE2 ) == 0 );

        REQUIRE( uut.read( 0, dstData, sizeof( dstData ), TOTAL_SIZE ) );
        REQUIRE( memcmp( dstData, srcData, TOTAL_SIZE ) == 0 );
        uut.stop();
    }

    SECTION( "write/read across a driver boundary" )
    {
        REQUIRE( uut.start() );

        // Starts at offset 14 of driver0, ends at offset 1 of driver2
        size_t offset = SIZE0 - 2;
        size_t len    = 2 + SIZE1 + 2;
        REQUIRE( uut.write( offset, srcData, len ) );
        REQUIRE( memcmp( driver0.m_memory + SIZE0 - 2, srcData, 2 ) == 0 );
        REQUIRE( memcmp( driver1.m_memory, srcData + 2, SIZE1 ) == 0 );
        REQUIRE( memcmp( driver2.m_memory, srcData + 2 + SIZE1, 2 ) == 0 );
        REQUIRE( driver2.m_memory[2] == 0xFF );

        REQUIRE( uut.read( offset, dstData, sizeof( dstData ), len ) );
        REQUIRE( memcmp( dstData, srcData, len ) == 0 );
        REQUIRE( driver2.m_lastReadSize == 2 );
        REQUIRE( driver2.m_lastDstSize == 2 );
        uut.stop();
    }

    SECTION( "write/read within the last driver" )
    {
        REQUIRE( uut.start() );
        size_t offset = SIZE0 + SIZE1 + 5;
        REQUIRE( uut.write( offset, srcData, 4 ) );
        REQUIRE( memcmp( driver2.m_memory + 5, srcData, 4 ) == 0 );
        REQUIRE( driver0.m_lastWriteSize == 0 );
        REQUIRE( driver1.m_lastWriteSize == 0 );

        REQUIRE( uut.read( offset, dstData, 4, 4 ) );
        REQUIRE( memcmp( dstData, srcData, 4 ) == 0 );
        uut.stop();
    }

    SECTION( "out of range" )
    {
        REQUIRE( uut.start() );
        REQUIRE( uut.write( TOTAL_SIZE - 1, srcData, 2 ) == false );
        REQUIRE( uut.read( TOTAL_SIZE - 1, dstData, sizeof( dstData ), 2 ) == false );
        REQUIRE( uut.write( TOTAL_SIZE - 1, srcData, 1 ) );
        REQUIRE( uut.read( TOTAL_SIZE - 1, dstData, sizeof( dstData ), 1 ) );
        REQUIRE( dstData[0] == srcData[0] );

        // Offset/length values that would wrap around if simply added
        REQUIRE( uut.write( SIZE_MAX, srcData, 2 ) == false );
        REQUIRE( uut.read( SIZE_MAX, dstData, sizeof( dstData ), 2 ) == false );
        REQUIRE( uut.write( 2, srcData, SIZE_MAX - 1 ) == false );
        REQUIRE( uut.read( 2, dstData, SIZE_MAX, SIZE_MAX - 1 ) == false );
        REQUIRE( driver0.m_lastWriteSize == 0 );
        REQUIRE( driver0.m_lastReadSize == 0 );
        uut.stop();
    }

    SECTION( "zero-length write/read" )
    {
        REQUIRE( uut.write( 0, srcData, 0 ) == false );  // Not started
        REQUIRE( uut.read( 0, dstData, sizeof( dstData ), 0 ) == false );

        REQUIRE( uut.start() );
        driver0.m_writeResult = false;  // Fails the transfer if a driver is accessed
        driver0.m_readResult  = false;
        driver2.m_writeResult = false;
        driver2.m_readResult  = false;
        REQUIRE( uut.write( 0, srcData, 0 ) );
        REQUIRE( uut.read( 0, dstData, sizeof( dstData ), 0 ) );
        REQUIRE( uut.write( TOTAL_SIZE, srcData, 0 ) );
        REQUIRE( uut.read( TOTAL_SIZE, dstData, sizeof( dstData ), 0 ) );
        REQUIRE( uut.write( TOTAL_SIZE + 1, srcData, 0 ) == false );
        REQUIRE( uut.read( TOTAL_SIZE + 1, dstData, sizeof( dstData ), 0 ) == false );
        uut.stop();
    }

    SECTION( "destination buffer too small" )
    {
        REQUIRE( uut.start() );
        REQUIRE( uut.read( 0, dstData, 3, 4 ) == false );
        REQUIRE( driver0.m_lastReadSize == 0 );
        uut.stop();
    }

    SECTION( "driver write/read failure" )
    {
        REQUIRE( uut.start() );
        driver1.m_writeResult = false;
        driver1.m_readResult  = false;
        REQUIRE( uut.write( 0, srcData, TOTAL_SIZE ) == false );
        REQUIRE( driver2.m_lastWriteSize == 0 );
        REQUIRE( uut.read( 0, dstData, sizeof( dstData ), TOTAL_SIZE ) == false );
        REQUIRE( driver2.m_lastReadSize == 0 );
        uut.stop();
    }

    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}
