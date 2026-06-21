/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "DecoderReader.h"
#include <string.h>

//------------------------------------------------------------------------------
namespace Kit {
namespace Framing {

///////////////////
void DecoderReader::initializeScan() noexcept
{
    m_framePtr  = nullptr;
    m_frameSize = 0;
    m_inFrame   = false;
    m_escaping  = false;
}

uint8_t DecoderReader::decodeEscapedByte( uint8_t escapedByte ) noexcept
{
    return escapedByte;
}

bool DecoderReader::isLegalByte( uint8_t byte ) noexcept
{
    return true;
}

///////////////////
bool DecoderReader::scan( Kit::Type::SSize_T  maxSizeOfFrameBuffer,
                          uint8_t*            frameBuffer,
                          Kit::Type::SSize_T& frameSize ) noexcept
{
    // Housekeeping
    initializeScan();

    // Zero out size of the found frame
    frameSize = 0;

    // Error case: Treat a null frame buffer as an IO failure case
    if ( !frameBuffer )
    {
        return false;
    }

    // Scan till a frame is found
    for ( ;; )
    {
        // Read N characters at time
        bool isEof;
        if ( !scan( maxSizeOfFrameBuffer, frameBuffer, frameSize, isEof ) )
        {
            return false;  // Read/IO error occurred
        }
        else if ( isEof )
        {
            return true;  // Frame Found!
        }
    }
}

bool DecoderReader::scan( Kit::Type::SSize_T  maxSizeOfFrameBuffer,
                          uint8_t*            frameBuffer,
                          Kit::Type::SSize_T& frameSize,
                          bool&               isEof ) noexcept
{
    // Default to in-progress
    isEof = false;

    // Get more input data once my local buffer/cache is empty
    if ( !m_dataLen )
    {
        if ( !m_src.read( m_buffer, m_bufSize, m_dataLen ) )
        {
            // Error reading data -->exit scan
            m_dataLen = 0;  // Reset my internal count so I start 'over' on the next call (if there is one)
            frameSize = m_frameSize;
            initializeScan();
            return false;
        }

        // Reset my data pointer
        m_dataPtr = m_buffer;
    }

    // Process my input buffer one character at a time
    for ( ; m_dataLen; m_dataLen--, m_dataPtr++ )
    {
        // OUTSIDE of a frame
        if ( !m_inFrame )
        {
            if ( isStartOfFrame( *m_dataPtr ) )
            {
                m_inFrame   = true;
                m_escaping  = false;
                m_frameSize = 0;
                m_framePtr  = frameBuffer;
            }
        }

        // INSIDE a frame
        else
        {
            // Trap illegal characters
            if ( !isLegalByte( *m_dataPtr ) )
            {
                m_inFrame = false;
            }

            // No escape sequence in progress
            else if ( !m_escaping )
            {
                // EOF Character
                if ( isEndOfFrame( *m_dataPtr ) )
                {
                    // EXIT routine with a success return code
                    m_dataPtr++;  // Explicitly consume the EOF character (since we are brute force exiting the loop)
                    m_dataLen--;
                    frameSize = m_frameSize;
                    isEof     = true;
                    initializeScan();  // Reset my internal frame state to be ready for the next frame
                    return true;
                }

                // SOF Character -->need to reset the frame
                else if ( isStartOfFrame( *m_dataPtr ) )
                {
                    m_frameSize = 0;
                    m_framePtr  = frameBuffer;
                }

                // Regular character
                else if ( !isEscapeByte( *m_dataPtr ) )
                {
                    // Store incoming character into the Client's buffer
                    if ( m_frameSize < maxSizeOfFrameBuffer )
                    {
                        *m_framePtr++ = *m_dataPtr;
                        m_frameSize++;
                    }

                    // Exceeded the Client's buffer space -->internal error -->reset my Frame state
                    else
                    {
                        initializeScan();
                    }
                }

                // Start escape sequence
                else
                {
                    m_escaping = true;
                }
            }


            // Escape Sequence
            else
            {
                // Store the escaped character into the Client's buffer
                if ( m_frameSize < maxSizeOfFrameBuffer )
                {
                    m_escaping    = false;
                    *m_framePtr++ = decodeEscapedByte( *m_dataPtr );
                    m_frameSize++;
                }

                // Exceeded the Client's buffer space -->internal error -->reset my Frame state
                else
                {
                    initializeScan();
                }
            }
        }
    }

    // If I get here there was no IO error - but still no End-of-Frame
    frameSize = m_frameSize;
    return true;
}


bool DecoderReader::oobRead( uint8_t*            buffer,
                             Kit::Type::SSize_T  numBytes,
                             Kit::Type::SSize_T& bytesRead ) noexcept
{
    // FAIL if processing a frame
    if ( m_inFrame )
    {
        return false;
    }

    // No cached data
    if ( !m_dataLen )
    {
        // Read data from the input source into the local cache
        if ( !m_src.read( m_buffer, m_bufSize, m_dataLen ) )
        {
            return false;
        }
        m_dataPtr = m_buffer;
    }

    // More data requested than what is cached
    if ( numBytes > m_dataLen )
    {
        bytesRead = m_dataLen;
        m_dataLen = 0;
        memcpy( buffer, m_dataPtr, bytesRead );
        return true;
    }

    // Consume part of the cached data
    memcpy( buffer, m_dataPtr, numBytes );
    m_dataLen -= numBytes;
    m_dataPtr += numBytes;
    bytesRead  = numBytes;
    return true;
}


}  // end namespace
}
//------------------------------------------------------------------------------