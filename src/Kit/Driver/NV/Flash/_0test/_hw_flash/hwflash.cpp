/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file

    Hardware test for the NV Flash driver on an STM32F4 target with a
    W25Q SPI NOR flash device.

    This test exercises the NV flash driver using the real W25Q hardware
    driver over SPI.  It verifies:
      - Flash device communication (JEDEC ID read)
      - Basic NV write and read back
      - Read-modify-write preservation
      - Multi-page write isolation
      - Data persistence across driver stop/start
      - Format (factory reset)
      - Driver statistics

    The test reports results via the serial console (UART).  A PASS/FAIL
    summary is printed at the end.

    The flash region used by this test starts at address 0 and uses 32
    sectors (128 KB) to avoid conflicting with other flash contents.
*/


#include "Kit/Bsp/Api.h"
#include "Kit/System/Api.h"
#include "Kit/System/Shutdown.h"
#include "Kit/System/Thread.h"
#include "Kit/System/FatalError.h"
#include "Kit/System/Trace.h"
#include "Kit/Driver/SPI/ST/M32F4/Api.h"
#include "Kit/Driver/Dio/ST/M32F4/Output.h"
#include "Kit/Driver/Flash/W25Q/Api.h"
#include "Kit/Driver/NV/Flash/Api.h"
#include "spi.h"
#include <cstring>

#define SECT_ "_0test"

///
using namespace Kit::Driver;
using namespace Kit::System;


// ============================================================================
// Test Configuration
// ============================================================================

/** NV region: 128 KB of flash (32 sectors), 4 KB application data, 256-byte pages */
static constexpr size_t   NV_PAGE_SIZE      = 256;
static constexpr size_t   NV_TOTAL_SIZE     = 4096;
static constexpr size_t   MAX_LOGICAL_PAGES = 16;
static constexpr uint32_t FLASH_START_ADDR  = 0;
static constexpr uint32_t FLASH_END_ADDR    = 128 * 1024;

static const NV::Flash::Config_T NV_CONFIG = {
    FLASH_START_ADDR,
    FLASH_END_ADDR,
    NV_PAGE_SIZE,
    NV_TOTAL_SIZE
};


// ============================================================================
// Test Helpers
// ============================================================================

/** Winbond manufacturer JEDEC ID */
static constexpr uint8_t WINBOND_MANUFACTURER_ID = 0xEF;

/** LED blink period in milliseconds */
static constexpr uint32_t LED_BLINK_PERIOD_MS = 500;

static unsigned testCount_  = 0;
static unsigned passCount_  = 0;
static unsigned failCount_  = 0;

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


// ============================================================================
// Test Thread Runnable
// ============================================================================

/** Main test runnable.  Runs inside a FreeRTOS thread so that the UART
    console, Kit::System::sleep(), and trace output all function correctly.
 */
class TestRunnable : public IRunnable
{
public:
    void entry() noexcept override
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "========================================" );
        KIT_SYSTEM_TRACE_MSG( SECT_, "       NV FLASH HARDWARE TEST" );
        KIT_SYSTEM_TRACE_MSG( SECT_, "========================================" );

        // --- Initialize hardware drivers ---
        KIT_SYSTEM_TRACE_MSG( SECT_, "Initializing SPI and flash drivers..." );
        SPI::ST::M32F4::Api    spiDriver( &hspi3 );
        Dio::ST::M32F4::Output csPin( CS_SPI_Flash_GPIO_Port, CS_SPI_Flash_Pin );
        spiDriver.start();

        Flash::W25Q::Api flashDriver( spiDriver, csPin, Flash::W25Q::W25Q128 );
        flashDriver.start();
        KIT_SYSTEM_TRACE_MSG( SECT_, "Drivers initialized" );

        // --- Test 1: JEDEC ID ---
        KIT_SYSTEM_TRACE_MSG( SECT_, "--- Test 1: JEDEC ID ---" );
        {
            uint8_t mfgId    = 0;
            uint8_t memType  = 0;
            uint8_t capacity = 0;
            bool ok = flashDriver.readJedecId( mfgId, memType, capacity );
            check( ok, "readJedecId succeeds" );
            check( mfgId == WINBOND_MANUFACTURER_ID, "Manufacturer ID is Winbond (0xEF)" );
            KIT_SYSTEM_TRACE_MSG( SECT_,
                "  JEDEC: mfg=0x%02X type=0x%02X cap=0x%02X",
                mfgId, memType, capacity );
        }

        // --- Test 2: Format and Initialize NV ---
        KIT_SYSTEM_TRACE_MSG( SECT_, "--- Test 2: Format and Init ---" );
        NV::Flash::Api<MAX_LOGICAL_PAGES> nv( flashDriver, NV_CONFIG );
        {
            check( nv.start() == true, "NV driver start succeeds" );
            check( nv.format() == true, "Format succeeds" );
            check( nv.getTotalSize() == NV_TOTAL_SIZE, "Total size is correct" );
            check( nv.getNumPages() == MAX_LOGICAL_PAGES, "Num pages is correct" );
            check( nv.getPageSize() == NV_PAGE_SIZE, "Page size is correct" );
        }

        // --- Test 3: Basic Write/Read ---
        KIT_SYSTEM_TRACE_MSG( SECT_, "--- Test 3: Basic Write/Read ---" );
        {
            uint8_t writeData[] = { 0x01, 0x02, 0x03, 0x04, 0x05 };
            check( nv.write( 0, writeData, sizeof( writeData ) ) == true, "Write 5 bytes succeeds" );

            uint8_t readData[5] = { 0 };
            check( nv.read( 0, readData, sizeof( readData ), sizeof( readData ) ) == true, "Read 5 bytes succeeds" );
            check( memcmp( readData, writeData, sizeof( writeData ) ) == 0, "Read data matches written data" );
            KIT_SYSTEM_TRACE_MSG( SECT_,
                "  Written: [%02X %02X %02X %02X %02X]",
                writeData[0], writeData[1], writeData[2], writeData[3], writeData[4] );
            KIT_SYSTEM_TRACE_MSG( SECT_,
                "  Read:    [%02X %02X %02X %02X %02X]",
                readData[0], readData[1], readData[2], readData[3], readData[4] );
        }

        // --- Test 4: Read-Modify-Write ---
        KIT_SYSTEM_TRACE_MSG( SECT_, "--- Test 4: Read-Modify-Write ---" );
        {
            // Overwrite first 2 bytes, leaving bytes 2-4 intact
            uint8_t data2[] = { 0xAA, 0xBB };
            check( nv.write( 0, data2, sizeof( data2 ) ) == true, "Partial overwrite succeeds" );

            uint8_t readData[5] = { 0 };
            check( nv.read( 0, readData, sizeof( readData ), sizeof( readData ) ) == true, "Read after overwrite succeeds" );
            check( readData[0] == 0xAA, "Byte 0 updated correctly" );
            check( readData[1] == 0xBB, "Byte 1 updated correctly" );
            check( readData[2] == 0x03, "Byte 2 preserved" );
            check( readData[3] == 0x04, "Byte 3 preserved" );
            check( readData[4] == 0x05, "Byte 4 preserved" );
        }

        // --- Test 5: Multi-Page Write ---
        KIT_SYSTEM_TRACE_MSG( SECT_, "--- Test 5: Multi-Page Write ---" );
        {
            uint8_t dataA[] = { 0x11, 0x22 };
            check( nv.write( 256, dataA, sizeof( dataA ) ) == true, "Write page 1 succeeds" );

            uint8_t dataB[] = { 0x33, 0x44 };
            check( nv.write( 512, dataB, sizeof( dataB ) ) == true, "Write page 2 succeeds" );

            uint8_t readA[2] = { 0 };
            check( nv.read( 256, readA, sizeof( readA ), sizeof( readA ) ) == true, "Read page 1 succeeds" );
            check( readA[0] == 0x11 && readA[1] == 0x22, "Page 1 data correct" );

            uint8_t readB[2] = { 0 };
            check( nv.read( 512, readB, sizeof( readB ), sizeof( readB ) ) == true, "Read page 2 succeeds" );
            check( readB[0] == 0x33 && readB[1] == 0x44, "Page 2 data correct" );
        }

        // --- Test 6: Stop/Start Persistence ---
        KIT_SYSTEM_TRACE_MSG( SECT_, "--- Test 6: Stop/Start Persistence ---" );
        {
            nv.stop();
            check( nv.start() == true, "NV driver restart succeeds" );

            // Verify data from test 3/4 survived
            uint8_t readData[5] = { 0 };
            check( nv.read( 0, readData, sizeof( readData ), sizeof( readData ) ) == true, "Read after restart succeeds" );
            check( readData[0] == 0xAA, "Byte 0 persisted" );
            check( readData[1] == 0xBB, "Byte 1 persisted" );
            check( readData[2] == 0x03, "Byte 2 persisted" );

            // Verify page 1 data survived
            uint8_t readA[2] = { 0 };
            check( nv.read( 256, readA, sizeof( readA ), sizeof( readA ) ) == true, "Read page 1 after restart succeeds" );
            check( readA[0] == 0x11 && readA[1] == 0x22, "Page 1 data persisted" );
        }

        // --- Test 7: Statistics ---
        KIT_SYSTEM_TRACE_MSG( SECT_, "--- Test 7: Statistics ---" );
        {
            uint32_t eraseCount;
            size_t   freePages;
            size_t   validPages;
            check( nv.getStatistics( eraseCount, freePages, validPages ) == true, "getStatistics succeeds" );
            check( validPages > 0, "Has valid pages" );
            check( freePages > 0, "Has free pages" );
            KIT_SYSTEM_TRACE_MSG( SECT_,
                "  Stats: erases=%lu free=%u valid=%u",
                (unsigned long) eraseCount, (unsigned) freePages, (unsigned) validPages );
        }

        // --- Summary ---
        KIT_SYSTEM_TRACE_MSG( SECT_, "========================================" );
        KIT_SYSTEM_TRACE_MSG( SECT_, "=== Results: %u/%u passed, %u failed ===",
            passCount_, testCount_, failCount_ );

        if ( failCount_ == 0 )
        {
            KIT_SYSTEM_TRACE_MSG( SECT_, "=== ALL TESTS PASSED ===" );
            Bsp_turn_on_debug1();
            Bsp_turn_on_debug2();
        }
        else
        {
            KIT_SYSTEM_TRACE_MSG( SECT_, "=== SOME TESTS FAILED ===" );
        }
        KIT_SYSTEM_TRACE_MSG( SECT_, "========================================" );

        // Blink LED to indicate completion (or rapid blink for failure)
        uint32_t blinkPeriod = ( failCount_ == 0 ) ? LED_BLINK_PERIOD_MS : 100;
        for ( ;; )
        {
            Bsp_toggle_debug1();
            sleep( blinkPeriod );
        }
    }
};


// ============================================================================
// Static objects
// ============================================================================

static TestRunnable testRunnable_;


// ============================================================================
// Entry point called from main.cpp
// ============================================================================

void runTests()
{
    // Create a FreeRTOS thread for the test.  The UART console and
    // Kit::System::sleep() require the FreeRTOS scheduler to be running.
    auto* testThread = Thread::create( testRunnable_, "NV_TEST" );
    if ( !testThread )
    {
        FatalError::logf( Shutdown::eFAILURE, "Failed to create test thread" );
        return;
    }

    // Start the FreeRTOS scheduler — control transfers to testRunnable_.entry()
    enableScheduling();

    // Should never reach here
}
