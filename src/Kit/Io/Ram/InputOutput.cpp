/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "InputOutput.h"
#include <string.h>

using namespace Kit::System;

//------------------------------------------------------------------------------
namespace Kit {
namespace Io {
namespace Ram {


////////////////////////////////////
bool InputOutput::available() noexcept
{
    if ( !m_started )
    {
        return false;
    }
    Mutex::ScopeLock criticalSection( m_lock );
    return m_ringBuffer.isEmpty() == false;
}

bool InputOutput::read( void* buffer, Kit::Type::SSize_T numBytes, Kit::Type::SSize_T& bytesRead ) noexcept
{
    // Skip if no data was requested
    bytesRead = 0;
    if ( numBytes == 0 )
    {
        return true;
    }

    if ( !m_started || numBytes < 0 )
    {
        return false;
    }

    // Loop until at least one byte is read or the stream is closed
    for ( ; m_started; )
    {
        // Scope block is used to limit the critical section to just the peek/remove operations on the ring buffer
        {
            Mutex::ScopeLock criticalSection( m_lock );

            // Determine how much data is currently available in the ring buffer AS A SEQUENTIAL read
            unsigned numFlatBytesAvailable = 0;
            uint8_t* srcPtr                = m_ringBuffer.peekNextRemoveItems( numFlatBytesAvailable );

            // At least one byte is available -->fullfill the read request with the available data.
            if ( numFlatBytesAvailable != 0 )
            {
                // Copy the lesser of the requested bytes and the available bytes to the client buffer
                unsigned numBytesToRead = static_cast<unsigned>( numBytes ) < numFlatBytesAvailable ? static_cast<unsigned>( numBytes ) : numFlatBytesAvailable;
                memcpy( buffer, srcPtr, numBytesToRead );
                bytesRead = numBytesToRead;

                // Remove the data from the buffer
                m_ringBuffer.removeElements( static_cast<unsigned>( numBytesToRead ) );

                // Unblock the waiting writer (if any)
                if ( m_writerWaiting )
                {
                    m_writerWaiting = false;
                    m_semaWriter.signal();
                }
                return true;
            }
            else
            {
                // No data is currently available -->I will block until data is available
                m_readerWaiting = true;
            }
        }

        // No data available
        m_semaReader.wait();
    }

    // If get here, then stream was closed while I was waiting for data -->return false
    bytesRead = 0;
    return false;
}

bool InputOutput::write( const void* buffer, Kit::Type::SSize_T numBytesToTx, Kit::Type::SSize_T& bytesWritten ) noexcept
{
    // Ignore write of zero bytes
    bytesWritten = 0;
    if ( numBytesToTx == 0 )
    {
        return true;
    }

    // Fail if not started
    if ( !m_started || numBytesToTx < 0 )
    {
        return false;
    }


    // Loop until at least one byte has been transferred OR the stream is closed
    for ( ; m_started; )
    {
        // Scope block is used to limit the critical section to just the peek/insert operations on the ring buffer
        {
            Mutex::ScopeLock criticalSection( m_lock );

            // Determine how much space is currently available in the ring buffer AS A SEQUENTIAL write
            unsigned numFlatBytesAvailable = 0;
            uint8_t* dstPtr                = m_ringBuffer.peekNextAddItems( numFlatBytesAvailable );

            // At least one byte is available -->fullfill the write request with the available space.
            if ( numFlatBytesAvailable != 0 )
            {
                // Copy the lesser of the requested bytes and the available bytes to the client buffer
                unsigned numBytesToWrite = static_cast<unsigned>( numBytesToTx ) < numFlatBytesAvailable ? static_cast<unsigned>( numBytesToTx ) : numFlatBytesAvailable;
                memcpy( dstPtr, buffer, numBytesToWrite );
                bytesWritten = numBytesToWrite;

                // Add the data to the buffer
                m_ringBuffer.addElements( static_cast<unsigned>( numBytesToWrite ) );

                // Unblock the waiting reader (if any)
                if ( m_readerWaiting )
                {
                    m_readerWaiting = false;
                    m_semaReader.signal();
                }
                return true;
            }
            else
            {
                // No space is currently available
                m_writerWaiting = true;
            }
        }

        // No space available -->block until space is available
        m_semaWriter.wait();
    }

    // If get here, then stream was closed while I was waiting for space -->return false
    bytesWritten = 0;
    return false;
}


void InputOutput::flush() noexcept
{
    // Has no meaning
}

bool InputOutput::isEos() noexcept
{
    // Has no meaning, always return false except when closed -->then return true
    return m_started == false;
}

void InputOutput::close() noexcept
{
    // By design, no explicit critical section is used.  Instead leveraging the
    // the 'atomic' nature of volatile bool variables to avoid potential deadlock
    // scenarios with the m_lock mutex

    // Only take action if I am not already closed.
    if ( m_started )
    {
        m_started = false;
        if ( m_readerWaiting )
        {
            m_semaReader.signal();
        }
        if ( m_writerWaiting )
        {
            m_semaWriter.signal();
        }
    }
}

}  // end namespace
}
}
//------------------------------------------------------------------------------