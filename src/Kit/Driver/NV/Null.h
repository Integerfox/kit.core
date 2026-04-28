#ifndef KIT_DRIVER_NV_NULL_H_
#define KIT_DRIVER_NV_NULL_H_
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
#include "Kit/Driver/NV/IApi.h"
#include <string.h>

/// The default page size, in bytes
#ifndef OPTION_KIT_DRIVER_NV_NULL_PAGE_SIZE
#define OPTION_KIT_DRIVER_NV_NULL_PAGE_SIZE 128
#endif

/// The default total page count
#ifndef OPTION_KIT_DRIVER_NV_NULL_TOTAL_PAGES
#define OPTION_KIT_DRIVER_NV_NULL_TOTAL_PAGES ( 512 )
#endif

///
namespace Kit {
///
namespace Driver {
///
namespace NV {

/** This concrete class provides a 'null' implementation of the NV::IApi interface.
    The implementation reports success for all operations while not storing any
    written data. Reads return zero-filled data when the destination buffer is
    large enough. This allows the driver to be used as a passive 'stub' when a
    platform-specific driver is not available.
 */
class Null : public IApi
{
public:
    /// Constructor
    Null( size_t numPages     = OPTION_KIT_DRIVER_NV_NULL_TOTAL_PAGES,
          size_t bytesPerPage = OPTION_KIT_DRIVER_NV_NULL_PAGE_SIZE ) noexcept
        : m_numPages( numPages ), m_bytesPerPage( bytesPerPage )
    {
    }

public:
    /// See Kit::Driver::IStart for details
    bool start( void* = nullptr ) noexcept override
    {
        return true;
    }

    /// See Kit::Driver::IStop for details
    void stop() noexcept override {}

public:
    /// See Kit::Driver::IApi for details
    virtual bool write( size_t,
                        const void*,
                        size_t ) noexcept override { return true; }

    /// See Kit::Driver::IApi for details. Note: the 'dstData' is set to all zeros
    virtual bool read( size_t,
                       void*  dstData,
                       size_t sizeDstData,
                       size_t numBytesToRead ) noexcept override
    {
        if ( sizeDstData < numBytesToRead )
        {
            return false;
        }

        memset( dstData, 0, numBytesToRead );
        return true;
    }


public:
    /// See Kit::Driver::IApi for details
    size_t getNumPages() const noexcept override { return m_numPages; }

    /// See Kit::Driver::IApi for details
    size_t getPageSize() const noexcept override { return m_bytesPerPage; }

    /// See Kit::Driver::IApi for details
    size_t getTotalSize() const noexcept override { return m_numPages * m_bytesPerPage; }

protected:
    /// Number of pages
    size_t m_numPages;

    /// Bytes per page
    size_t m_bytesPerPage;
};

}  // end namespaces
}
}
#endif  // end header latch
