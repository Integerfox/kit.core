#ifndef KIT_FRAMING_STREAM_SOURCE_H_
#define KIT_FRAMING_STREAM_SOURCE_H_
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
#include "Kit/Io/IInput.h"


///
namespace Kit {
///
namespace Framing {

/** This concrete class implements the ISource interface using a Kit::Io::IInput
    instance.

    This class is NOT thread-safe.
 */
class StreamSource : public ISource
{
public:
    /// Constructor
    StreamSource( Kit::Io::IInput& inputStream ) noexcept
        : m_srcPtr( &inputStream )
    {
    }

    /// Constructor
    StreamSource() noexcept
        : m_srcPtr( nullptr )
    {
    }

    /// Allows the consumer to change/set the Input stream after construction
    void setInput( Kit::Io::IInput& inputStream ) noexcept
    {
        m_srcPtr = &inputStream;
    }

public:
    /// See Kit::Framing::ISource
     bool read( void*               dstBuffer,
                Kit::Type::SSize_T  numBytes,
                Kit::Type::SSize_T& bytesRead ) noexcept override
    {
        // Validate parameters and do NOT allow the read call to block if no data is currently available
        if ( dstBuffer == nullptr || m_srcPtr == nullptr || m_srcPtr->available() == false )
        {
            bytesRead = 0;
            return false;
        }

        return m_srcPtr->read( dstBuffer, numBytes, bytesRead );
    }

    /// Getter for the underlying input stream
    inline Kit::Io::IInput* getStream() noexcept
    {
        return m_srcPtr;
    }

    protected:
    /// Underlying input stream
    Kit::Io::IInput* m_srcPtr;
};


}  // end namespaces
}
#endif  // end header latch
