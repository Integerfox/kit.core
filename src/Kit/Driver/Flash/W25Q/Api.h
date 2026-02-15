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


#include "kit_config.h"
#include "Kit/Driver/Flash/IApi.h"
#include "Kit/Driver/SPI/IApi.h"
#include "Kit/Driver/Dio/IOutput.h"

///
namespace Kit {
///
namespace Driver {
///
namespace Flash {
///
namespace W25Q {


/** W25Q series command definitions */
struct Commands_T
{
    static constexpr uint8_t WRITE_ENABLE       = 0x06;  ///< Enable write operations
    static constexpr uint8_t WRITE_DISABLE      = 0x04;  ///< Disable write operations
    static constexpr uint8_t READ_STATUS_REG1   = 0x05;  ///< Read Status Register-1
    static constexpr uint8_t READ_STATUS_REG2   = 0x35;  ///< Read Status Register-2
    static constexpr uint8_t WRITE_STATUS_REG   = 0x01;  ///< Write Status Register
    static constexpr uint8_t READ_DATA          = 0x03;  ///< Read data (standard)
    static constexpr uint8_t FAST_READ          = 0x0B;  ///< Fast read data
    static constexpr uint8_t PAGE_PROGRAM       = 0x02;  ///< Page program (write)
    static constexpr uint8_t SECTOR_ERASE_4K    = 0x20;  ///< Sector erase (4KB)
    static constexpr uint8_t BLOCK_ERASE_32K    = 0x52;  ///< Block erase (32KB)
    static constexpr uint8_t BLOCK_ERASE_64K    = 0xD8;  ///< Block erase (64KB)
    static constexpr uint8_t CHIP_ERASE         = 0xC7;  ///< Chip erase (0x60 also works)
    static constexpr uint8_t POWER_DOWN         = 0xB9;  ///< Enter power-down mode
    static constexpr uint8_t RELEASE_POWER_DOWN = 0xAB;  ///< Release from power-down
    static constexpr uint8_t DEVICE_ID          = 0xAB;  ///< Read device ID
    static constexpr uint8_t JEDEC_ID           = 0x9F;  ///< Read JEDEC ID
    static constexpr uint8_t UNIQUE_ID          = 0x4B;  ///< Read unique ID
};


/** Status Register-1 bit definitions */
struct StatusReg1_T
{
    static constexpr uint8_t BUSY  = 0x01;  ///< Erase/Write in progress
    static constexpr uint8_t WEL   = 0x02;  ///< Write Enable Latch
    static constexpr uint8_t BP0   = 0x04;  ///< Block Protect bit 0
    static constexpr uint8_t BP1   = 0x08;  ///< Block Protect bit 1
    static constexpr uint8_t BP2   = 0x10;  ///< Block Protect bit 2
    static constexpr uint8_t TB    = 0x20;  ///< Top/Bottom protect
    static constexpr uint8_t SEC   = 0x40;  ///< Sector/Block protect
    static constexpr uint8_t SRP0  = 0x80;  ///< Status Register Protect
};


/** Device size configurations for common W25Q variants */
struct DeviceInfo_T
{
    uint32_t totalSize;      ///< Total flash size in bytes
    uint32_t sectorSize;     ///< Sector size in bytes (4KB)
    uint32_t pageSize;       ///< Page size in bytes (256)
    uint32_t numSectors;     ///< Number of sectors
};


/** Pre-defined device configurations */
namespace Devices {
    /** W25Q128JV (128 Mbit / 16 MB) */
    constexpr DeviceInfo_T W25Q128 = {
        .totalSize   = 16 * 1024 * 1024,  // 16 MB
        .sectorSize  = 4 * 1024,          // 4 KB
        .pageSize    = 256,               // 256 bytes
        .numSectors  = 4096               // 4096 sectors
    };

    /** W25Q64 (64 Mbit / 8 MB) */
    constexpr DeviceInfo_T W25Q64 = {
        .totalSize   = 8 * 1024 * 1024,   // 8 MB
        .sectorSize  = 4 * 1024,          // 4 KB
        .pageSize    = 256,               // 256 bytes
        .numSectors  = 2048               // 2048 sectors
    };

    /** W25Q32 (32 Mbit / 4 MB) */
    constexpr DeviceInfo_T W25Q32 = {
        .totalSize   = 4 * 1024 * 1024,   // 4 MB
        .sectorSize  = 4 * 1024,          // 4 KB
        .pageSize    = 256,               // 256 bytes
        .numSectors  = 1024               // 1024 sectors
    };

    /** W25Q16 (16 Mbit / 2 MB) */
    constexpr DeviceInfo_T W25Q16 = {
        .totalSize   = 2 * 1024 * 1024,   // 2 MB
        .sectorSize  = 4 * 1024,          // 4 KB
        .pageSize    = 256,               // 256 bytes
        .numSectors  = 512                // 512 sectors
    };
}


/** This concrete class implements the flash driver interface for Winbond W25Q
    series SPI NOR flash memory chips.

    The driver uses standard SPI mode (Mode 0 or Mode 3) and 3-byte addressing.
    The application must provide:
    - An SPI driver instance for communication
    - A Digital Output instance for chip select (CS) control

    ## Usage Example
    ```cpp
    // Hardware instances (platform specific)
    Kit::Driver::SPI::MyPlatformSpi     spi;
    Kit::Driver::Dio::MyPlatformOutput  csPin;

    // Create W25Q driver for W25Q128JV
    Kit::Driver::Flash::W25Q::Api flash( spi, csPin, Kit::Driver::Flash::W25Q::Devices::W25Q128 );

    // Start the driver
    flash.start();

    // Read 100 bytes from address 0x1000
    uint8_t buffer[100];
    flash.read( 0x1000, buffer, sizeof(buffer) );

    // Erase sector before writing
    flash.eraseSector( 0x1000 );

    // Write data
    uint8_t data[] = { 0x01, 0x02, 0x03, 0x04 };
    flash.write( 0x1000, data, sizeof(data) );
    ```
 */
class Api : public Kit::Driver::Flash::IApi
{
public:
    /** Constructor.

        @param spi          Reference to SPI driver for communication
        @param chipSelect   Reference to Digital Output for CS control
        @param deviceInfo   Device configuration (size, sectors, etc.)
     */
    Api( Kit::Driver::SPI::IApi&       spi,
         Kit::Driver::Dio::IOutput&    chipSelect,
         const DeviceInfo_T&           deviceInfo ) noexcept;


public:
    /// See Kit::Driver::IStart
    bool start( void* startArgs = nullptr ) noexcept override;

    /// See Kit::Driver::IStop
    void stop() noexcept override;


public:
    /// See Kit::Driver::Flash::IApi
    bool read( uint32_t address,
               void*    dstData,
               size_t   numBytes ) noexcept override;

    /// See Kit::Driver::Flash::IApi
    bool write( uint32_t    address,
                const void* srcData,
                size_t      numBytes ) noexcept override;

    /// See Kit::Driver::Flash::IApi
    bool eraseSector( uint32_t address ) noexcept override;

    /// See Kit::Driver::Flash::IApi
    bool eraseBlock32K( uint32_t address ) noexcept override;

    /// See Kit::Driver::Flash::IApi
    bool eraseBlock64K( uint32_t address ) noexcept override;

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
    /** This method reads the JEDEC ID from the flash device.
        The JEDEC ID contains manufacturer ID and device ID information.

        @param manufacturerId   Output: Manufacturer ID (0xEF for Winbond)
        @param memoryType       Output: Memory type
        @param capacity         Output: Capacity code

        @return true if successful, false otherwise
     */
    bool readJedecId( uint8_t& manufacturerId,
                      uint8_t& memoryType,
                      uint8_t& capacity ) noexcept;

    /** This method checks if the flash is currently busy with an erase
        or write operation.

        @return true if busy, false if idle
     */
    bool isBusy() noexcept;


protected:
    /** Sends the Write Enable command */
    void writeEnable() noexcept;

    /** Sends the Write Disable command */
    void writeDisable() noexcept;

    /** Reads Status Register-1 */
    uint8_t readStatusReg1() noexcept;

    /** Waits for the flash to complete any pending operation */
    void waitUntilReady() noexcept;

    /** Writes a single page (max 256 bytes) */
    bool writePage( uint32_t address, const uint8_t* data, size_t numBytes ) noexcept;

    /** Asserts (sets low) the chip select signal */
    void csAssert() noexcept;

    /** Deasserts (sets high) the chip select signal */
    void csDeassert() noexcept;


protected:
    /// SPI driver reference
    Kit::Driver::SPI::IApi& m_spi;

    /// Chip select output reference
    Kit::Driver::Dio::IOutput& m_chipSelect;

    /// Device configuration
    const DeviceInfo_T& m_deviceInfo;

    /// Flag indicating if driver is started
    bool m_isStarted;


private:
    /// Prevent access to the copy constructor
    Api( const Api& ) = delete;

    /// Prevent access to the assignment operator
    const Api& operator=( const Api& ) = delete;
};


};  // end namespaces
};
};
};
#endif  // end header latch
