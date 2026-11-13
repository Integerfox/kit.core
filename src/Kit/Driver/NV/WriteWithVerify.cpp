/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */


#include "Kit/Driver/NV/WriteWithVerify.h"
#include "Kit/System/Assert.h"
#include <string.h>


//------------------------------------------------------------------------------
namespace Kit {
namespace Driver {
namespace NV {

////////////////////////////////////////
WriteWithVerify::WriteWithVerify( IApi& nvDriver, uint8_t numRetries ) noexcept
    : m_nvDriver( nvDriver )
    , m_numRetries( numRetries )
{
    // The write is attempted 'numRetries' + 1 times, which must fit in a uint8_t
    KIT_SYSTEM_ASSERT( numRetries < UINT8_MAX );
}

bool WriteWithVerify::start( void* startArgs ) noexcept
{
    return m_nvDriver.start( startArgs );
}

void WriteWithVerify::stop() noexcept
{
    m_nvDriver.stop();
}

////////////////////
bool WriteWithVerify::preWrite( size_t, const void*, size_t ) noexcept
{
    // Default: NOP
    return true;
}

bool WriteWithVerify::postWrite( bool writeResult ) noexcept
{
    // Default: NOP
    return writeResult;
}

bool WriteWithVerify::write( size_t dstOffset, const void* srcData, size_t numBytesToWrite ) noexcept
{
    // Basic error checking
    if ( srcData == nullptr || !preWrite( dstOffset, srcData, numBytesToWrite ) )
    {
        return false;
    }

    // Attempt the write operation at least once
    bool    result     = false;
    uint8_t retryCount = m_numRetries + 1;
    while ( retryCount )
    {
        if ( m_nvDriver.write( dstOffset, srcData, numBytesToWrite ) )
        {
            if ( readBack( numBytesToWrite, srcData, dstOffset ) )
            {
                // Write and verify successful
                result = true;
                break;
            }
        }

        // Write failed or verification failed
        --retryCount;
    }

    return postWrite( result );
}

bool WriteWithVerify::read( size_t srcOffset, void* dstData, size_t sizeDstData, size_t numBytesToRead ) noexcept
{
    // Basic error checking
    if ( dstData == nullptr )
    {
        return false;
    }

    return m_nvDriver.read( srcOffset, dstData, sizeDstData, numBytesToRead );
}

size_t WriteWithVerify::getNumPages() const noexcept
{
    return m_nvDriver.getNumPages();
}

size_t WriteWithVerify::getPageSize() const noexcept
{
    return m_nvDriver.getPageSize();
}

size_t WriteWithVerify::getTotalSize() const noexcept
{
    return m_nvDriver.getTotalSize();
}

////////////////////////////////
bool WriteWithVerify::readBack( size_t numBytesWritten, const void* srcData, size_t offset ) noexcept
{
    // Verify the write by reading back the data
    const uint8_t* srcPtr = static_cast<const uint8_t*>( srcData );
    while ( numBytesWritten )
    {
        // Limit the amount data that can be read at one time
        size_t bytesToRead = numBytesWritten;
        if ( bytesToRead > OPTION_KIT_DRIVER_NV_WRITE_WITH_VERIFY_TEMP_BUF_SIZE )
        {
            bytesToRead = OPTION_KIT_DRIVER_NV_WRITE_WITH_VERIFY_TEMP_BUF_SIZE;
        }

        // Read back the data
        if ( m_nvDriver.read( offset, m_tempBuf, sizeof( m_tempBuf ), bytesToRead ) )
        {
            // Compare the written data with the read-back data
            if ( memcmp( srcPtr, m_tempBuf, bytesToRead ) != 0 )
            {
                // Data mismatch
                return false;
            }
        }

        // Fail the read back when we are not able to read back
        else
        {
            return false;
        }

        // Housekeeping
        numBytesWritten -= bytesToRead;
        offset          += bytesToRead;
        srcPtr          += bytesToRead;
    }

    // If I get here verify-read was successful
    return true;
}

}  // end namespace
}
}
//------------------------------------------------------------------------------
