#ifndef KIT_DRIVER_FLASH_W25Q_STDSPI24_H_
#define KIT_DRIVER_FLASH_W25Q_STDSPI24_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */


#include "Kit/Driver/Flash/IApi.h"
#include "Kit/Driver/Flash/W25Q/Commands.h"
#include "Kit/Driver/SPI/IHalfDuplex.h"
#include "Kit/Driver/Dio/IOutput.h"
#include <stdint.h>

///
namespace Kit {
///
namespace Driver {
///
namespace Flash {
///
namespace W25Q {


/** Size of the SPI command + 24-bit address buffer in bytes.  This driver
    only supports the standard 3-byte (24-bit) addressing mode.
 */
static constexpr size_t CMD_24BIT_ADDR_SIZE = 4;

/** Chip erase timeout in milliseconds (up to 200 seconds) */
static constexpr uint32_t CHIP_ERASE_TIMEOUT_MS = 200000;


/** This class implements the Flash::IApi interface for the Winbond W25Q
    series SPI NOR flash devices using standard (single-lane) SPI and 3-byte
    (24-bit) addressing.  It communicates with the flash device via an SPI
    driver and a chip select (CS) digital output pin.

    The implementation handles page-boundary crossing for write operations,
    polls the BUSY status after each write/erase, and supports W25Q16
    through W25Q128 devices.

    NOTE: This driver is intentionally scoped to Standard SPI operation and
          3-byte addressing.  QSPI and/or 4-byte addressing variants can be
          added as separate drivers that co-exist in this directory.
 */
class StdSpi24 : public IApi
{
public:
    /** Constructor.  The 'spi' and 'cs' references must remain valid for
        the lifetime of this object.  The 'info' describes the specific
        W25Q device variant being used.
     */
    StdSpi24( SPI::IHalfDuplex&   spi,
              Dio::IOutput&       cs,
              const DeviceInfo_T& info ) noexcept;


public:
    /// See Kit::Driver::IStart
    bool start( void* startArgs = nullptr ) noexcept override;

    /// See Kit::Driver::IStop
    void stop() noexcept override;


public:
    /// See Kit::Driver::Flash::IApi
    Result_T read( size_t srcOffset,
                   void*  dstBuffer,
                   size_t numBytes ) noexcept override;

    /// See Kit::Driver::Flash::IApi
    Result_T write( size_t      dstOffset,
                    const void* srcBuffer,
                    size_t      numBytes ) noexcept override;

    /// See Kit::Driver::Flash::IApi
    Result_T eraseSector( size_t   sectorAddress,
                          unsigned numberOfSectors = 1 ) noexcept override;

    /// See Kit::Driver::Flash::IApi
    Result_T eraseChip() noexcept override;

    /// See Kit::Driver::Flash::IApi
    bool waitUntilReady( uint32_t timeoutMs = 1000 ) noexcept override;


public:
    /// See Kit::Driver::Flash::IApi
    size_t getTotalSize() const noexcept override;

    /// See Kit::Driver::Flash::IApi
    size_t getSectorSize() const noexcept override;

    /// See Kit::Driver::Flash::IApi
    size_t getWritePageSize() const noexcept override;

    /// See Kit::Driver::Flash::IApi
    size_t getNumSectors() const noexcept override;


public:
    /** Reads the JEDEC device ID from the flash.  Returns the manufacturer
        ID, memory type, and capacity bytes.

        The method returns true if the operation was successful; else false
        is returned.
     */
    bool readJedecId( uint8_t& mfgId,
                      uint8_t& memType,
                      uint8_t& capacity ) noexcept;


protected:
    /// Sends the WRITE_ENABLE command
    bool writeEnable() noexcept;

    /// Sends a single-byte SPI command
    inline bool sendCommand( uint8_t cmd ) noexcept
    {
        return m_spi.write( &cmd, 1 );
    }

    /// Sends a command followed by a 24-bit address
    bool sendCommandWithAddress( uint8_t cmd, size_t address ) noexcept;


protected:
    SPI::IHalfDuplex& m_spi;         //!< Reference to SPI driver
    Dio::IOutput&     m_cs;          //!< Reference to chip select output
    DeviceInfo_T      m_deviceInfo;  //!< Device configuration
    bool              m_started;     //!< Driver started flag
};


}  // end namespaces
}
}
}
#endif  // end header latch
