/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file

    Hardware test for W25Q flash driver on STM32F4 platform.

    This test verifies basic flash operations:
    - JEDEC ID read
    - Sector erase
    - Page write
    - Data read and verification
    - NV interface operations

    Hardware Setup:
    - W25Q128JV flash connected to SPI3
    - CS pin on configured GPIO (see BSP)
*/

#include "Kit/Bsp/Api.h"
#include "Kit/System/Api.h"
#include "Kit/System/Trace.h"
#include "Kit/System/Thread.h"
#include "Kit/System/FatalError.h"
#include "Kit/System/Shutdown.h"
#include "Kit/Driver/SPI/ST/M32F4/Api.h"
#include "Kit/Driver/Dio/ST/M32F4/Output.h"
#include "Kit/Driver/Flash/W25Q/Api.h"
#include "Kit/Driver/NV/Flash/Api.h"
#include <string.h>

#define SECT_ "_0test"

///
using namespace Kit::System;
using namespace Kit::Driver;

// Test configuration
static constexpr uint32_t TEST_ADDRESS       = 0x000000;  // Start of flash
static constexpr size_t   TEST_DATA_SIZE     = 256;       // One page
static constexpr size_t   NV_FLASH_START     = 0x000000;
static constexpr size_t   NV_FLASH_END       = 0x010000;  // 64KB for NV
static constexpr size_t   NV_PAGE_SIZE       = 64;
static constexpr size_t   NV_TOTAL_SIZE      = 512;

// Test buffers
static uint8_t writeBuffer_[TEST_DATA_SIZE];
static uint8_t readBuffer_[TEST_DATA_SIZE];

// Driver instances (constructed in test)
static SPI::ST::M32F4::Api*     spiDriver_    = nullptr;
static Dio::ST::M32F4::Output*  csPin_        = nullptr;
static Flash::W25Q::Api*        flashDriver_  = nullptr;
static NV::Flash::Api*          nvDriver_     = nullptr;

// NV Configuration
static NV::Flash::Config_T nvConfig_ = {
    NV_FLASH_START,
    NV_FLASH_END,
    NV_PAGE_SIZE,
    NV_TOTAL_SIZE
};

///////////////////////////////////////////////////////////////////////////////
// Test helper functions

static void printHex( const uint8_t* data, size_t len )
{
    for ( size_t i = 0; i < len; i++ )
    {
        printf( "%02X ", data[i] );
        if ( ( i + 1 ) % 16 == 0 )
        {
            printf( "\n" );
        }
    }
    if ( len % 16 != 0 )
    {
        printf( "\n" );
    }
}

static void fillTestPattern( uint8_t* buffer, size_t len, uint8_t seed )
{
    for ( size_t i = 0; i < len; i++ )
    {
        buffer[i] = (uint8_t)( seed + i );
    }
}

static bool verifyTestPattern( const uint8_t* buffer, size_t len, uint8_t seed )
{
    for ( size_t i = 0; i < len; i++ )
    {
        if ( buffer[i] != (uint8_t)( seed + i ) )
        {
            printf( "Verify failed at index %zu: expected 0x%02X, got 0x%02X\n",
                    i, (uint8_t)( seed + i ), buffer[i] );
            return false;
        }
    }
    return true;
}

///////////////////////////////////////////////////////////////////////////////
// Test cases

static bool test_JedecId()
{
    printf( "\n=== Test: JEDEC ID Read ===\n" );

    uint8_t mfgId, memType, capacity;

    if ( !flashDriver_->readJedecId( mfgId, memType, capacity ) )
    {
        printf( "FAILED: Could not read JEDEC ID\n" );
        return false;
    }

    printf( "Manufacturer ID: 0x%02X (expected 0xEF for Winbond)\n", mfgId );
    printf( "Memory Type:     0x%02X\n", memType );
    printf( "Capacity:        0x%02X\n", capacity );

    // Verify Winbond manufacturer ID
    if ( mfgId != 0xEF )
    {
        printf( "WARNING: Unexpected manufacturer ID (not Winbond)\n" );
    }

    printf( "PASSED\n" );
    return true;
}

static bool test_SectorErase()
{
    printf( "\n=== Test: Sector Erase ===\n" );

    // Erase the test sector
    printf( "Erasing sector at 0x%06X...\n", (unsigned)TEST_ADDRESS );
    if ( !flashDriver_->eraseSector( TEST_ADDRESS ) )
    {
        printf( "FAILED: Sector erase failed\n" );
        return false;
    }

    // Read back and verify all 0xFF
    if ( !flashDriver_->read( TEST_ADDRESS, readBuffer_, TEST_DATA_SIZE ) )
    {
        printf( "FAILED: Read after erase failed\n" );
        return false;
    }

    for ( size_t i = 0; i < TEST_DATA_SIZE; i++ )
    {
        if ( readBuffer_[i] != 0xFF )
        {
            printf( "FAILED: Byte at offset %zu is 0x%02X (expected 0xFF)\n", i, readBuffer_[i] );
            return false;
        }
    }

    printf( "PASSED: Sector erased correctly\n" );
    return true;
}

static bool test_PageWrite()
{
    printf( "\n=== Test: Page Write ===\n" );

    // Fill write buffer with test pattern
    fillTestPattern( writeBuffer_, TEST_DATA_SIZE, 0x00 );

    printf( "Writing %zu bytes at 0x%06X...\n", TEST_DATA_SIZE, (unsigned)TEST_ADDRESS );

    // First erase the sector
    if ( !flashDriver_->eraseSector( TEST_ADDRESS ) )
    {
        printf( "FAILED: Pre-write erase failed\n" );
        return false;
    }

    // Write the data
    if ( !flashDriver_->write( TEST_ADDRESS, writeBuffer_, TEST_DATA_SIZE ) )
    {
        printf( "FAILED: Write failed\n" );
        return false;
    }

    // Read back
    memset( readBuffer_, 0, TEST_DATA_SIZE );
    if ( !flashDriver_->read( TEST_ADDRESS, readBuffer_, TEST_DATA_SIZE ) )
    {
        printf( "FAILED: Read back failed\n" );
        return false;
    }

    // Verify
    if ( !verifyTestPattern( readBuffer_, TEST_DATA_SIZE, 0x00 ) )
    {
        printf( "FAILED: Data verification failed\n" );
        printf( "Expected:\n" );
        printHex( writeBuffer_, 32 );
        printf( "Got:\n" );
        printHex( readBuffer_, 32 );
        return false;
    }

    printf( "PASSED: Page write verified\n" );
    return true;
}

static bool test_CrossPageWrite()
{
    printf( "\n=== Test: Cross-Page Write ===\n" );

    // Test writing across page boundary (page = 256 bytes)
    uint32_t crossAddr = TEST_ADDRESS + 200;  // Start 56 bytes before page boundary
    size_t crossLen    = 128;                 // Write across boundary

    // Fill buffer
    fillTestPattern( writeBuffer_, crossLen, 0xA0 );

    printf( "Erasing sector...\n" );
    if ( !flashDriver_->eraseSector( TEST_ADDRESS ) )
    {
        printf( "FAILED: Erase failed\n" );
        return false;
    }

    printf( "Writing %zu bytes at 0x%06X (crosses page boundary)...\n", crossLen, (unsigned)crossAddr );
    if ( !flashDriver_->write( crossAddr, writeBuffer_, crossLen ) )
    {
        printf( "FAILED: Cross-page write failed\n" );
        return false;
    }

    // Read back
    memset( readBuffer_, 0, crossLen );
    if ( !flashDriver_->read( crossAddr, readBuffer_, crossLen ) )
    {
        printf( "FAILED: Read back failed\n" );
        return false;
    }

    // Verify
    if ( !verifyTestPattern( readBuffer_, crossLen, 0xA0 ) )
    {
        printf( "FAILED: Cross-page data verification failed\n" );
        return false;
    }

    printf( "PASSED: Cross-page write verified\n" );
    return true;
}

static bool test_NV_WriteRead()
{
    printf( "\n=== Test: NV Write/Read ===\n" );

    // Format NV storage first
    printf( "Formatting NV storage...\n" );
    if ( !nvDriver_->format() )
    {
        printf( "FAILED: NV format failed\n" );
        return false;
    }

    // Write test data
    uint8_t nvData[16] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                          0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10 };

    printf( "Writing %zu bytes to NV offset 0...\n", sizeof( nvData ) );
    if ( !nvDriver_->write( 0, nvData, sizeof( nvData ) ) )
    {
        printf( "FAILED: NV write failed\n" );
        return false;
    }

    // Read back
    uint8_t nvReadBack[16];
    memset( nvReadBack, 0, sizeof( nvReadBack ) );

    printf( "Reading back...\n" );
    if ( !nvDriver_->read( 0, nvReadBack, sizeof( nvReadBack ) ) )
    {
        printf( "FAILED: NV read failed\n" );
        return false;
    }

    // Verify
    if ( memcmp( nvData, nvReadBack, sizeof( nvData ) ) != 0 )
    {
        printf( "FAILED: NV data mismatch\n" );
        printf( "Expected:\n" );
        printHex( nvData, sizeof( nvData ) );
        printf( "Got:\n" );
        printHex( nvReadBack, sizeof( nvReadBack ) );
        return false;
    }

    printf( "PASSED: NV write/read verified\n" );
    return true;
}

static bool test_NV_Persistence()
{
    printf( "\n=== Test: NV Persistence (Multiple Writes) ===\n" );

    // Write at different offsets
    uint8_t data1[4] = { 0xAA, 0xBB, 0xCC, 0xDD };
    uint8_t data2[4] = { 0x11, 0x22, 0x33, 0x44 };

    printf( "Writing data1 at offset 0...\n" );
    if ( !nvDriver_->write( 0, data1, sizeof( data1 ) ) )
    {
        printf( "FAILED: First write failed\n" );
        return false;
    }

    printf( "Writing data2 at offset 100...\n" );
    if ( !nvDriver_->write( 100, data2, sizeof( data2 ) ) )
    {
        printf( "FAILED: Second write failed\n" );
        return false;
    }

    // Read back both
    uint8_t read1[4], read2[4];

    if ( !nvDriver_->read( 0, read1, sizeof( read1 ) ) ||
         !nvDriver_->read( 100, read2, sizeof( read2 ) ) )
    {
        printf( "FAILED: Read back failed\n" );
        return false;
    }

    // Verify both
    if ( memcmp( data1, read1, sizeof( data1 ) ) != 0 )
    {
        printf( "FAILED: data1 mismatch\n" );
        return false;
    }
    if ( memcmp( data2, read2, sizeof( data2 ) ) != 0 )
    {
        printf( "FAILED: data2 mismatch\n" );
        return false;
    }

    printf( "PASSED: Multiple writes verified\n" );
    return true;
}

///////////////////////////////////////////////////////////////////////////////
// Test runner

void runtests()
{
    KIT_SYSTEM_TRACE_MSG( SECT_, "Starting Flash Driver Hardware Tests" );
    printf( "\n========================================\n" );
    printf( "Flash Driver Hardware Test Suite\n" );
    printf( "========================================\n" );

    // Create driver instances
    printf( "\nInitializing drivers...\n" );

    // SPI driver using hspi3 from BSP
    static SPI::ST::M32F4::Api spiInst( &hspi3 );
    spiDriver_ = &spiInst;

    // CS pin - using the CS_SPI_Flash pin defined in BSP main.h
    // PD14 - active low chip select for W25Q flash
    // activeHigh=true because the flash driver uses:
    //   - setLow() to assert CS (select chip) -> we need physical LOW
    //   - setHigh() to deassert CS (deselect chip) -> we need physical HIGH
    static Dio::ST::M32F4::Output csInst( CS_SPI_Flash_GPIO_Port, CS_SPI_Flash_Pin, true );
    csPin_ = &csInst;
    
    // Ensure CS starts high (deselected)
    csPin_->setHigh();

    // Flash driver
    static Flash::W25Q::Api flashInst( *spiDriver_, *csPin_, Flash::W25Q::Devices::W25Q128 );
    flashDriver_ = &flashInst;

    // NV driver
    static NV::Flash::Api nvInst( *flashDriver_, nvConfig_ );
    nvDriver_ = &nvInst;

    // Start drivers
    printf( "Starting SPI driver...\n" );
    if ( !spiDriver_->start() )
    {
        FatalError::logf( Shutdown::eFAILURE, "Failed to start SPI driver" );
    }

    printf( "Starting Flash driver...\n" );
    if ( !flashDriver_->start() )
    {
        FatalError::logf( Shutdown::eFAILURE, "Failed to start Flash driver" );
    }

    printf( "Starting NV driver...\n" );
    if ( !nvDriver_->start() )
    {
        FatalError::logf( Shutdown::eFAILURE, "Failed to start NV driver" );
    }

    // Print flash info
    printf( "\nFlash Device Info:\n" );
    printf( "  Total Size:   %zu bytes (%zu KB)\n", flashDriver_->getTotalSize(), flashDriver_->getTotalSize() / 1024 );
    printf( "  Sector Size:  %zu bytes\n", flashDriver_->getSectorSize() );
    printf( "  Page Size:    %zu bytes\n", flashDriver_->getPageSize() );
    printf( "  Num Sectors:  %zu\n", flashDriver_->getNumSectors() );

    printf( "\nNV Storage Info:\n" );
    printf( "  Total Size:   %zu bytes\n", nvDriver_->getTotalSize() );
    printf( "  Page Size:    %zu bytes\n", nvDriver_->getPageSize() );
    printf( "  Num Pages:    %zu\n", nvDriver_->getNumPages() );

    // Run tests
    int passed = 0;
    int failed = 0;

    if ( test_JedecId() ) passed++; else failed++;
    if ( test_SectorErase() ) passed++; else failed++;
    if ( test_PageWrite() ) passed++; else failed++;
    if ( test_CrossPageWrite() ) passed++; else failed++;
    if ( test_NV_WriteRead() ) passed++; else failed++;
    if ( test_NV_Persistence() ) passed++; else failed++;

    // Summary
    printf( "\n========================================\n" );
    printf( "Test Results: %d passed, %d failed\n", passed, failed );
    printf( "========================================\n" );

    if ( failed > 0 )
    {
        printf( "\n*** SOME TESTS FAILED ***\n" );
        Bsp_turn_on_debug2();  // Red LED for failure
    }
    else
    {
        printf( "\n*** ALL TESTS PASSED ***\n" );
        Bsp_turn_on_debug1();  // Green LED for success
    }

    // Blink LED to indicate completion
    KIT_SYSTEM_TRACE_MSG( SECT_, "Tests complete - entering idle loop" );
    for ( ;; )
    {
        if ( failed == 0 )
        {
            Bsp_toggle_debug1();
        }
        else
        {
            Bsp_toggle_debug2();
        }
        Kit::System::sleep( 500 );
    }
}
