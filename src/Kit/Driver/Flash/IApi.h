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
    driver.  The interface provides read, write, and erase operations at
    various granularities (sector, 32KB block, 64KB block, full chip).

    The interface assumes a flash device organized into sectors (smallest
    erasable unit) and pages (largest single-program unit).  Writes that
    cross page boundaries are handled internally by the implementation.

    The interface itself is NOT thread safe. It is the responsibility of
    the users/clients of the driver to handle any threading issues.
 */
class IApi : public virtual IStart,
             public virtual IStop
{
public:
    /** This method reads 'numBytes' from flash starting at 'srcOffset'
        into 'dstBuffer'.  The application is responsible for ensuring that
        'dstBuffer' is at least 'numBytes' in length.

        The method returns true if the operation was successful; else false
        is returned.
     */
    virtual bool read( size_t srcOffset,
                       void*  dstBuffer,
                       size_t numBytes ) noexcept = 0;

    /** This method writes 'numBytes' from 'srcBuffer' to flash starting
        at 'dstOffset'.  Writes that cross flash page boundaries are handled
        automatically.  The application is responsible for ensuring that
        'srcBuffer' is at least 'numBytes' in length.

        The method returns true if the operation was successful; else false
        is returned.
     */
    virtual bool write( size_t      dstOffset,
                        const void* srcBuffer,
                        size_t      numBytes ) noexcept = 0;

    /** This method erases one flash sector (typically 4KB) at the specified
        sector-aligned address.

        The method returns true if the operation was successful; else false
        is returned.
     */
    virtual bool eraseSector( size_t sectorAddress ) noexcept = 0;

    /** This method erases a 32KB block at the specified block-aligned address.

        The method returns true if the operation was successful; else false
        is returned.
     */
    virtual bool eraseBlock32K( size_t blockAddress ) noexcept = 0;

    /** This method erases a 64KB block at the specified block-aligned address.

        The method returns true if the operation was successful; else false
        is returned.
     */
    virtual bool eraseBlock64K( size_t blockAddress ) noexcept = 0;

    /** This method erases the entire flash chip.

        The method returns true if the operation was successful; else false
        is returned.
     */
    virtual bool eraseChip() noexcept = 0;


public:
    /** Returns the total flash device size in bytes */
    virtual size_t getTotalSize() const noexcept = 0;

    /** Returns the flash sector size in bytes (smallest erasable unit) */
    virtual size_t getSectorSize() const noexcept = 0;

    /** Returns the flash page size in bytes (largest single-program unit) */
    virtual size_t getPageSize() const noexcept = 0;

    /** Returns the total number of sectors in the flash device */
    virtual size_t getNumSectors() const noexcept = 0;


public:
    /// Virtual destructor
    virtual ~IApi() noexcept = default;
};


}  // end namespaces
}
}
#endif  // end header latch
