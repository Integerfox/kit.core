/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file
    Hardware test for Kit::Driver::Flash.  Exercises the Flash driver by
    performing erase / write / read-back operations against a real flash
    device.

    This test is platform-independent: it operates solely on the
    Kit::Driver::Flash::IApi interface so that it can be re-used for all
    concrete Flash drivers.  The caller (e.g. a platform-specific main())
    is responsible for creating/starting the concrete driver, creating the
    thread this runs in, and starting the OSAL scheduler.

    The test exercises:
      - Device geometry reporting (size, sector size, page size, sector count)
      - Single-sector erase and blank (0xFF) verification
      - Basic write and read-back
      - A write that crosses a flash write-page boundary
      - Multi-sector erase
      - waitUntilReady()

    WARNING: This test is DESTRUCTIVE.  It erases and re-writes the first few
             sectors of the flash device.

    A PASS/FAIL summary is reported via the trace/console.  When finished the
    debug LED blinks as a visual "test complete" heartbeat.
*/

#include "test.h"
#include "Kit/System/Api.h"
#include "Kit/System/Trace.h"
#include "Kit/Bsp/Api.h"
#include <string.h>

#define SECT_ "_0test"

///
using namespace Kit::System;
using Kit::Driver::Flash::IApi;


////////////////////////////////////////////////////////////////////////////////

/// Number of payload bytes used by the write/read-back tests
static constexpr size_t TEST_PATTERN_SIZE = 32;

/// Running pass/fail tallies
static unsigned testCount_ = 0;
static unsigned passCount_ = 0;
static unsigned failCount_ = 0;

/// Records the result of a single check and traces it
static void check( bool condition, const char* description )
{
    testCount_++;
    if ( condition )
    {
        passCount_++;
        KIT_SYSTEM_TRACE_MSG( SECT_, "  PASS: %s", description );
    }
    else
    {
        failCount_++;
        KIT_SYSTEM_TRACE_MSG( SECT_, "  FAIL: %s", description );
    }
}

/// Returns true if every byte in 'buffer' equals 'value'
static bool allBytesAre( const uint8_t* buffer, size_t numBytes, uint8_t value )
{
    for ( size_t i = 0; i < numBytes; i++ )
    {
        if ( buffer[i] != value )
        {
            return false;
        }
    }
    return true;
}


////////////////////////////////////////////////////////////////////////////////
void runtests( IApi& flash )
{
    KIT_SYSTEM_TRACE_MSG( SECT_, "========================================" );
    KIT_SYSTEM_TRACE_MSG( SECT_, "         FLASH HARDWARE TEST" );
    KIT_SYSTEM_TRACE_MSG( SECT_, "========================================" );

    // --- Device geometry ---
    const size_t totalSize  = flash.getTotalSize();
    const size_t sectorSize = flash.getSectorSize();
    const size_t writePage  = flash.getWritePageSize();
    const size_t numSectors = flash.getNumSectors();
    KIT_SYSTEM_TRACE_MSG( SECT_, "--- Device Geometry ---" );
    KIT_SYSTEM_TRACE_MSG( SECT_, "  totalSize     = %lu", (unsigned long)totalSize );
    KIT_SYSTEM_TRACE_MSG( SECT_, "  sectorSize    = %lu", (unsigned long)sectorSize );
    KIT_SYSTEM_TRACE_MSG( SECT_, "  writePageSize = %lu", (unsigned long)writePage );
    KIT_SYSTEM_TRACE_MSG( SECT_, "  numSectors    = %lu", (unsigned long)numSectors );
    check( sectorSize > 0, "sectorSize is non-zero" );
    check( writePage > 0, "writePageSize is non-zero" );
    check( numSectors > 0, "numSectors is non-zero" );
    check( totalSize == sectorSize * numSectors, "totalSize == sectorSize * numSectors" );

    // A small stack buffer sized to a write page (256 bytes on W25Q).  Bounded
    // to keep the stack footprint reasonable if a driver reports a large page.
    static uint8_t readBuf[512];
    const size_t   readChunk = ( sectorSize < sizeof( readBuf ) ) ? sectorSize : sizeof( readBuf );

    // --- Test 1: Erase sector 0 and verify blank ---
    KIT_SYSTEM_TRACE_MSG( SECT_, "--- Test 1: Erase + blank check (sector 0) ---" );
    {
        check( flash.eraseSector( 0, 1 ) == IApi::SUCCESS, "eraseSector(0,1) succeeds" );
        check( flash.waitUntilReady() == true, "waitUntilReady after erase" );

        memset( readBuf, 0x00, readChunk );
        check( flash.read( 0, readBuf, readChunk ) == IApi::SUCCESS, "read after erase succeeds" );
        check( allBytesAre( readBuf, readChunk, 0xFF ), "erased sector reads as 0xFF" );
    }

    // --- Test 2: Basic write / read-back ---
    KIT_SYSTEM_TRACE_MSG( SECT_, "--- Test 2: Write / read-back ---" );
    {
        uint8_t writeData[TEST_PATTERN_SIZE];
        for ( size_t i = 0; i < TEST_PATTERN_SIZE; i++ )
        {
            writeData[i] = static_cast<uint8_t>( 0xA0 + i );
        }
        check( flash.write( 0, writeData, TEST_PATTERN_SIZE ) == IApi::SUCCESS, "write pattern succeeds" );
        check( flash.waitUntilReady() == true, "waitUntilReady after write" );

        memset( readBuf, 0x00, TEST_PATTERN_SIZE );
        check( flash.read( 0, readBuf, TEST_PATTERN_SIZE ) == IApi::SUCCESS, "read-back succeeds" );
        check( memcmp( readBuf, writeData, TEST_PATTERN_SIZE ) == 0, "read-back matches written data" );
    }

    // --- Test 3: Write that crosses a write-page boundary ---
    KIT_SYSTEM_TRACE_MSG( SECT_, "--- Test 3: Page-crossing write ---" );
    if ( writePage >= 4 && ( writePage + 4 ) <= sectorSize )
    {
        // Erase a fresh sector (sector 1) so the region is blank first
        const size_t sector1Addr = sectorSize;
        check( flash.eraseSector( sector1Addr, 1 ) == IApi::SUCCESS, "eraseSector(sector 1) succeeds" );
        check( flash.waitUntilReady() == true, "waitUntilReady after erase (sector 1)" );

        // Straddle the boundary between write page 0 and write page 1
        const size_t crossOffset = sector1Addr + writePage - 4;
        uint8_t      crossData[8];
        for ( size_t i = 0; i < sizeof( crossData ); i++ )
        {
            crossData[i] = static_cast<uint8_t>( 0x50 + i );
        }
        check( flash.write( crossOffset, crossData, sizeof( crossData ) ) == IApi::SUCCESS, "page-crossing write succeeds" );
        check( flash.waitUntilReady() == true, "waitUntilReady after page-crossing write" );

        uint8_t crossRead[8];
        memset( crossRead, 0x00, sizeof( crossRead ) );
        check( flash.read( crossOffset, crossRead, sizeof( crossRead ) ) == IApi::SUCCESS, "page-crossing read-back succeeds" );
        check( memcmp( crossRead, crossData, sizeof( crossData ) ) == 0, "page-crossing data intact across boundary" );
    }
    else
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "  SKIP: geometry too small for page-crossing test" );
    }

    // --- Test 4: Multi-sector erase ---
    KIT_SYSTEM_TRACE_MSG( SECT_, "--- Test 4: Multi-sector erase ---" );
    if ( numSectors >= 2 )
    {
        check( flash.eraseSector( 0, 2 ) == IApi::SUCCESS, "eraseSector(0,2) succeeds" );
        check( flash.waitUntilReady() == true, "waitUntilReady after multi-sector erase" );

        memset( readBuf, 0x00, readChunk );
        check( flash.read( 0, readBuf, readChunk ) == IApi::SUCCESS, "read sector 0 after multi-erase" );
        check( allBytesAre( readBuf, readChunk, 0xFF ), "sector 0 blank after multi-erase" );

        memset( readBuf, 0x00, readChunk );
        check( flash.read( sectorSize, readBuf, readChunk ) == IApi::SUCCESS, "read sector 1 after multi-erase" );
        check( allBytesAre( readBuf, readChunk, 0xFF ), "sector 1 blank after multi-erase" );
    }
    else
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "  SKIP: device has fewer than 2 sectors" );
    }

    // --- Summary ---
    KIT_SYSTEM_TRACE_MSG( SECT_, "========================================" );
    KIT_SYSTEM_TRACE_MSG( SECT_, "  RESULTS: %u passed, %u failed (of %u)", passCount_, failCount_, testCount_ );
    KIT_SYSTEM_TRACE_MSG( SECT_, "  OVERALL: %s", ( failCount_ == 0 ) ? "PASS" : "FAIL" );
    KIT_SYSTEM_TRACE_MSG( SECT_, "========================================" );

    // Visual "test complete" heartbeat.  A steady blink indicates the test ran
    // to completion; consult the console for the PASS/FAIL summary.
    for ( ;; )
    {
        Bsp_toggle_debug1();
        sleep( 500 );
    }
}
