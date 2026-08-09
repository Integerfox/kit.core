/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */


#include "Kit/Driver/Flash/W25Q/StdSpi24.h"
#include "Kit/System/Api.h"
#include <string.h>

///
using namespace Kit::Driver::Flash::W25Q;


//////////////////////////////////////////////////////////////////////////////
StdSpi24::StdSpi24( SPI::IHalfDuplex&   spi,
                    Dio::IOutput&       cs,
                    const DeviceInfo_T& info ) noexcept
    : m_spi( spi )
    , m_cs( cs )
    , m_deviceInfo( info )
    , m_started( false )
{
}


//////////////////////////////////////////////////////////////////////////////
bool StdSpi24::start( void* startArgs ) noexcept
{
    if ( m_started )
    {
        return true;
    }

    // Deassert chip select (CS = HIGH = inactive)
    m_cs.deassertPin();

    // Release from power-down mode first.  If a previous firmware put the
    // flash into power-down mode (via 0xB9) and the MCU was reset without
    // cycling power, the device ignores ALL commands including Enable Reset
    // and Reset Device.  The Release Power-Down (0xAB) command is the only
    // command recognised in power-down mode.
    m_cs.assertPin();
    sendCommand( RELEASE_POWER_DOWN );
    m_cs.deassertPin();

    // Wait for release from power-down to complete (tRES1 = 3us per W25Q
    // datasheet).  This only runs once at start-up, so a 1ms OSAL sleep is
    // used instead of a clock-speed dependent busy-wait.
    Kit::System::sleep( 1 );

    // Send software reset sequence to recover from any undefined state
    // that may result from spurious clock edges during MCU power-on.
    // The Enable Reset (66h) + Reset Device (99h) sequence resets the
    // internal state machine to its power-on default.
    m_cs.assertPin();
    sendCommand( ENABLE_RESET );
    m_cs.deassertPin();

    // Short delay between the two reset commands (>50ns per datasheet)
    Kit::System::sleep( 1 );

    m_cs.assertPin();
    sendCommand( RESET_DEVICE );
    m_cs.deassertPin();

    // Wait for reset to complete (tRST = 30us max per W25Q datasheet)
    Kit::System::sleep( 1 );

    m_started = true;
    return true;
}

void StdSpi24::stop() noexcept
{
    if ( m_started )
    {
        m_cs.deassertPin();
        m_started = false;
    }
}


//////////////////////////////////////////////////////////////////////////////
StdSpi24::Result_T StdSpi24::read( size_t srcOffset,
                                   void*  dstBuffer,
                                   size_t numBytes ) noexcept
{
    if ( !m_started || dstBuffer == nullptr || numBytes == 0 )
    {
        return ERR_FAILED;
    }

    if ( srcOffset + numBytes > m_deviceInfo.totalSize )
    {
        return ERR_RANGE;
    }

    m_cs.assertPin();

    // Send READ_DATA command + 24-bit address
    if ( !sendCommandWithAddress( READ_DATA, srcOffset ) )
    {
        m_cs.deassertPin();
        return ERR_FAILED;
    }

    // Read data
    bool result = m_spi.read( dstBuffer, numBytes );

    m_cs.deassertPin();
    return result ? SUCCESS : ERR_FAILED;
}


//////////////////////////////////////////////////////////////////////////////
StdSpi24::Result_T StdSpi24::write( size_t      dstOffset,
                                    const void* srcBuffer,
                                    size_t      numBytes ) noexcept
{
    if ( !m_started || srcBuffer == nullptr || numBytes == 0 )
    {
        return ERR_FAILED;
    }

    if ( dstOffset + numBytes > m_deviceInfo.totalSize )
    {
        return ERR_RANGE;
    }

    const uint8_t* src       = static_cast<const uint8_t*>( srcBuffer );
    size_t         remaining = numBytes;
    size_t         address   = dstOffset;

    while ( remaining > 0 )
    {
        // Wait for any previous programming operation to complete before
        // starting the next page.  Placing the wait at the top of the loop
        // (instead of after the program command) means the caller is not
        // blocked on the physical programming time of the final/only page.
        if ( !waitUntilReady() )
        {
            return ERR_FAILED;
        }

        // Calculate bytes remaining in current flash page
        size_t offsetInPage  = address % m_deviceInfo.pageSize;
        size_t bytesThisPage = m_deviceInfo.pageSize - offsetInPage;
        if ( bytesThisPage > remaining )
        {
            bytesThisPage = remaining;
        }

        // Enable write latch
        if ( !writeEnable() )
        {
            return ERR_FAILED;
        }

        // Send PAGE_PROGRAM command
        m_cs.assertPin();
        if ( !sendCommandWithAddress( PAGE_PROGRAM, address ) )
        {
            m_cs.deassertPin();
            return ERR_FAILED;
        }

        // Write data for this page chunk
        if ( !m_spi.write( src, bytesThisPage ) )
        {
            m_cs.deassertPin();
            return ERR_FAILED;
        }
        m_cs.deassertPin();

        // Advance to next page
        src       += bytesThisPage;
        address   += bytesThisPage;
        remaining -= bytesThisPage;
    }

    return SUCCESS;
}


//////////////////////////////////////////////////////////////////////////////
StdSpi24::Result_T StdSpi24::eraseSector( size_t   sectorAddress,
                                          unsigned numberOfSectors ) noexcept
{
    if ( !m_started )
    {
        return ERR_FAILED;
    }

    // Erase the requested run of sectors one 4KB sector at a time
    size_t address = sectorAddress;
    for ( unsigned i = 0; i < numberOfSectors; i++ )
    {
        if ( !writeEnable() )
        {
            return ERR_FAILED;
        }

        m_cs.assertPin();
        bool result = sendCommandWithAddress( SECTOR_ERASE, address );
        m_cs.deassertPin();

        if ( result )
        {
            result = waitUntilReady();
        }

        if ( !result )
        {
            return ERR_FAILED;
        }

        address += m_deviceInfo.sectorSize;
    }

    return SUCCESS;
}


StdSpi24::Result_T StdSpi24::eraseChip() noexcept
{
    if ( !m_started )
    {
        return ERR_FAILED;
    }

    if ( !writeEnable() )
    {
        return ERR_FAILED;
    }

    m_cs.assertPin();
    bool result = sendCommand( CHIP_ERASE );
    m_cs.deassertPin();

    if ( result )
    {
        result = waitUntilReady( CHIP_ERASE_TIMEOUT_MS );
    }

    return result ? SUCCESS : ERR_FAILED;
}


//////////////////////////////////////////////////////////////////////////////
size_t StdSpi24::getTotalSize() const noexcept
{
    return m_deviceInfo.totalSize;
}

size_t StdSpi24::getSectorSize() const noexcept
{
    return m_deviceInfo.sectorSize;
}

size_t StdSpi24::getWritePageSize() const noexcept
{
    return m_deviceInfo.pageSize;
}

size_t StdSpi24::getNumSectors() const noexcept
{
    return m_deviceInfo.numSectors;
}


//////////////////////////////////////////////////////////////////////////////
bool StdSpi24::readJedecId( uint8_t& mfgId,
                            uint8_t& memType,
                            uint8_t& capacity ) noexcept
{
    if ( !m_started )
    {
        return false;
    }

    m_cs.assertPin();

    if ( !sendCommand( JEDEC_ID ) )
    {
        m_cs.deassertPin();
        return false;
    }

    uint8_t idBuffer[JEDEC_ID_SIZE];
    if ( !m_spi.read( idBuffer, JEDEC_ID_SIZE ) )
    {
        m_cs.deassertPin();
        return false;
    }

    m_cs.deassertPin();

    mfgId    = idBuffer[0];
    memType  = idBuffer[1];
    capacity = idBuffer[2];
    return true;
}


//////////////////////////////////////////////////////////////////////////////
bool StdSpi24::writeEnable() noexcept
{
    m_cs.assertPin();
    bool result = sendCommand( WRITE_ENABLE );
    m_cs.deassertPin();
    return result;
}

bool StdSpi24::waitUntilReady( uint32_t timeoutMs ) noexcept
{
    for ( uint32_t i = 0; i < timeoutMs; i++ )
    {
        m_cs.assertPin();

        if ( !sendCommand( READ_STATUS_REG1 ) )
        {
            m_cs.deassertPin();
            return false;
        }

        uint8_t status = 0xFF;
        if ( !m_spi.read( &status, 1 ) )
        {
            m_cs.deassertPin();
            return false;
        }

        m_cs.deassertPin();

        if ( ( status & BUSY ) == 0 )
        {
            return true;  // Ready
        }

        // Poll roughly once per millisecond so that 'timeoutMs' iterations
        // correspond to a real-time timeout in milliseconds.
        Kit::System::sleep( 1 );
    }

    return false;  // Timed out
}

bool StdSpi24::sendCommandWithAddress( uint8_t cmd, size_t address ) noexcept
{
    uint8_t cmdBuffer[CMD_24BIT_ADDR_SIZE];
    cmdBuffer[0] = cmd;
    cmdBuffer[1] = static_cast<uint8_t>( ( address >> 16 ) & 0xFF );
    cmdBuffer[2] = static_cast<uint8_t>( ( address >> 8 ) & 0xFF );
    cmdBuffer[3] = static_cast<uint8_t>( address & 0xFF );
    return m_spi.write( cmdBuffer, sizeof( cmdBuffer ) );
}
