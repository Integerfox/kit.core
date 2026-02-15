/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Api.h"
#include <string.h>

///
using namespace Kit::Driver::Flash::W25Q;

/////////////////////////////////////////////////////////////////////////////
Api::Api( Kit::Driver::SPI::IApi&       spi,
          Kit::Driver::Dio::IOutput&    chipSelect,
          const DeviceInfo_T&           deviceInfo ) noexcept
    : m_spi( spi )
    , m_chipSelect( chipSelect )
    , m_deviceInfo( deviceInfo )
    , m_isStarted( false )
{
    // Ensure CS is deasserted (high) at construction
    m_chipSelect.setHigh();
}


/////////////////////////////////////////////////////////////////////////////
bool Api::start( void* startArgs ) noexcept
{
    // Do nothing if already started
    if ( m_isStarted )
    {
        return true;
    }

    // Ensure CS is deasserted
    csDeassert();

    // Verify we can communicate with the device by reading JEDEC ID
    uint8_t mfgId, memType, capacity;
    if ( !readJedecId( mfgId, memType, capacity ) )
    {
        return false;
    }

    // Verify it's a Winbond device (manufacturer ID = 0xEF)
    // Note: This check could be made optional via startArgs
    if ( mfgId != 0xEF )
    {
        return false;
    }

    m_isStarted = true;
    return true;
}


void Api::stop() noexcept
{
    if ( !m_isStarted )
    {
        return;
    }

    // Ensure any pending operation completes
    waitUntilReady();

    // Ensure CS is deasserted
    csDeassert();

    m_isStarted = false;
}


/////////////////////////////////////////////////////////////////////////////
bool Api::read( uint32_t address,
                void*    dstData,
                size_t   numBytes ) noexcept
{
    if ( !m_isStarted || dstData == nullptr || numBytes == 0 )
    {
        return false;
    }

    // Validate address range
    if ( address + numBytes > m_deviceInfo.totalSize )
    {
        return false;
    }

    // Wait for any pending operation
    waitUntilReady();

    // Build command: READ (1 byte) + Address (3 bytes)
    uint8_t cmd[4];
    cmd[0] = Commands_T::READ_DATA;
    cmd[1] = ( address >> 16 ) & 0xFF;
    cmd[2] = ( address >> 8 ) & 0xFF;
    cmd[3] = address & 0xFF;

    csAssert();

    // Send command and address
    bool result = m_spi.write( cmd, sizeof( cmd ) );
    if ( result )
    {
        // Read data
        result = m_spi.read( dstData, numBytes );
    }

    csDeassert();

    return result;
}


bool Api::write( uint32_t    address,
                 const void* srcData,
                 size_t      numBytes ) noexcept
{
    if ( !m_isStarted || srcData == nullptr || numBytes == 0 )
    {
        return false;
    }

    // Validate address range
    if ( address + numBytes > m_deviceInfo.totalSize )
    {
        return false;
    }

    const uint8_t* data = static_cast<const uint8_t*>( srcData );
    size_t remaining    = numBytes;
    uint32_t currentAddr = address;

    while ( remaining > 0 )
    {
        // Calculate bytes to write in this page
        // Must not cross page boundary (256 bytes aligned)
        size_t pageOffset     = currentAddr % m_deviceInfo.pageSize;
        size_t bytesThisPage  = m_deviceInfo.pageSize - pageOffset;

        if ( bytesThisPage > remaining )
        {
            bytesThisPage = remaining;
        }

        // Write the page
        if ( !writePage( currentAddr, data, bytesThisPage ) )
        {
            return false;
        }

        // Update for next iteration
        currentAddr += bytesThisPage;
        data        += bytesThisPage;
        remaining   -= bytesThisPage;
    }

    return true;
}


bool Api::writePage( uint32_t address, const uint8_t* data, size_t numBytes ) noexcept
{
    // Wait for any pending operation
    waitUntilReady();

    // Enable write operations
    writeEnable();

    // Build command: PAGE_PROGRAM (1 byte) + Address (3 bytes)
    uint8_t cmd[4];
    cmd[0] = Commands_T::PAGE_PROGRAM;
    cmd[1] = ( address >> 16 ) & 0xFF;
    cmd[2] = ( address >> 8 ) & 0xFF;
    cmd[3] = address & 0xFF;

    csAssert();

    // Send command and address
    bool result = m_spi.write( cmd, sizeof( cmd ) );
    if ( result )
    {
        // Send data
        result = m_spi.write( data, numBytes );
    }

    csDeassert();

    // Wait for write to complete
    waitUntilReady();

    return result;
}


bool Api::eraseSector( uint32_t address ) noexcept
{
    if ( !m_isStarted )
    {
        return false;
    }

    // Validate address
    if ( address >= m_deviceInfo.totalSize )
    {
        return false;
    }

    // Wait for any pending operation
    waitUntilReady();

    // Enable write operations
    writeEnable();

    // Build command: SECTOR_ERASE (1 byte) + Address (3 bytes)
    uint8_t cmd[4];
    cmd[0] = Commands_T::SECTOR_ERASE_4K;
    cmd[1] = ( address >> 16 ) & 0xFF;
    cmd[2] = ( address >> 8 ) & 0xFF;
    cmd[3] = address & 0xFF;

    csAssert();
    bool result = m_spi.write( cmd, sizeof( cmd ) );
    csDeassert();

    // Wait for erase to complete
    waitUntilReady();

    return result;
}


bool Api::eraseBlock32K( uint32_t address ) noexcept
{
    if ( !m_isStarted )
    {
        return false;
    }

    // Validate address
    if ( address >= m_deviceInfo.totalSize )
    {
        return false;
    }

    // Wait for any pending operation
    waitUntilReady();

    // Enable write operations
    writeEnable();

    // Build command: BLOCK_ERASE_32K (1 byte) + Address (3 bytes)
    uint8_t cmd[4];
    cmd[0] = Commands_T::BLOCK_ERASE_32K;
    cmd[1] = ( address >> 16 ) & 0xFF;
    cmd[2] = ( address >> 8 ) & 0xFF;
    cmd[3] = address & 0xFF;

    csAssert();
    bool result = m_spi.write( cmd, sizeof( cmd ) );
    csDeassert();

    // Wait for erase to complete
    waitUntilReady();

    return result;
}


bool Api::eraseBlock64K( uint32_t address ) noexcept
{
    if ( !m_isStarted )
    {
        return false;
    }

    // Validate address
    if ( address >= m_deviceInfo.totalSize )
    {
        return false;
    }

    // Wait for any pending operation
    waitUntilReady();

    // Enable write operations
    writeEnable();

    // Build command: BLOCK_ERASE_64K (1 byte) + Address (3 bytes)
    uint8_t cmd[4];
    cmd[0] = Commands_T::BLOCK_ERASE_64K;
    cmd[1] = ( address >> 16 ) & 0xFF;
    cmd[2] = ( address >> 8 ) & 0xFF;
    cmd[3] = address & 0xFF;

    csAssert();
    bool result = m_spi.write( cmd, sizeof( cmd ) );
    csDeassert();

    // Wait for erase to complete
    waitUntilReady();

    return result;
}


bool Api::eraseChip() noexcept
{
    if ( !m_isStarted )
    {
        return false;
    }

    // Wait for any pending operation
    waitUntilReady();

    // Enable write operations
    writeEnable();

    // Send chip erase command
    uint8_t cmd = Commands_T::CHIP_ERASE;

    csAssert();
    bool result = m_spi.write( &cmd, 1 );
    csDeassert();

    // Wait for erase to complete (this can take a long time!)
    waitUntilReady();

    return result;
}


/////////////////////////////////////////////////////////////////////////////
size_t Api::getTotalSize() const noexcept
{
    return m_deviceInfo.totalSize;
}


size_t Api::getSectorSize() const noexcept
{
    return m_deviceInfo.sectorSize;
}


size_t Api::getPageSize() const noexcept
{
    return m_deviceInfo.pageSize;
}


size_t Api::getNumSectors() const noexcept
{
    return m_deviceInfo.numSectors;
}


/////////////////////////////////////////////////////////////////////////////
bool Api::readJedecId( uint8_t& manufacturerId,
                       uint8_t& memoryType,
                       uint8_t& capacity ) noexcept
{
    uint8_t cmd    = Commands_T::JEDEC_ID;
    uint8_t id[3]  = { 0 };

    csAssert();

    bool result = m_spi.write( &cmd, 1 );
    if ( result )
    {
        result = m_spi.read( id, sizeof( id ) );
    }

    csDeassert();

    if ( result )
    {
        manufacturerId = id[0];
        memoryType     = id[1];
        capacity       = id[2];
    }

    return result;
}


bool Api::isBusy() noexcept
{
    return ( readStatusReg1() & StatusReg1_T::BUSY ) != 0;
}


/////////////////////////////////////////////////////////////////////////////
void Api::writeEnable() noexcept
{
    uint8_t cmd = Commands_T::WRITE_ENABLE;

    csAssert();
    m_spi.write( &cmd, 1 );
    csDeassert();
}


void Api::writeDisable() noexcept
{
    uint8_t cmd = Commands_T::WRITE_DISABLE;

    csAssert();
    m_spi.write( &cmd, 1 );
    csDeassert();
}


uint8_t Api::readStatusReg1() noexcept
{
    uint8_t cmd    = Commands_T::READ_STATUS_REG1;
    uint8_t status = 0;

    csAssert();
    m_spi.write( &cmd, 1 );
    m_spi.read( &status, 1 );
    csDeassert();

    return status;
}


void Api::waitUntilReady() noexcept
{
    // Poll the BUSY bit in Status Register-1 until it clears
    while ( isBusy() )
    {
        // The busy wait could be improved with a yield or delay
        // for RTOS environments, but keeping it simple for now
    }
}


void Api::csAssert() noexcept
{
    m_chipSelect.setLow();
}


void Api::csDeassert() noexcept
{
    m_chipSelect.setHigh();
}
