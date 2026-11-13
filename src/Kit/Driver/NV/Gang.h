#ifndef KIT_DRIVER_NV_GANG_H_
#define KIT_DRIVER_NV_GANG_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */


#include "Kit/Driver/NV/IApi.h"
#include <stdint.h>


///
namespace Kit {
///
namespace Driver {
///
namespace NV {


/** This class implements the Non-volatile storage driver using a collection
    of NV drivers to provide a 'flat address' space that spans multiple
    discrete storage chips/media.

    The individual drivers/nv-storage-instance can be different sizes and
    different physical media.

    The interface itself is NOT thread safe. It is the responsibility of
    the users/clients of the driver to handle any threading issues.
 */
class Gang : public IApi
{
public:
    /** Constructor. The 'drivers' argument is an array of pointers to concrete
        NV drivers and 'numDrivers' is the number of elements in the array.
        The array must contain at least one driver and none of the array
        elements can be a nullptr (the constructor asserts on violations).
        The array is NOT copied, i.e. it must remain valid for the lifetime
        of the Gang instance. The constructor does not access the drivers
        themselves.
     */
    Gang( IApi* drivers[], unsigned numDrivers ) noexcept;

public:
    /** See Kit::Driver::IStart.  The 'startArgs' argument is passed to the
        start() method of each of the ganged drivers.
     */
    bool start( void* startArgs = nullptr ) noexcept override;

    /// See Kit::Driver::IStop
    void stop() noexcept override;

public:
    /// See Kit::Driver::NV::IApi
    bool write( size_t dstOffset, const void* srcData, size_t numBytesToWrite ) noexcept override;

    /// See Kit::Driver::NV::IApi
    bool read( size_t srcOffset, void* dstData, size_t sizeDstData, size_t numBytesToRead ) noexcept override;

    /** See Kit::Driver::NV::IApi.  The number of pages is based on the smallest
        page size from the various ganged drivers that fits within the total
        available space. When the ganged drivers have pages size that are not
        multiples of each other, the returned value can be just approximate
        number of pages.
     */
    size_t getNumPages() const noexcept override;

    /** See Kit::Driver::NV::IApi. The reported page size is the smallest page
        size from the various ganged drivers.  The value is calculated on
        demand, i.e. it is not cached, and is only valid while the instance
        is started (returns zero when the instance is stopped).
     */
    size_t getPageSize() const noexcept override;

    /** See Kit::Driver::NV::IApi.  Reports the total available storage size.
        This method accurately returns the total amount of storage.
     */
    size_t getTotalSize() const noexcept override;

protected:
    /// Helper method to read data
    bool driverRead( IApi&     driver,
                     size_t    normalizedSrcOffset,
                     uint8_t*& dstData,
                     size_t&   numBytesToRead ) noexcept;

    /// Helper method to write data
    bool driverWrite( IApi&           driver,
                      size_t          normalizedDstOffset,
                      const uint8_t*& srcData,
                      size_t&         numBytesToWrite ) noexcept;


protected:
    /// List of drivers
    IApi** m_drivers;

    /// Total size (calculated by start())
    size_t m_totalSize;

    /// Number of drivers in the list
    unsigned m_numDrivers;

    /// Started state
    bool m_started;
};


}  // end namespaces
}
}
#endif  // end header latch
