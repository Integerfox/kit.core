/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file
    Unit tests for Kit::Driver::NV::Flash::Api -- the NV flash driver with
    wear leveling.  Tests cover:
      - Basic read/write/format operations
      - Read-modify-write preservation
      - Cross-page writes
      - Scatter writes to different pages
      - Sector reclamation
      - Startup scan and page map rebuild
      - 4KB application data configuration (16 logical pages, 128KB flash)
      - 128KB application data configuration (512 logical pages, 4MB flash)
      - Error handling and boundary conditions
*/

#include "catch2/catch_test_macros.hpp"
#include "Kit/Driver/Flash/_testsupport/MockFlash.h"
#include "Kit/Driver/NV/Flash/Api.h"
#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include <string.h>

using namespace Kit::Driver;
using namespace Kit::System;

// ============================================================================
// Configuration: 4KB Application Data
//   Logical NV Size:    4096 bytes
//   NV Page Size:       256 bytes
//   Logical Pages:      16
//   Physical Flash:     128KB (32 sectors, 448 record slots)
//   Over-provisioning:  28:1
// ============================================================================
static constexpr size_t   FLASH_SIZE_4K        = 128 * 1024;   // 128KB
static constexpr size_t   NV_PAGE_SIZE_4K      = 256;
static constexpr size_t   NV_TOTAL_SIZE_4K     = 4096;
static constexpr size_t   MAX_LOGICAL_PAGES_4K = 16;

static const NV::Flash::Config_T CONFIG_4K = {
    0,                  // flashStartAddress
    FLASH_SIZE_4K,      // flashEndAddress
    NV_PAGE_SIZE_4K,    // nvPageSize
    NV_TOTAL_SIZE_4K    // nvTotalSize
};

using MockFlash4K = Flash::TestSupport::MockFlash<FLASH_SIZE_4K>;
using NvFlash4K   = NV::Flash::Api<MAX_LOGICAL_PAGES_4K>;


// ============================================================================
// Configuration: 128KB Application Data  
//   Logical NV Size:    131072 bytes (128KB)
//   NV Page Size:       256 bytes
//   Logical Pages:      512
//   Physical Flash:     4MB (1024 sectors, 14336 record slots)
//   Over-provisioning:  28:1
// ============================================================================
static constexpr size_t   FLASH_SIZE_128K        = 4 * 1024 * 1024; // 4MB
static constexpr size_t   NV_PAGE_SIZE_128K      = 256;
static constexpr size_t   NV_TOTAL_SIZE_128K     = 128 * 1024;
static constexpr size_t   MAX_LOGICAL_PAGES_128K = 512;

static const NV::Flash::Config_T CONFIG_128K = {
    0,                    // flashStartAddress
    FLASH_SIZE_128K,      // flashEndAddress
    NV_PAGE_SIZE_128K,    // nvPageSize
    NV_TOTAL_SIZE_128K    // nvTotalSize
};

using MockFlash128K = Flash::TestSupport::MockFlash<FLASH_SIZE_128K>;
using NvFlash128K   = NV::Flash::Api<MAX_LOGICAL_PAGES_128K>;


// ============================================================================
// 4KB Configuration Tests
// ============================================================================

TEST_CASE( "NV Flash 4K - Initialization" )
{
    MockFlash4K mockFlash;
    mockFlash.start();
    NvFlash4K nv( mockFlash, CONFIG_4K );
    ShutdownUnitTesting::clearAndUseCounter();

    SECTION( "start succeeds on erased flash" )
    {
        REQUIRE( nv.start() == true );
        REQUIRE( nv.getTotalSize() == NV_TOTAL_SIZE_4K );
        REQUIRE( nv.getNumPages() == MAX_LOGICAL_PAGES_4K );
        REQUIRE( nv.getPageSize() == NV_PAGE_SIZE_4K );
        REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
    }

    SECTION( "start is idempotent" )
    {
        REQUIRE( nv.start() == true );
        REQUIRE( nv.start() == true ); // Second call succeeds without error
        REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
    }

    SECTION( "stop and restart" )
    {
        REQUIRE( nv.start() == true );
        nv.stop();
        REQUIRE( nv.start() == true );
        REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
    }
}


TEST_CASE( "NV Flash 4K - Basic Read/Write" )
{
    MockFlash4K mockFlash;
    mockFlash.start();
    NvFlash4K nv( mockFlash, CONFIG_4K );
    nv.start();
    ShutdownUnitTesting::clearAndUseCounter();

    SECTION( "write and read back small data" )
    {
        uint8_t writeData[] = { 0x01, 0x02, 0x03, 0x04, 0x05 };
        REQUIRE( nv.write( 0, writeData, sizeof( writeData ) ) == true );

        uint8_t readData[5] = { 0 };
        REQUIRE( nv.read( 0, readData, sizeof( readData ), sizeof( readData ) ) == true );
        REQUIRE( memcmp( readData, writeData, sizeof( writeData ) ) == 0 );
        REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
    }

    SECTION( "read unwritten area returns 0xFF" )
    {
        uint8_t readData[10];
        REQUIRE( nv.read( 0, readData, sizeof( readData ), sizeof( readData ) ) == true );

        uint8_t expected[10];
        memset( expected, 0xFF, sizeof( expected ) );
        REQUIRE( memcmp( readData, expected, sizeof( expected ) ) == 0 );
        REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
    }

    SECTION( "write full page" )
    {
        uint8_t pageData[NV_PAGE_SIZE_4K];
        for ( size_t i = 0; i < NV_PAGE_SIZE_4K; i++ )
        {
            pageData[i] = static_cast<uint8_t>( i & 0xFF );
        }

        REQUIRE( nv.write( 0, pageData, NV_PAGE_SIZE_4K ) == true );

        uint8_t readBack[NV_PAGE_SIZE_4K];
        REQUIRE( nv.read( 0, readBack, NV_PAGE_SIZE_4K, NV_PAGE_SIZE_4K ) == true );
        REQUIRE( memcmp( readBack, pageData, NV_PAGE_SIZE_4K ) == 0 );
        REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
    }
}


TEST_CASE( "NV Flash 4K - Read-Modify-Write Preservation" )
{
    MockFlash4K mockFlash;
    mockFlash.start();
    NvFlash4K nv( mockFlash, CONFIG_4K );
    nv.start();
    ShutdownUnitTesting::clearAndUseCounter();

    SECTION( "sequential writes to same page preserve unmodified bytes" )
    {
        // Write #1: 10 bytes at offset 0
        uint8_t data1[] = { 0x01, 0x02, 0x03, 0x04, 0x05,
                            0x06, 0x07, 0x08, 0x09, 0x0A };
        REQUIRE( nv.write( 0, data1, sizeof( data1 ) ) == true );

        // Write #2: Overwrite first 2 bytes
        uint8_t data2[] = { 0xB2, 0xC4 };
        REQUIRE( nv.write( 0, data2, sizeof( data2 ) ) == true );

        // Read back 10 bytes: should be B2 C4 03 04 05 06 07 08 09 0A
        uint8_t readData[10];
        REQUIRE( nv.read( 0, readData, sizeof( readData ), sizeof( readData ) ) == true );

        REQUIRE( readData[0] == 0xB2 );
        REQUIRE( readData[1] == 0xC4 );
        REQUIRE( readData[2] == 0x03 );
        REQUIRE( readData[3] == 0x04 );
        REQUIRE( readData[4] == 0x05 );
        REQUIRE( readData[5] == 0x06 );
        REQUIRE( readData[6] == 0x07 );
        REQUIRE( readData[7] == 0x08 );
        REQUIRE( readData[8] == 0x09 );
        REQUIRE( readData[9] == 0x0A );
        REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
    }

    SECTION( "three sequential writes from design doc scenario" )
    {
        // Write #1: 10 bytes at offset 0
        uint8_t data1[] = { 0x01, 0x02, 0x03, 0x04, 0x05,
                            0x06, 0x07, 0x08, 0x09, 0x0A };
        REQUIRE( nv.write( 0, data1, sizeof( data1 ) ) == true );

        // Write #2: 2 bytes at offset 0
        uint8_t data2[] = { 0xB2, 0xC4 };
        REQUIRE( nv.write( 0, data2, sizeof( data2 ) ) == true );

        // Write #3: 3 bytes at offset 5
        uint8_t data3[] = { 0xDD, 0xEE, 0xFF };
        REQUIRE( nv.write( 5, data3, sizeof( data3 ) ) == true );

        // Read back: should be B2 C4 03 04 05 DD EE FF 09 0A
        uint8_t readData[10];
        REQUIRE( nv.read( 0, readData, sizeof( readData ), sizeof( readData ) ) == true );

        REQUIRE( readData[0] == 0xB2 );
        REQUIRE( readData[1] == 0xC4 );
        REQUIRE( readData[2] == 0x03 );
        REQUIRE( readData[3] == 0x04 );
        REQUIRE( readData[4] == 0x05 );
        REQUIRE( readData[5] == 0xDD );
        REQUIRE( readData[6] == 0xEE );
        REQUIRE( readData[7] == 0xFF );
        REQUIRE( readData[8] == 0x09 );
        REQUIRE( readData[9] == 0x0A );
        REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
    }
}


TEST_CASE( "NV Flash 4K - Scatter Write" )
{
    MockFlash4K mockFlash;
    mockFlash.start();
    NvFlash4K nv( mockFlash, CONFIG_4K );
    nv.start();
    ShutdownUnitTesting::clearAndUseCounter();

    SECTION( "writes to different logical pages" )
    {
        // Write A: offset 0 (Page 0)
        uint8_t dataA[] = { 0xAA, 0xBB, 0xCC, 0xDD };
        REQUIRE( nv.write( 0, dataA, sizeof( dataA ) ) == true );

        // Write B: offset 256 (Page 1)
        uint8_t dataB[] = { 0x11, 0x22, 0x33 };
        REQUIRE( nv.write( 256, dataB, sizeof( dataB ) ) == true );

        // Write C: offset 512 (Page 2)
        uint8_t dataC[] = { 0xEE, 0xFF };
        REQUIRE( nv.write( 512, dataC, sizeof( dataC ) ) == true );

        // Write D: update page 0 (offset 0)
        uint8_t dataD[] = { 0x99 };
        REQUIRE( nv.write( 0, dataD, sizeof( dataD ) ) == true );

        // Write E: update page 1 (offset 256)
        uint8_t dataE[] = { 0x44, 0x55, 0x66, 0x77 };
        REQUIRE( nv.write( 256, dataE, sizeof( dataE ) ) == true );

        // Verify page 0: should be 99 BB CC DD
        uint8_t read0[4];
        REQUIRE( nv.read( 0, read0, sizeof( read0 ), sizeof( read0 ) ) == true );
        REQUIRE( read0[0] == 0x99 );
        REQUIRE( read0[1] == 0xBB );
        REQUIRE( read0[2] == 0xCC );
        REQUIRE( read0[3] == 0xDD );

        // Verify page 1: should be 44 55 66 77
        uint8_t read1[4];
        REQUIRE( nv.read( 256, read1, sizeof( read1 ), sizeof( read1 ) ) == true );
        REQUIRE( read1[0] == 0x44 );
        REQUIRE( read1[1] == 0x55 );
        REQUIRE( read1[2] == 0x66 );
        REQUIRE( read1[3] == 0x77 );

        // Verify page 2: should be EE FF
        uint8_t read2[2];
        REQUIRE( nv.read( 512, read2, sizeof( read2 ), sizeof( read2 ) ) == true );
        REQUIRE( read2[0] == 0xEE );
        REQUIRE( read2[1] == 0xFF );

        REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
    }
}


TEST_CASE( "NV Flash 4K - Cross-Page Write" )
{
    MockFlash4K mockFlash;
    mockFlash.start();
    NvFlash4K nv( mockFlash, CONFIG_4K );
    nv.start();
    ShutdownUnitTesting::clearAndUseCounter();

    SECTION( "write spanning two pages" )
    {
        // Write 20 bytes starting at offset 250 (crosses page 0/page 1 boundary)
        uint8_t writeData[20];
        for ( uint8_t i = 0; i < 20; i++ )
        {
            writeData[i] = 0xA0 + i;
        }
        REQUIRE( nv.write( 250, writeData, sizeof( writeData ) ) == true );

        // Read back from page 0 (last 6 bytes)
        uint8_t read0[6];
        REQUIRE( nv.read( 250, read0, 6, 6 ) == true );
        REQUIRE( read0[0] == 0xA0 );
        REQUIRE( read0[1] == 0xA1 );
        REQUIRE( read0[2] == 0xA2 );
        REQUIRE( read0[3] == 0xA3 );
        REQUIRE( read0[4] == 0xA4 );
        REQUIRE( read0[5] == 0xA5 );

        // Read back from page 1 (first 14 bytes)
        uint8_t read1[14];
        REQUIRE( nv.read( 256, read1, 14, 14 ) == true );
        REQUIRE( read1[0] == 0xA6 );
        REQUIRE( read1[1] == 0xA7 );

        // Read the full 20 bytes as one operation
        uint8_t readFull[20];
        REQUIRE( nv.read( 250, readFull, 20, 20 ) == true );
        REQUIRE( memcmp( readFull, writeData, 20 ) == 0 );

        REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
    }
}


TEST_CASE( "NV Flash 4K - Format" )
{
    MockFlash4K mockFlash;
    mockFlash.start();
    NvFlash4K nv( mockFlash, CONFIG_4K );
    nv.start();
    ShutdownUnitTesting::clearAndUseCounter();

    SECTION( "format erases all data" )
    {
        // Write some data
        uint8_t writeData[] = { 0x42, 0x43, 0x44 };
        REQUIRE( nv.write( 0, writeData, sizeof( writeData ) ) == true );

        // Format
        REQUIRE( nv.format() == true );

        // Read should return 0xFF (erased state)
        uint8_t readData[3];
        REQUIRE( nv.read( 0, readData, sizeof( readData ), sizeof( readData ) ) == true );
        REQUIRE( readData[0] == 0xFF );
        REQUIRE( readData[1] == 0xFF );
        REQUIRE( readData[2] == 0xFF );

        REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
    }

    SECTION( "format allows new writes" )
    {
        uint8_t data1[] = { 0x10, 0x20 };
        REQUIRE( nv.write( 0, data1, sizeof( data1 ) ) == true );

        REQUIRE( nv.format() == true );

        uint8_t data2[] = { 0x30, 0x40 };
        REQUIRE( nv.write( 0, data2, sizeof( data2 ) ) == true );

        uint8_t readData[2];
        REQUIRE( nv.read( 0, readData, sizeof( readData ), sizeof( readData ) ) == true );
        REQUIRE( readData[0] == 0x30 );
        REQUIRE( readData[1] == 0x40 );

        REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
    }
}


TEST_CASE( "NV Flash 4K - Startup Scan Rebuilds Page Map" )
{
    MockFlash4K mockFlash;
    mockFlash.start();
    ShutdownUnitTesting::clearAndUseCounter();

    SECTION( "page map is rebuilt after restart" )
    {
        // First session: write data
        {
            NvFlash4K nv( mockFlash, CONFIG_4K );
            nv.start();

            uint8_t data1[] = { 0xAA, 0xBB, 0xCC };
            REQUIRE( nv.write( 0, data1, sizeof( data1 ) ) == true );

            uint8_t data2[] = { 0x11, 0x22 };
            REQUIRE( nv.write( 256, data2, sizeof( data2 ) ) == true );

            nv.stop();
        }

        // Second session: data should persist
        {
            NvFlash4K nv( mockFlash, CONFIG_4K );
            nv.start();

            uint8_t read0[3];
            REQUIRE( nv.read( 0, read0, sizeof( read0 ), sizeof( read0 ) ) == true );
            REQUIRE( read0[0] == 0xAA );
            REQUIRE( read0[1] == 0xBB );
            REQUIRE( read0[2] == 0xCC );

            uint8_t read1[2];
            REQUIRE( nv.read( 256, read1, sizeof( read1 ), sizeof( read1 ) ) == true );
            REQUIRE( read1[0] == 0x11 );
            REQUIRE( read1[1] == 0x22 );

            nv.stop();
        }

        REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
    }

    SECTION( "restart finds latest version after multiple writes" )
    {
        // Write multiple versions
        {
            NvFlash4K nv( mockFlash, CONFIG_4K );
            nv.start();

            uint8_t v1[] = { 0x01 };
            REQUIRE( nv.write( 0, v1, 1 ) == true );

            uint8_t v2[] = { 0x02 };
            REQUIRE( nv.write( 0, v2, 1 ) == true );

            uint8_t v3[] = { 0x03 };
            REQUIRE( nv.write( 0, v3, 1 ) == true );

            nv.stop();
        }

        // Restart: should find version 3
        {
            NvFlash4K nv( mockFlash, CONFIG_4K );
            nv.start();

            uint8_t readData[1];
            REQUIRE( nv.read( 0, readData, 1, 1 ) == true );
            REQUIRE( readData[0] == 0x03 );

            nv.stop();
        }

        REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
    }
}


TEST_CASE( "NV Flash 4K - Sector Reclamation" )
{
    // Use a smaller flash to trigger reclamation sooner
    // 2 sectors = 8192 bytes, 14 record slots per sector = 28 total slots
    static constexpr size_t SMALL_FLASH_SIZE = 8192;
    static const NV::Flash::Config_T SMALL_CONFIG = {
        0,                  // flashStartAddress
        SMALL_FLASH_SIZE,   // flashEndAddress
        256,                // nvPageSize
        256                 // nvTotalSize (1 logical page)
    };

    Flash::TestSupport::MockFlash<SMALL_FLASH_SIZE> mockFlash;
    mockFlash.start();
    NV::Flash::Api<1> nv( mockFlash, SMALL_CONFIG );
    nv.start();
    ShutdownUnitTesting::clearAndUseCounter();

    SECTION( "driver handles many writes requiring reclamation" )
    {
        // With 28 record slots and 1 logical page, the 29th write requires
        // sector reclamation
        uint8_t data[4];
        for ( size_t i = 0; i < 40; i++ )
        {
            data[0] = static_cast<uint8_t>( i );
            data[1] = static_cast<uint8_t>( i + 1 );
            data[2] = static_cast<uint8_t>( i + 2 );
            data[3] = static_cast<uint8_t>( i + 3 );
            REQUIRE( nv.write( 0, data, sizeof( data ) ) == true );
        }

        // Verify last write is readable
        uint8_t readData[4];
        REQUIRE( nv.read( 0, readData, sizeof( readData ), sizeof( readData ) ) == true );
        REQUIRE( readData[0] == 39 );
        REQUIRE( readData[1] == 40 );
        REQUIRE( readData[2] == 41 );
        REQUIRE( readData[3] == 42 );

        // At least one sector erase should have occurred
        REQUIRE( mockFlash.getEraseCount() > 0 );

        REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
    }
}


TEST_CASE( "NV Flash 4K - Boundary Conditions" )
{
    MockFlash4K mockFlash;
    mockFlash.start();
    NvFlash4K nv( mockFlash, CONFIG_4K );
    nv.start();
    ShutdownUnitTesting::clearAndUseCounter();

    SECTION( "write at end of NV space" )
    {
        uint8_t data[] = { 0xFE, 0xFD };
        size_t  offset = NV_TOTAL_SIZE_4K - sizeof( data );
        REQUIRE( nv.write( offset, data, sizeof( data ) ) == true );

        uint8_t readData[2];
        REQUIRE( nv.read( offset, readData, sizeof( readData ), sizeof( readData ) ) == true );
        REQUIRE( readData[0] == 0xFE );
        REQUIRE( readData[1] == 0xFD );
        REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
    }

    SECTION( "write beyond NV space fails" )
    {
        uint8_t data[] = { 0x01 };
        REQUIRE( nv.write( NV_TOTAL_SIZE_4K, data, 1 ) == false );
        REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
    }

    SECTION( "read beyond NV space fails" )
    {
        uint8_t data[1];
        REQUIRE( nv.read( NV_TOTAL_SIZE_4K, data, 1, 1 ) == false );
        REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
    }

    SECTION( "zero-length write fails" )
    {
        uint8_t data[] = { 0x01 };
        REQUIRE( nv.write( 0, data, 0 ) == false );
        REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
    }

    SECTION( "null pointer write fails" )
    {
        REQUIRE( nv.write( 0, nullptr, 10 ) == false );
        REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
    }

    SECTION( "operations on stopped driver fail" )
    {
        NvFlash4K nv2( mockFlash, CONFIG_4K );
        // Don't start it
        uint8_t data[] = { 0x01 };
        REQUIRE( nv2.write( 0, data, 1 ) == false );
        REQUIRE( nv2.read( 0, data, 1, 1 ) == false );
        REQUIRE( nv2.format() == false );
        REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
    }
}


TEST_CASE( "NV Flash 4K - Statistics" )
{
    MockFlash4K mockFlash;
    mockFlash.start();
    NvFlash4K nv( mockFlash, CONFIG_4K );
    nv.start();
    ShutdownUnitTesting::clearAndUseCounter();

    SECTION( "initial statistics show all free" )
    {
        uint32_t eraseCount;
        size_t   freePages;
        size_t   validPages;
        REQUIRE( nv.getStatistics( eraseCount, freePages, validPages ) == true );
        REQUIRE( eraseCount == 0 );
        REQUIRE( validPages == 0 );
        REQUIRE( freePages > 0 );
        REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
    }

    SECTION( "statistics update after writes" )
    {
        uint8_t data[] = { 0x01, 0x02, 0x03 };
        REQUIRE( nv.write( 0, data, sizeof( data ) ) == true );
        REQUIRE( nv.write( 256, data, sizeof( data ) ) == true );

        uint32_t eraseCount;
        size_t   freePages;
        size_t   validPages;
        REQUIRE( nv.getStatistics( eraseCount, freePages, validPages ) == true );
        REQUIRE( validPages == 2 );
        REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
    }
}


TEST_CASE( "NV Flash 4K - All Logical Pages" )
{
    MockFlash4K mockFlash;
    mockFlash.start();
    NvFlash4K nv( mockFlash, CONFIG_4K );
    nv.start();
    ShutdownUnitTesting::clearAndUseCounter();

    SECTION( "write and read all 16 logical pages" )
    {
        // Write a unique byte to each logical page
        for ( size_t page = 0; page < MAX_LOGICAL_PAGES_4K; page++ )
        {
            uint8_t data = static_cast<uint8_t>( page + 0x40 );
            size_t  offset = page * NV_PAGE_SIZE_4K;
            REQUIRE( nv.write( offset, &data, 1 ) == true );
        }

        // Read back and verify each page
        for ( size_t page = 0; page < MAX_LOGICAL_PAGES_4K; page++ )
        {
            uint8_t data;
            size_t  offset = page * NV_PAGE_SIZE_4K;
            REQUIRE( nv.read( offset, &data, 1, 1 ) == true );
            REQUIRE( data == static_cast<uint8_t>( page + 0x40 ) );
        }

        REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
    }
}


// ============================================================================
// 128KB Configuration Tests
// ============================================================================

TEST_CASE( "NV Flash 128K - Initialization" )
{
    MockFlash128K mockFlash;
    mockFlash.start();
    NvFlash128K nv( mockFlash, CONFIG_128K );
    ShutdownUnitTesting::clearAndUseCounter();

    SECTION( "start succeeds with large configuration" )
    {
        REQUIRE( nv.start() == true );
        REQUIRE( nv.getTotalSize() == NV_TOTAL_SIZE_128K );
        REQUIRE( nv.getNumPages() == MAX_LOGICAL_PAGES_128K );
        REQUIRE( nv.getPageSize() == NV_PAGE_SIZE_128K );
        REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
    }
}


TEST_CASE( "NV Flash 128K - Basic Read/Write" )
{
    MockFlash128K mockFlash;
    mockFlash.start();
    NvFlash128K nv( mockFlash, CONFIG_128K );
    nv.start();
    ShutdownUnitTesting::clearAndUseCounter();

    SECTION( "write and read at beginning" )
    {
        uint8_t writeData[] = { 0x10, 0x20, 0x30 };
        REQUIRE( nv.write( 0, writeData, sizeof( writeData ) ) == true );

        uint8_t readData[3];
        REQUIRE( nv.read( 0, readData, sizeof( readData ), sizeof( readData ) ) == true );
        REQUIRE( memcmp( readData, writeData, sizeof( writeData ) ) == 0 );
        REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
    }

    SECTION( "write and read at end of NV space" )
    {
        uint8_t writeData[] = { 0xAA, 0xBB };
        size_t  offset = NV_TOTAL_SIZE_128K - sizeof( writeData );
        REQUIRE( nv.write( offset, writeData, sizeof( writeData ) ) == true );

        uint8_t readData[2];
        REQUIRE( nv.read( offset, readData, sizeof( readData ), sizeof( readData ) ) == true );
        REQUIRE( memcmp( readData, writeData, sizeof( writeData ) ) == 0 );
        REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
    }

    SECTION( "write to high-numbered page" )
    {
        // Page 500 (offset = 500 * 256 = 128000)
        size_t  offset = 500 * NV_PAGE_SIZE_128K;
        uint8_t writeData[] = { 0xDE, 0xAD };
        REQUIRE( nv.write( offset, writeData, sizeof( writeData ) ) == true );

        uint8_t readData[2];
        REQUIRE( nv.read( offset, readData, sizeof( readData ), sizeof( readData ) ) == true );
        REQUIRE( readData[0] == 0xDE );
        REQUIRE( readData[1] == 0xAD );
        REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
    }
}


TEST_CASE( "NV Flash 128K - Scatter Writes Across Wide Range" )
{
    MockFlash128K mockFlash;
    mockFlash.start();
    NvFlash128K nv( mockFlash, CONFIG_128K );
    nv.start();
    ShutdownUnitTesting::clearAndUseCounter();

    SECTION( "writes to pages spread across NV space" )
    {
        // Write to page 0, 100, 200, 300, 400, 511
        size_t pages[] = { 0, 100, 200, 300, 400, 511 };
        for ( size_t i = 0; i < 6; i++ )
        {
            uint8_t data = static_cast<uint8_t>( pages[i] & 0xFF );
            size_t  offset = pages[i] * NV_PAGE_SIZE_128K;
            REQUIRE( nv.write( offset, &data, 1 ) == true );
        }

        // Verify each
        for ( size_t i = 0; i < 6; i++ )
        {
            uint8_t data;
            size_t  offset = pages[i] * NV_PAGE_SIZE_128K;
            REQUIRE( nv.read( offset, &data, 1, 1 ) == true );
            REQUIRE( data == static_cast<uint8_t>( pages[i] & 0xFF ) );
        }

        REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
    }
}


TEST_CASE( "NV Flash 128K - Restart Persistence" )
{
    MockFlash128K mockFlash;
    mockFlash.start();
    ShutdownUnitTesting::clearAndUseCounter();

    SECTION( "data persists across driver restart" )
    {
        // Session 1
        {
            NvFlash128K nv( mockFlash, CONFIG_128K );
            nv.start();

            uint8_t data[] = { 0xCA, 0xFE };
            REQUIRE( nv.write( 50000, data, sizeof( data ) ) == true );

            nv.stop();
        }

        // Session 2
        {
            NvFlash128K nv( mockFlash, CONFIG_128K );
            nv.start();

            uint8_t readData[2];
            REQUIRE( nv.read( 50000, readData, sizeof( readData ), sizeof( readData ) ) == true );
            REQUIRE( readData[0] == 0xCA );
            REQUIRE( readData[1] == 0xFE );

            nv.stop();
        }

        REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
    }
}
