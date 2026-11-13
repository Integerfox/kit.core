#ifndef KIT_DRIVER_NV_WRITE_WITH_VERIFY_H_
#define KIT_DRIVER_NV_WRITE_WITH_VERIFY_H_
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
#include <stdint.h>

/** Size, in bytes, of the temporary buffer used when performing the read-verify
    operation
*/
#ifndef OPTION_KIT_DRIVER_NV_WRITE_WITH_VERIFY_TEMP_BUF_SIZE
#define OPTION_KIT_DRIVER_NV_WRITE_WITH_VERIFY_TEMP_BUF_SIZE 256
#endif

///
namespace Kit {
///
namespace Driver {
///
namespace NV {


/** This class extends IApi interface so that all write operations are verified
    by reading back the data after the write.  In addition, the class supports
    retrying write operations N time when there is read-back failure.

    The interface itself is NOT thread safe. It is the responsibility of
    the users/clients of the driver to handle any threading issues.
 */
class WriteWithVerify : public IApi
{
public:
    /** Constructor. 'numRetries' is the number of times a failed write (or
        read-back verification) is retried, i.e. a write is attempted at most
        'numRetries' + 1 times.  'numRetries' must be less than UINT8_MAX
        (the constructor asserts on violations).
     */
    WriteWithVerify( IApi& nvDriver, uint8_t numRetries = 0 ) noexcept;

public:
    /// See Kit::Driver::IStart
    bool start( void* startArgs = nullptr ) noexcept override;

    /// See Kit::Driver::IStop
    void stop() noexcept override;

public:
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

protected:
    /// Helper method
    bool readBack( size_t numBytesWritten, const void* srcData, size_t offset ) noexcept;

    /** Hook that is called before the write operation (i.e. before any
        write/retry attempts).  Returning false fails the write operation
        without writing any data.  Every call to preWrite() is paired with a
        call to postWrite() - even when preWrite() returns false - so that a
        child class can use the hooks to bracket the write operation (e.g.
        disable/enable write protection).  Default is NOP/return true.
     */
    virtual bool preWrite( size_t dstOffset, const void* srcData, size_t numBytesToWrite ) noexcept;

    /** Hook that is called after the write operation.  'writeResult' is the
        result of the write (and read-back verification) attempts, and is false
        when preWrite() returned false.  The return value is the result of the
        write() call - except when preWrite() returned false, in which case the
        write() call always fails.  Default is NOP/return 'writeResult'.
     */
    virtual bool postWrite( bool writeResult ) noexcept;

protected:
    /// Underlying NV driver that performs the physical reads/writes
    IApi& m_nvDriver;

    /// Temporary read buffer (purposely not instantiate on the stack)
    uint8_t m_tempBuf[OPTION_KIT_DRIVER_NV_WRITE_WITH_VERIFY_TEMP_BUF_SIZE];

    /// Number of retries to perform when there is failure
    uint8_t m_numRetries;
};


}  // end namespaces
}
}
#endif  // end header latch
