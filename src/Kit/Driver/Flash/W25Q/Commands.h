#ifndef KIT_DRIVER_FLASH_W25Q_COMMANDS_H_
#define KIT_DRIVER_FLASH_W25Q_COMMANDS_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file
    Winbond W25Q series magic constants (SPI command opcodes, status register
    bit definitions, and device geometry).  These constants are common to the
    W25Q family and are shared by the various concrete driver implementations
    (e.g. Standard SPI vs QSPI).  They are kept separate from any specific
    driver so that new driver variants can re-use them.
*/


#include <stdint.h>
#include <stdlib.h>

///
namespace Kit {
///
namespace Driver {
///
namespace Flash {
///
namespace W25Q {


/** W25Q SPI command opcodes */
enum Command_T
{
    WRITE_ENABLE       = 0x06,  //!< Enable write latch
    WRITE_DISABLE      = 0x04,  //!< Disable write latch
    READ_STATUS_REG1   = 0x05,  //!< Read status register 1
    READ_STATUS_REG2   = 0x35,  //!< Read status register 2
    WRITE_STATUS_REG   = 0x01,  //!< Write status register
    PAGE_PROGRAM       = 0x02,  //!< Program up to 256 bytes
    SECTOR_ERASE       = 0x20,  //!< Erase 4KB sector
    BLOCK_ERASE_32K    = 0x52,  //!< Erase 32KB block
    BLOCK_ERASE_64K    = 0xD8,  //!< Erase 64KB block
    CHIP_ERASE         = 0xC7,  //!< Erase entire chip
    READ_DATA          = 0x03,  //!< Read at up to 50MHz
    FAST_READ          = 0x0B,  //!< Read at up to 133MHz
    JEDEC_ID           = 0x9F,  //!< Read JEDEC manufacturer ID
    POWER_DOWN         = 0xB9,  //!< Enter low power mode
    RELEASE_POWER_DOWN = 0xAB,  //!< Exit low power mode
    ENABLE_RESET       = 0x66,  //!< Enable software reset
    RESET_DEVICE       = 0x99   //!< Reset device
};

/** Status Register 1 bit definitions */
enum StatusReg1_T
{
    BUSY = 0x01,  //!< Bit 0: erase/write in progress
    WEL  = 0x02,  //!< Bit 1: write enable latch
    BP0  = 0x04,  //!< Bit 2: block protect 0
    BP1  = 0x08,  //!< Bit 3: block protect 1
    BP2  = 0x10,  //!< Bit 4: block protect 2
    TB   = 0x20,  //!< Bit 5: top/bottom protect
    SEC  = 0x40,  //!< Bit 6: sector protect
    SRP0 = 0x80   //!< Bit 7: status register protect
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
static constexpr DeviceInfo_T W25Q64  = { 8 * 1024 * 1024, 4096, 256, 2048 };
static constexpr DeviceInfo_T W25Q32  = { 4 * 1024 * 1024, 4096, 256, 1024 };
static constexpr DeviceInfo_T W25Q16  = { 2 * 1024 * 1024, 4096, 256, 512 };

/** Size of the JEDEC device ID response in bytes */
static constexpr size_t JEDEC_ID_SIZE = 3;


}  // end namespaces
}
}
}
#endif  // end header latch
