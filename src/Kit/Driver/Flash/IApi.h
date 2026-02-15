#ifndef KIT_DRIVER_FLASH_IAPI_H_
#define KIT_DRIVER_FLASH_IAPI_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */


#include <stdint.h>
#include <stdlib.h>
#include "Kit/Driver/IStart.h"
#include "Kit/Driver/IStop.h"

///
namespace Kit {
///
namespace Driver {
///
namespace Flash {


/** This class defines the interface for a platform independent Flash memory
    driver. The interface provides operations for reading, writing, and erasing
    flash memory.

    Flash memory has important characteristics that differ from EEPROM:
    - Data must be erased (set to 0xFF) before it can be written
    - Erase operations work on entire sectors (typically 4KB)
    - Writing can only change bits from 1 to 0
    - Limited erase cycle lifetime (typically 100K cycles)

    The interface is designed to support both NOR and NAND flash devices,
    though specific implementations may have additional constraints.

    The interface itself is NOT thread safe. It is the responsibility of
    the users/clients of the driver to handle any threading issues.
 */
class IApi : public virtual IStart,
             public virtual IStop
{
public:
    /** This method reads data from flash memory starting at the specified
        address.

        @param address      The flash memory address to read from
        @param dstData      Pointer to buffer to store read data
        @param numBytes     Number of bytes to read

        @return true if the operation was successful; else false
     */
    virtual bool read( uint32_t address,
                       void*    dstData,
                       size_t   numBytes ) noexcept = 0;

    /** This method writes data to flash memory starting at the specified
        address. The target memory area MUST be erased (all 0xFF) before
        writing.

        Note: Many flash devices have page write size limitations (e.g., 256
        bytes for W25Q series). This method handles crossing page boundaries
        internally.

        @param address      The flash memory address to write to
        @param srcData      Pointer to data to write
        @param numBytes     Number of bytes to write

        @return true if the operation was successful; else false
     */
    virtual bool write( uint32_t    address,
                        const void* srcData,
                        size_t      numBytes ) noexcept = 0;

    /** This method erases a sector containing the specified address. After
        erasing, all bytes in the sector will be 0xFF.

        Note: This operation erases the ENTIRE sector containing the address,
        not just the bytes at that address.

        @param address      Any address within the sector to erase

        @return true if the operation was successful; else false
     */
    virtual bool eraseSector( uint32_t address ) noexcept = 0;

    /** This method erases a 32KB block containing the specified address.
        After erasing, all bytes in the block will be 0xFF.

        Note: Not all flash devices support 32KB block erase.

        @param address      Any address within the block to erase

        @return true if the operation was successful; else false
     */
    virtual bool eraseBlock32K( uint32_t address ) noexcept = 0;

    /** This method erases a 64KB block containing the specified address.
        After erasing, all bytes in the block will be 0xFF.

        @param address      Any address within the block to erase

        @return true if the operation was successful; else false
     */
    virtual bool eraseBlock64K( uint32_t address ) noexcept = 0;

    /** This method erases the entire flash chip. After erasing, all bytes
        will be 0xFF.

        WARNING: This operation can take several seconds to complete on
        large flash devices.

        @return true if the operation was successful; else false
     */
    virtual bool eraseChip() noexcept = 0;


public:
    /** This method returns the total flash memory size in bytes.
     */
    virtual size_t getTotalSize() const noexcept = 0;

    /** This method returns the sector size in bytes. A sector is the minimum
        erase unit for sector erase operations.
     */
    virtual size_t getSectorSize() const noexcept = 0;

    /** This method returns the page size in bytes. A page is the maximum
        number of bytes that can be written in a single write operation.
     */
    virtual size_t getPageSize() const noexcept = 0;

    /** This method returns the number of sectors in the flash device.
     */
    virtual size_t getNumSectors() const noexcept = 0;


public:
    /// Virtual destructor
    virtual ~IApi() noexcept = default;
};


};  // end namespaces
};
};
#endif  // end header latch
