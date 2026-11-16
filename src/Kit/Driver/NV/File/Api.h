#ifndef KIT_DRIVER_NV_FILE_API_H_
#define KIT_DRIVER_NV_FILE_API_H_
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


/// Number of Pages
#ifndef OPTION_KIT_DRIVER_NV_FILE_NUM_PAGES
#define OPTION_KIT_DRIVER_NV_FILE_NUM_PAGES 512
#endif

/** Number of bytes per page.  This is also the size of the stack buffer used
    to 'erase' a newly created storage file.
 */
#ifndef OPTION_KIT_DRIVER_NV_FILE_BYTES_PER_PAGE
#define OPTION_KIT_DRIVER_NV_FILE_BYTES_PER_PAGE 128
#endif

/** The filename to use as the physical storage
 */
#ifndef OPTION_KIT_DRIVER_NV_FILE_NAME
#define OPTION_KIT_DRIVER_NV_FILE_NAME "eeprom.bin"
#endif

/** The value used to erase the physical storage
 */
#ifndef OPTION_KIT_DRIVER_NV_FILE_ERASED_VALUE
#define OPTION_KIT_DRIVER_NV_FILE_ERASED_VALUE 0xFF
#endif

///
namespace Kit {
///
namespace Driver {
///
namespace NV {
///
namespace File {


/** This class implements the Non-volatile storage driver using the
    Kit::Io::File interfaces.  The storage is a single file.  When the driver
    is started, the file is created if it does not exist, and a file that is
    shorter than the storage size is padded (at its end) up to the storage
    size with OPTION_KIT_DRIVER_NV_FILE_ERASED_VALUE, i.e. existing content
    is preserved.  A file that is longer than the storage size is accepted
    as-is, and only the first getTotalSize() bytes are used.  This allows
    the application to change the storage size between releases, and it
    completes the initialization of a file whose padding was interrupted
    (e.g. a failed start() or a power loss).

    Because the Kit::Io::File interfaces use Kit::Type::SSize_T for offsets
    and byte counts, the driver fails to start if the total storage size
    exceeds the maximum value of Kit::Type::SSize_T.

    The interface itself is NOT thread safe. It is the responsibility of
    the users/clients of the driver to handle any threading issues.
 */
class Api : public IApi
{
public:
    /// Constructor
    Api( size_t      numPages     = OPTION_KIT_DRIVER_NV_FILE_NUM_PAGES,
         size_t      bytesPerPage = OPTION_KIT_DRIVER_NV_FILE_BYTES_PER_PAGE,
         const char* filename     = OPTION_KIT_DRIVER_NV_FILE_NAME ) noexcept;

public:
    /// See Kit::Driver::IStart. Note: 'startArgs' is not used
    bool start( void* startArgs = nullptr ) noexcept override;

    /// See Kit::Driver::IStop
    void stop() noexcept override;

    /// See Kit::Driver::NV::IApi
    bool write( size_t dstOffset, const void* srcData, size_t numBytesToWrite ) noexcept override;

    /// See Kit::Driver::NV::IApi
    bool read( size_t srcOffset, void* dstData, size_t sizeDstData, size_t numBytesToRead ) noexcept override;

    /// See Kit::Driver::NV::IApi
    size_t getNumPages() const noexcept override;

    /// See Kit::Driver::NV::IApi
    size_t getPageSize() const noexcept override;

    /// See Kit::Driver::NV::IApi
    size_t getTotalSize() const noexcept override;

public:
    /** Returns the current filename */
    inline const char* getFileName() const noexcept
    {
        return m_fname;
    }

    /** This method allows the client to change the filename that was
        specified via the constructor.  This method will FAIL if called
        when the driver is in its 'running' state.

        NOTE: Just the like constructor parameter, the filename is NOT copied; the
              caller is responsible for ensuring that the string remains
              valid while the driver is being used.
     */
    inline bool setFileName( const char* filename ) noexcept
    {
        if ( !m_started && filename != nullptr )
        {
            m_fname = filename;
            return true;
        }
        return false;
    }

protected:
    /// File name
    const char* m_fname;

    /// number of pages
    size_t m_numPages;

    /// bytes per page
    size_t m_pageSize;

    /// Actual storage size
    size_t m_totalSize;

    /// Started state
    bool m_started;
};


}  // end namespaces
}
}
}
#endif  // end header latch
