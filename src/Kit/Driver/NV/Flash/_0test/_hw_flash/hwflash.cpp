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
      - Wear leveling and sector reclamation
      - Startup scan time

    The test reports results via the serial console (UART).  A PASS/FAIL
    summary is printed at the end.

    The flash region used by this test starts at address 0 and uses 32
    sectors (128 KB) to avoid conflicting with other flash contents.

    Configuration Sizing (from design documentation):

    | Parameter                | 4KB App Data      | 128KB App Data      |
    |--------------------------|-------------------|---------------------|
    | Physical Flash Required  | 128KB (32 sectors)| 4MB (1024 sectors)  |
    | Page Map RAM             | 64 bytes          | 2048 bytes (2KB)    |
    | Startup Scan Time        | ~16-25 ms         | ~500-625 ms         |
    | Over-provisioning Ratio  | 28:1              | 28:1                |
    | Total Record Slots       | 448               | 14,336              |

    This hardware test uses the 4KB configuration.
*/


#include "Kit/Bsp/Api.h"
#include "Kit/System/Api.h"
#include "Kit/System/Shutdown.h"
#include "Kit/System/Thread.h"
#include "Kit/System/FatalError.h"
#include "Kit/System/Trace.h"
#include "Kit/System/ElapsedTime.h"
#include "Kit/Driver/SPI/ST/M32F4/Polled.h"
#include "Kit/Driver/Dio/ST/M32F4/Output.h"
#include "Kit/Driver/Flash/W25Q/Api.h"
#include "Kit/Driver/NV/Flash/Api.h"
#include "Kit/Checksum/Crc32EthernetFast.h"
#include "spi.h"
#include <cstring>

#define SECT_ "_0test"

///
using namespace Kit::Driver;
using namespace Kit::System;


// ============================================================================
// Test Configuration
// ============================================================================

/** NV region: 128 KB of flash (32 sectors), 4 KB application data, 256-byte pages.
    Over-provisioning ratio: 28:1 (448 record slots / 16 logical pages).
    Page map RAM: 16 * 4 = 64 bytes.
 */
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

/** Expected sizing metrics from the design documentation */
static constexpr size_t EXPECTED_SECTORS            = 32;
static constexpr size_t EXPECTED_RECORDS_PER_SECTOR = 14;
static constexpr size_t EXPECTED_TOTAL_RECORD_SLOTS = EXPECTED_SECTORS * EXPECTED_RECORDS_PER_SECTOR;  // 448
static constexpr size_t EXPECTED_PAGE_MAP_RAM_BYTES = MAX_LOGICAL_PAGES * sizeof( uint32_t );          // 64

/** Maximum acceptable startup scan time in milliseconds.
    Design doc estimates ~16-25 ms for 4KB config (448 headers at ~35us each).
 */
static constexpr uint32_t MAX_STARTUP_SCAN_TIME_MS = 100;


// ============================================================================
// Test Helpers
// ============================================================================

/** Winbond manufacturer JEDEC ID */
static constexpr uint8_t WINBOND_MANUFACTURER_ID = 0xEF;

/** LED blink period in milliseconds */
static constexpr uint32_t LED_BLINK_PERIOD_MS = 500;

static unsigned testCount_ = 0;
static unsigned passCount_ = 0;
static unsigned failCount_ = 0;

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
        SPI::ST::M32F4::Polled spiDriver( &hspi3 );
        Dio::ST::M32F4::Output csPin( CS_SPI_Flash_GPIO_Port, CS_SPI_Flash_Pin, false );
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
            bool    ok       = flashDriver.readJedecId( mfgId, memType, capacity );
            check( ok, "readJedecId succeeds" );
            check( mfgId == WINBOND_MANUFACTURER_ID, "Manufacturer ID is Winbond (0xEF)" );
            KIT_SYSTEM_TRACE_MSG( SECT_,
                                  "  JEDEC: mfg=0x%02X type=0x%02X cap=0x%02X",
                                  mfgId,
                                  memType,
                                  capacity );
        }

        // --- Test 2: Format and Initialize NV ---
        KIT_SYSTEM_TRACE_MSG( SECT_, "--- Test 2: Format and Init ---" );
        Kit::Checksum::Crc32EthernetFast  crcAlgo;
        static uint8_t                    nvWorkBuffer[NV_PAGE_SIZE];
        NV::Flash::Api<MAX_LOGICAL_PAGES> nv( flashDriver, crcAlgo, NV_CONFIG, nvWorkBuffer, sizeof( nvWorkBuffer ) );
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
                                  writeData[0],
                                  writeData[1],
                                  writeData[2],
                                  writeData[3],
                                  writeData[4] );
            KIT_SYSTEM_TRACE_MSG( SECT_,
                                  "  Read:    [%02X %02X %02X %02X %02X]",
                                  readData[0],
                                  readData[1],
                                  readData[2],
                                  readData[3],
                                  readData[4] );
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
                                  (unsigned long)eraseCount,
                                  (unsigned)freePages,
                                  (unsigned)validPages );

            // Validate sizing metrics from design documentation
            size_t totalSlots = freePages + validPages;
            // After format (32 erases) + test writes, total should be close to 448
            // Some slots may be INVALID (used by old records not yet reclaimed)
            KIT_SYSTEM_TRACE_MSG( SECT_,
                                  "  Sizing: total_slots=%u (expected ~%u), page_map_RAM=%u bytes",
                                  (unsigned)totalSlots,
                                  (unsigned)EXPECTED_TOTAL_RECORD_SLOTS,
                                  (unsigned)EXPECTED_PAGE_MAP_RAM_BYTES );
        }

        // --- Test 8: Wear Leveling ---
        //
        // Verifies that repeated writes to the same logical page do NOT
        // erase flash on every write.  The log-structured design appends
        // new records to free slots and only erases when an entire sector
        // of INVALID records can be reclaimed.
        //
        // With 448 total record slots and 16 logical pages, we expect:
        //   - First ~432 writes (448 - 16 valid) consume free slots
        //   - Erases should only occur after free slots are exhausted
        //   - Over-provisioning ratio of 28:1 means ~1 erase per 10-14 writes
        //     once reclamation starts
        KIT_SYSTEM_TRACE_MSG( SECT_, "--- Test 8: Wear Leveling ---" );
        {
            // Re-format to start with a clean state
            check( nv.format() == true, "Format before wear-leveling test" );

            // Snapshot erase count after format (cumulative across driver lifetime)
            uint32_t erasesAfterFormat;
            size_t   dummy1, dummy2;
            nv.getStatistics( erasesAfterFormat, dummy1, dummy2 );

            // Write to page 0 repeatedly — each write creates a new record
            // at a different physical location
            static constexpr size_t NUM_WEAR_WRITES = 50;
            bool                    allWritesOk     = true;
            for ( size_t i = 0; i < NUM_WEAR_WRITES; i++ )
            {
                uint8_t data[] = { static_cast<uint8_t>( i ),
                                   static_cast<uint8_t>( i + 1 ) };
                if ( !nv.write( 0, data, sizeof( data ) ) )
                {
                    allWritesOk = false;
                    break;
                }
            }
            check( allWritesOk, "50 repeated writes to same page succeed" );

            // Verify the last write is readable
            uint8_t lastRead[2] = { 0 };
            check( nv.read( 0, lastRead, sizeof( lastRead ), sizeof( lastRead ) ) == true, "Read after repeated writes succeeds" );
            check( lastRead[0] == ( NUM_WEAR_WRITES - 1 ) && lastRead[1] == NUM_WEAR_WRITES, "Last written data is correct" );

            // Check statistics to verify wear leveling behavior
            uint32_t eraseCount;
            size_t   freePages;
            size_t   validPages;
            check( nv.getStatistics( eraseCount, freePages, validPages ) == true, "Stats after wear-leveling writes" );
            KIT_SYSTEM_TRACE_MSG( SECT_,
                                  "  After %u writes: erases=%lu free=%u valid=%u",
                                  (unsigned)NUM_WEAR_WRITES,
                                  (unsigned long)eraseCount,
                                  (unsigned)freePages,
                                  (unsigned)validPages );

            // With 32 sectors (448 slots) and 50 writes to one page, we
            // should NOT have needed to erase any sectors beyond the
            // format — 50 writes << 448 free slots.
            uint32_t extraErases = eraseCount - erasesAfterFormat;
            check( extraErases == 0, "No extra erases needed (wear leveling effective)" );
            check( validPages == 1, "Only 1 valid page (latest version)" );
            check( freePages > 0, "Free slots remain" );

            KIT_SYSTEM_TRACE_MSG( SECT_,
                                  "  Wear-leveling: %u writes consumed %u slots, %u free remain",
                                  (unsigned)NUM_WEAR_WRITES,
                                  (unsigned)( EXPECTED_TOTAL_RECORD_SLOTS - freePages - validPages ),
                                  (unsigned)freePages );
        }

        // --- Test 9: Sector Reclamation ---
        //
        // Exhausts the free record slots to force sector reclamation.
        // After format, there are 448 free slots.  Writing to the same
        // logical page uses 1 free slot per write (old record marked
        // INVALID, new record written to free slot).  After ~447 writes,
        // free slots are exhausted and the driver must erase a sector
        // containing only INVALID records to reclaim space.
        KIT_SYSTEM_TRACE_MSG( SECT_, "--- Test 9: Sector Reclamation ---" );
        {
            // Re-format for a clean starting state
            check( nv.format() == true, "Format before reclamation test" );

            // Determine how many writes we need to trigger at least one
            // sector reclamation.  With 448 slots and 1 logical page,
            // the first 447 writes consume free slots, the 448th should
            // trigger reclamation of sector(s) with INVALID records.
            //
            // We write more than 448 to guarantee reclamation occurs.
            static constexpr size_t WRITES_TO_TRIGGER_RECLAMATION = 460;

            bool allWritesOk = true;
            for ( size_t i = 0; i < WRITES_TO_TRIGGER_RECLAMATION; i++ )
            {
                uint8_t data[] = { static_cast<uint8_t>( i & 0xFF ),
                                   static_cast<uint8_t>( ( i >> 8 ) & 0xFF ) };
                if ( !nv.write( 0, data, sizeof( data ) ) )
                {
                    KIT_SYSTEM_TRACE_MSG( SECT_, "  Write failed at iteration %u", (unsigned)i );
                    allWritesOk = false;
                    break;
                }
            }
            check( allWritesOk, "460 writes (forcing reclamation) all succeed" );

            // Verify the last write is correct
            uint8_t lastRead[2] = { 0 };
            check( nv.read( 0, lastRead, sizeof( lastRead ), sizeof( lastRead ) ) == true, "Read after reclamation succeeds" );
            uint8_t expectedByte0 = static_cast<uint8_t>( ( WRITES_TO_TRIGGER_RECLAMATION - 1 ) & 0xFF );
            uint8_t expectedByte1 = static_cast<uint8_t>( ( ( WRITES_TO_TRIGGER_RECLAMATION - 1 ) >> 8 ) & 0xFF );
            check( lastRead[0] == expectedByte0 && lastRead[1] == expectedByte1, "Data correct after reclamation" );

            // Statistics should show reclamation occurred
            uint32_t eraseCount;
            size_t   freePages;
            size_t   validPages;
            check( nv.getStatistics( eraseCount, freePages, validPages ) == true, "Stats after reclamation" );

            // eraseCount includes 32 from format + reclamation erases
            uint32_t reclamationErases = eraseCount - EXPECTED_SECTORS;
            KIT_SYSTEM_TRACE_MSG( SECT_,
                                  "  Reclamation: erases=%lu (format=%u + reclaim=%lu) free=%u valid=%u",
                                  (unsigned long)eraseCount,
                                  (unsigned)EXPECTED_SECTORS,
                                  (unsigned long)reclamationErases,
                                  (unsigned)freePages,
                                  (unsigned)validPages );

            check( reclamationErases > 0, "Sector reclamation occurred" );
            check( validPages == 1, "Only 1 valid page after reclamation" );
            check( freePages > 0, "Free slots available after reclamation" );

            // Verify data survives a restart after reclamation
            nv.stop();
            check( nv.start() == true, "Restart after reclamation succeeds" );
            uint8_t persistRead[2] = { 0 };
            check( nv.read( 0, persistRead, sizeof( persistRead ), sizeof( persistRead ) ) == true, "Read after restart-post-reclamation" );
            check( persistRead[0] == expectedByte0 && persistRead[1] == expectedByte1, "Data persists after reclamation + restart" );
        }

        // --- Test 10: Startup Scan Time ---
        //
        // Measures the time taken by nv.start() to scan flash and rebuild
        // the in-memory page map.  For the 4KB configuration (448 record
        // slots), the design documentation estimates ~16-25 ms.
        KIT_SYSTEM_TRACE_MSG( SECT_, "--- Test 10: Startup Scan Time ---" );
        {
            // Write to several pages so the scan finds valid records
            uint8_t data[] = { 0x42 };
            for ( size_t page = 0; page < MAX_LOGICAL_PAGES; page++ )
            {
                nv.write( page * NV_PAGE_SIZE, data, 1 );
            }

            // Stop and measure restart time
            nv.stop();

            uint32_t startTime = ElapsedTime::milliseconds();
            check( nv.start() == true, "Restart for scan time measurement" );
            uint32_t scanTime = ElapsedTime::deltaMilliseconds( startTime );

            KIT_SYSTEM_TRACE_MSG( SECT_,
                                  "  Startup scan time: %lu ms (max allowed: %lu ms)",
                                  (unsigned long)scanTime,
                                  (unsigned long)MAX_STARTUP_SCAN_TIME_MS );
            check( scanTime <= MAX_STARTUP_SCAN_TIME_MS, "Scan time within acceptable range" );

            // Verify data integrity after timed restart
            uint8_t readData;
            check( nv.read( 0, &readData, 1, 1 ) == true, "Read after timed restart" );
            check( readData == 0x42, "Data correct after timed restart" );
        }

        // --- Summary ---
        KIT_SYSTEM_TRACE_MSG( SECT_, "========================================" );
        KIT_SYSTEM_TRACE_MSG( SECT_, "=== Results: %u/%u passed, %u failed ===", passCount_, testCount_, failCount_ );

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
