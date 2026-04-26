#ifndef KIT_DRIVER_FLASH_W25Q_API_H_
#define KIT_DRIVER_FLASH_W25Q_API_H_
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
#include "Kit/Driver/SPI/IApi.h"
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


/** W25Q SPI command opcodes */
struct Commands_T
{
    static constexpr uint8_t WRITE_ENABLE       = 0x06; //!< Enable write latch
    static constexpr uint8_t WRITE_DISABLE      = 0x04; //!< Disable write latch
    static constexpr uint8_t READ_STATUS_REG1   = 0x05; //!< Read status register 1
    static constexpr uint8_t READ_STATUS_REG2   = 0x35; //!< Read status register 2
    static constexpr uint8_t WRITE_STATUS_REG   = 0x01; //!< Write status register
    static constexpr uint8_t PAGE_PROGRAM       = 0x02; //!< Program up to 256 bytes
    static constexpr uint8_t SECTOR_ERASE       = 0x20; //!< Erase 4KB sector
    static constexpr uint8_t BLOCK_ERASE_32K    = 0x52; //!< Erase 32KB block
    static constexpr uint8_t BLOCK_ERASE_64K    = 0xD8; //!< Erase 64KB block
    static constexpr uint8_t CHIP_ERASE         = 0xC7; //!< Erase entire chip
    static constexpr uint8_t READ_DATA          = 0x03; //!< Read at up to 50MHz
    static constexpr uint8_t FAST_READ          = 0x0B; //!< Read at up to 133MHz
    static constexpr uint8_t JEDEC_ID           = 0x9F; //!< Read JEDEC manufacturer ID
    static constexpr uint8_t POWER_DOWN         = 0xB9; //!< Enter low power mode
    static constexpr uint8_t RELEASE_POWER_DOWN = 0xAB; //!< Exit low power mode
};

/** Status Register 1 bit definitions */
struct StatusReg1_T
{
    static constexpr uint8_t BUSY = 0x01; //!< Bit 0: erase/write in progress
    static constexpr uint8_t WEL  = 0x02; //!< Bit 1: write enable latch
    static constexpr uint8_t BP0  = 0x04; //!< Bit 2: block protect 0
    static constexpr uint8_t BP1  = 0x08; //!< Bit 3: block protect 1
    static constexpr uint8_t BP2  = 0x10; //!< Bit 4: block protect 2
    static constexpr uint8_t TB   = 0x20; //!< Bit 5: top/bottom protect
    static constexpr uint8_t SEC  = 0x40; //!< Bit 6: sector protect
    static constexpr uint8_t SRP0 = 0x80; //!< Bit 7: status register protect
};

/** Device information/configuration */
struct DeviceInfo_T
{
    size_t totalSize;   //!< Total flash size in bytes
    size_t sectorSize;  //!< Sector size (always 4096 for W25Q)
    size_t pageSize;    //!< Page size (always 256 for W25Q)
    size_t numSectors;  //!< Total number of sectors
};


/** Predefined device configurations for common W25Q variants */
static constexpr DeviceInfo_T W25Q128 = { 16 * 1024 * 1024, 4096, 256, 4096 };
static constexpr DeviceInfo_T W25Q64  = {  8 * 1024 * 1024, 4096, 256, 2048 };
static constexpr DeviceInfo_T W25Q32  = {  4 * 1024 * 1024, 4096, 256, 1024 };
static constexpr DeviceInfo_T W25Q16  = {  2 * 1024 * 1024, 4096, 256,  512 };

/** Size of the JEDEC device ID response in bytes */
static constexpr size_t JEDEC_ID_SIZE = 3;

/** Size of the SPI command + 24-bit address buffer in bytes */
static constexpr size_t CMD_ADDR_SIZE = 4;

/** Chip erase timeout in milliseconds (up to 200 seconds) */
static constexpr uint32_t CHIP_ERASE_TIMEOUT_MS = 200000;


/** This class implements the Flash::IApi interface for the Winbond W25Q
    series SPI NOR flash devices.  It communicates with the flash device
    via an SPI driver and a chip select (CS) digital output pin.

    The implementation handles page-boundary crossing for write operations,
    polls the BUSY status after each write/erase, and supports W25Q16
    through W25Q128 devices.
 */
class Api : public IApi
{
public:
    /** Constructor.  The 'spi' and 'cs' references must remain valid for
        the lifetime of this object.  The 'info' describes the specific
        W25Q device variant being used.
     */
    Api( SPI::IApi&          spi,
         Dio::IOutput&       cs,
         const DeviceInfo_T& info ) noexcept;


public:
    /// See Kit::Driver::IStart
    bool start( void* startArgs = nullptr ) noexcept override;

    /// See Kit::Driver::IStop
    void stop() noexcept override;


public:
    /// See Kit::Driver::Flash::IApi
    bool read( size_t srcOffset,
               void*  dstBuffer,
               size_t numBytes ) noexcept override;

    /// See Kit::Driver::Flash::IApi
    bool write( size_t      dstOffset,
                const void* srcBuffer,
                size_t      numBytes ) noexcept override;

    /// See Kit::Driver::Flash::IApi
    bool eraseSector( size_t sectorAddress ) noexcept override;

    /// See Kit::Driver::Flash::IApi
    bool eraseBlock32K( size_t blockAddress ) noexcept override;

    /// See Kit::Driver::Flash::IApi
    bool eraseBlock64K( size_t blockAddress ) noexcept override;

    /// See Kit::Driver::Flash::IApi
    bool eraseChip() noexcept override;


public:
    /// See Kit::Driver::Flash::IApi
    size_t getTotalSize() const noexcept override;

    /// See Kit::Driver::Flash::IApi
    size_t getSectorSize() const noexcept override;

    /// See Kit::Driver::Flash::IApi
    size_t getPageSize() const noexcept override;

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

    /// Polls the status register BUSY bit until ready or timeout
    bool waitUntilReady( uint32_t timeoutMs = 1000 ) noexcept;

    /// Sends a single-byte SPI command
    bool sendCommand( uint8_t cmd ) noexcept;

    /// Sends a command followed by a 24-bit address
    bool sendCommandWithAddress( uint8_t cmd, size_t address ) noexcept;


protected:
    SPI::IApi&    m_spi;        //!< Reference to SPI driver
    Dio::IOutput& m_cs;         //!< Reference to chip select output
    DeviceInfo_T  m_deviceInfo; //!< Device configuration
    uint8_t       m_cmdBuffer[CMD_ADDR_SIZE]; //!< Command + address buffer
    bool          m_started;    //!< Driver started flag
};


}  // end namespaces
}
}
}
#endif  // end header latch
