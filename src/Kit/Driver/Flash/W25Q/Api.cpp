/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */


#include "Kit/Driver/Flash/W25Q/Api.h"
#include <string.h>

///
using namespace Kit::Driver::Flash::W25Q;


//////////////////////////////////////////////////////////////////////////////
Api::Api( SPI::IApi&          spi,
          Dio::IOutput&       cs,
          const DeviceInfo_T& info ) noexcept
    : m_spi( spi )
    , m_cs( cs )
    , m_deviceInfo( info )
    , m_cmdBuffer{ 0 }
    , m_started( false )
{
}


//////////////////////////////////////////////////////////////////////////////
bool Api::start( void* startArgs ) noexcept
{
    if ( m_started )
    {
        return true;
    }

    // Deassert chip select (CS = HIGH = inactive)
    m_cs.setHigh();

    // Release from power-down mode first.  If a previous firmware put the
    // flash into power-down mode (via 0xB9) and the MCU was reset without
    // cycling power, the device ignores ALL commands including Enable Reset
    // and Reset Device.  The Release Power-Down (0xAB) command is the only
    // command recognised in power-down mode.
    m_cs.setLow();
    sendCommand( Commands_T::RELEASE_POWER_DOWN );
    m_cs.setHigh();

    // Wait for release from power-down to complete (tRES1 = 3us per W25Q
    // datasheet).  Use a generous delay to cover all variants.
    for ( volatile uint32_t i = 0; i < 1000; i++ ) {}

    // Send software reset sequence to recover from any undefined state
    // that may result from spurious clock edges during MCU power-on.
    // The Enable Reset (66h) + Reset Device (99h) sequence resets the
    // internal state machine to its power-on default.
    m_cs.setLow();
    sendCommand( Commands_T::ENABLE_RESET );
    m_cs.setHigh();

    // Short delay between the two reset commands (>50ns per datasheet)
    for ( volatile uint32_t i = 0; i < 10; i++ ) {}

    m_cs.setLow();
    sendCommand( Commands_T::RESET_DEVICE );
    m_cs.setHigh();

    // Wait for reset to complete (tRST = 30us max per W25Q datasheet)
    for ( volatile uint32_t i = 0; i < 5000; i++ ) {}

    m_started = true;
    return true;
}

void Api::stop() noexcept
{
    if ( m_started )
    {
        m_cs.setHigh();
        m_started = false;
    }
}


//////////////////////////////////////////////////////////////////////////////
bool Api::read( size_t srcOffset,
                void*  dstBuffer,
                size_t numBytes ) noexcept
{
    if ( !m_started || dstBuffer == nullptr || numBytes == 0 )
    {
        return false;
    }

    if ( srcOffset + numBytes > m_deviceInfo.totalSize )
    {
        return false;
    }

    m_cs.setLow();

    // Send READ_DATA command + 24-bit address
    if ( !sendCommandWithAddress( Commands_T::READ_DATA, srcOffset ) )
    {
        m_cs.setHigh();
        return false;
    }

    // Read data
    bool result = m_spi.read( dstBuffer, numBytes );

    m_cs.setHigh();
    return result;
}


//////////////////////////////////////////////////////////////////////////////
bool Api::write( size_t      dstOffset,
                 const void* srcBuffer,
                 size_t      numBytes ) noexcept
{
    if ( !m_started || srcBuffer == nullptr || numBytes == 0 )
    {
        return false;
    }

    if ( dstOffset + numBytes > m_deviceInfo.totalSize )
    {
        return false;
    }

    const uint8_t* src       = static_cast<const uint8_t*>( srcBuffer );
    size_t         remaining = numBytes;
    size_t         address   = dstOffset;

    while ( remaining > 0 )
    {
        // Calculate bytes remaining in current flash page
        size_t offsetInPage   = address % m_deviceInfo.pageSize;
        size_t bytesThisPage  = m_deviceInfo.pageSize - offsetInPage;
        if ( bytesThisPage > remaining )
        {
            bytesThisPage = remaining;
        }

        // Enable write latch
        if ( !writeEnable() )
        {
            return false;
        }

        // Send PAGE_PROGRAM command
        m_cs.setLow();
        if ( !sendCommandWithAddress( Commands_T::PAGE_PROGRAM, address ) )
        {
            m_cs.setHigh();
            return false;
        }

        // Write data for this page chunk
        if ( !m_spi.write( src, bytesThisPage ) )
        {
            m_cs.setHigh();
            return false;
        }
        m_cs.setHigh();

        // Wait for programming to complete
        if ( !waitUntilReady() )
        {
            return false;
        }

        // Advance to next page
        src       += bytesThisPage;
        address   += bytesThisPage;
        remaining -= bytesThisPage;
    }

    return true;
}


//////////////////////////////////////////////////////////////////////////////
bool Api::eraseSector( size_t sectorAddress ) noexcept
{
    if ( !m_started )
    {
        return false;
    }

    if ( !writeEnable() )
    {
        return false;
    }

    m_cs.setLow();
    bool result = sendCommandWithAddress( Commands_T::SECTOR_ERASE, sectorAddress );
    m_cs.setHigh();

    if ( result )
    {
        result = waitUntilReady();
    }

    return result;
}


bool Api::eraseBlock32K( size_t blockAddress ) noexcept
{
    if ( !m_started )
    {
        return false;
    }

    if ( !writeEnable() )
    {
        return false;
    }

    m_cs.setLow();
    bool result = sendCommandWithAddress( Commands_T::BLOCK_ERASE_32K, blockAddress );
    m_cs.setHigh();

    if ( result )
    {
        result = waitUntilReady();
    }

    return result;
}


bool Api::eraseBlock64K( size_t blockAddress ) noexcept
{
    if ( !m_started )
    {
        return false;
    }

    if ( !writeEnable() )
    {
        return false;
    }

    m_cs.setLow();
    bool result = sendCommandWithAddress( Commands_T::BLOCK_ERASE_64K, blockAddress );
    m_cs.setHigh();

    if ( result )
    {
        result = waitUntilReady();
    }

    return result;
}


bool Api::eraseChip() noexcept
{
    if ( !m_started )
    {
        return false;
    }

    if ( !writeEnable() )
    {
        return false;
    }

    m_cs.setLow();
    bool result = sendCommand( Commands_T::CHIP_ERASE );
    m_cs.setHigh();

    if ( result )
    {
        result = waitUntilReady( CHIP_ERASE_TIMEOUT_MS );
    }

    return result;
}


//////////////////////////////////////////////////////////////////////////////
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


//////////////////////////////////////////////////////////////////////////////
bool Api::readJedecId( uint8_t& mfgId,
                       uint8_t& memType,
                       uint8_t& capacity ) noexcept
{
    if ( !m_started )
    {
        return false;
    }

    m_cs.setLow();

    uint8_t cmd = Commands_T::JEDEC_ID;
    if ( !m_spi.write( &cmd, 1 ) )
    {
        m_cs.setHigh();
        return false;
    }

    uint8_t idBuffer[JEDEC_ID_SIZE];
    if ( !m_spi.read( idBuffer, JEDEC_ID_SIZE ) )
    {
        m_cs.setHigh();
        return false;
    }

    m_cs.setHigh();

    mfgId    = idBuffer[0];
    memType  = idBuffer[1];
    capacity = idBuffer[2];
    return true;
}


//////////////////////////////////////////////////////////////////////////////
bool Api::writeEnable() noexcept
{
    m_cs.setLow();
    bool result = sendCommand( Commands_T::WRITE_ENABLE );
    m_cs.setHigh();
    return result;
}

bool Api::waitUntilReady( uint32_t timeoutMs ) noexcept
{
    // In a real implementation, this would poll the status register with a
    // timeout.  For now, we poll once per iteration.
    for ( uint32_t i = 0; i < timeoutMs; i++ )
    {
        m_cs.setLow();

        uint8_t cmd = Commands_T::READ_STATUS_REG1;
        if ( !m_spi.write( &cmd, 1 ) )
        {
            m_cs.setHigh();
            return false;
        }

        uint8_t status = 0xFF;
        if ( !m_spi.read( &status, 1 ) )
        {
            m_cs.setHigh();
            return false;
        }

        m_cs.setHigh();

        if ( ( status & StatusReg1_T::BUSY ) == 0 )
        {
            return true; // Ready
        }

        // In a real implementation, we would delay here
    }

    return false; // Timed out
}

bool Api::sendCommand( uint8_t cmd ) noexcept
{
    return m_spi.write( &cmd, 1 );
}

bool Api::sendCommandWithAddress( uint8_t cmd, size_t address ) noexcept
{
    m_cmdBuffer[0] = cmd;
    m_cmdBuffer[1] = static_cast<uint8_t>( ( address >> 16 ) & 0xFF );
    m_cmdBuffer[2] = static_cast<uint8_t>( ( address >>  8 ) & 0xFF );
    m_cmdBuffer[3] = static_cast<uint8_t>( address & 0xFF );
    return m_spi.write( m_cmdBuffer, sizeof( m_cmdBuffer ) );
}
