#ifndef KIT_FRAMING_STRING_SOURCE_H_
#define KIT_FRAMING_STRING_SOURCE_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Framing/ISource.h"
#include <string.h>

///
namespace Kit {
///
namespace Framing {

/** This concrete class implements the ISource interface using a null terminated
    string as the input source.  The string is not modified by this class.

    This class is NOT thread-safe.
 */
class StringSource : public ISource
{
public:
    /// Constructor
    StringSource( const char* inputString ) noexcept
        : m_srcPtr( inputString )
        , m_currentIdx( 0 )
        , m_length( inputString != nullptr ? strlen( inputString ) : 0 )
    {
    }

    /// Constructor
    StringSource() noexcept
        : m_srcPtr( nullptr )
        , m_currentIdx( 0 )
        , m_length( 0 )
    {
    }

    /// Allows the consumer to change/set the Input string after construction
    void setInput( const char* inputString ) noexcept
    {
        m_srcPtr     = inputString;
        m_currentIdx = 0;
        m_length     = inputString != nullptr ? strlen( inputString ) : 0;
    }

public:
    /// See Kit::Framing::ISource
    bool read( void*               dstBuffer,
               Kit::Type::SSize_T  numBytes,
               Kit::Type::SSize_T& bytesRead ) noexcept override
    {
        // Validate parameters AND trap end-of-string has already been reached.
        if ( dstBuffer == nullptr || m_srcPtr == nullptr || m_currentIdx < 0 || numBytes < 0 )
        {
            bytesRead = 0;
            return false;
        }

        // Determine how many bytes can been read
        Kit::Type::SSize_T availableBytes = m_length - m_currentIdx;

        // Requesting all of the remaining bytes
        if ( numBytes >= availableBytes )
        {
            bytesRead = availableBytes;
            memcpy( dstBuffer, m_srcPtr + m_currentIdx, availableBytes );
            m_currentIdx = -1;  // Mark end of string reached
            return true;
        }

        // Partial read of the remaining bytes
        memcpy( dstBuffer, m_srcPtr + m_currentIdx, numBytes );
        m_currentIdx += numBytes;
        bytesRead     = numBytes;
        return true;
    }

protected:
    /// Underlying input string
    const char* m_srcPtr;

    /// Current read position within the input string. Negative value indicates the end of the string has been reached.
    Kit::Type::SSize_T m_currentIdx;

    /// Length of the input string (excluding the null terminator)
    Kit::Type::SSize_T m_length;
};


}  // end namespaces
}
#endif  // end header latch
