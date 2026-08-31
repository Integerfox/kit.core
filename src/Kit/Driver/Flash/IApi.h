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
#include <stdint.h>
#include "Kit/Driver/IStart.h"
#include "Kit/Driver/IStop.h"

///
namespace Kit {
///
namespace Driver {
///
namespace Flash {


/** This class defines the interface for a platform independent Flash memory
    driver.  The interface provides read, write, and erase operations.  Erasing
    is performed at sector granularity (one or more sectors) or the full chip.

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
    /// Return codes for flash operations
    enum Result_T
    {
        SUCCESS = 0,    //!< Operation completed successfully
        ERR_RANGE,      //!< Operation failed due to out-of-range access, e.g. address + numBytes > getTotalSize()
        ERR_ALIGNMENT,  //!< Operation failed due to improper address/sector alignment
        ERR_FAILED      //!< Operation failed (generic error)
    };

public:
    /** This method reads 'numBytes' from flash starting at 'srcOffset'
        into 'dstBuffer'.  The application is responsible for ensuring that
        'dstBuffer' is at least 'numBytes' in length.

        The method returns Result_T::SUCCESS if the operation was successful;
        else an error code is returned.
     */
    virtual Result_T read( size_t srcOffset,
                           void*  dstBuffer,
                           size_t numBytes ) noexcept = 0;

    /** This method writes 'numBytes' from 'srcBuffer' to flash starting
        at 'dstOffset'.  Writes that cross flash page boundaries are handled
        automatically.  The application is responsible for ensuring that
        'srcBuffer' is at least 'numBytes' in length.

        The method returns Result_T::SUCCESS if the operation was successful;
        else an error code is returned.
     */
    virtual Result_T write( size_t      dstOffset,
                            const void* srcBuffer,
                            size_t      numBytes ) noexcept = 0;

    /** This method erases 'numberOfSectors' flash sectors (typically 4KB each)
        starting at the specified sector-aligned address.

        Passing a 'numberOfSectors' greater than one allows the concrete
        implementation to optimize the request (e.g. converting eight 4KB
        sector erases into a single 32KB block erase command).

        The method returns Result_T::SUCCESS if the operation was successful;
        else an error code is returned.
     */
    virtual Result_T eraseSector( size_t   sectorAddress,
                                  unsigned numberOfSectors = 1 ) noexcept = 0;

    /** This method erases the entire flash chip.

        The method returns Result_T::SUCCESS if the operation was successful;
        else an error code is returned.
     */
    virtual Result_T eraseChip() noexcept = 0;

    /** This method blocks until the Flash is 'ready' (i.e. not busy with a
        physical write or erase operation).  The method returns true when the
        flash is ready; else false is returned if the timeout period expired
        before the flash was ready.
     */
    virtual bool waitUntilReady( uint32_t timeoutMs = 1000 ) noexcept = 0;


public:
    /** Returns the total flash device size in bytes */
    virtual size_t getTotalSize() const noexcept = 0;

    /** Returns the flash sector size in bytes (smallest erasable unit) */
    virtual size_t getSectorSize() const noexcept = 0;

    /** Returns the flash write page size in bytes (largest single-program
        unit used by the write() method)
     */
    virtual size_t getWritePageSize() const noexcept = 0;

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
