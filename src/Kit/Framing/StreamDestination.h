#ifndef KIT_FRAMING_STREAMDESTINATION_H_
#define KIT_FRAMING_STREAMDESTINATION_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Framing/IDestination.h"
#include "Kit/Io/IOutput.h"

///
namespace Kit {
///
namespace Framing {

/** This concrete class implements the IDestination interface using a Kit::Io::Output
    instance.

    This class is NOT thread-safe.
 */
class StreamDestination : public IDestination
{
public:
    /// Constructor
    StreamDestination( Kit::Io::IOutput& outputStream ) noexcept
        : m_dstPtr( &outputStream )
    {
    }

    /// Constructor
    StreamDestination() noexcept
        : m_dstPtr( nullptr )
    {
    }

    /// Allows the consumer to change/set the Output stream after construction
    void setOutput( Kit::Io::IOutput& outputStream ) noexcept
    {
        m_dstPtr = &outputStream;
    }

public:
    /// See Kit::Framing::IDestination
    bool startOutput() noexcept override
    {
        return m_dstPtr != nullptr;  // No special handling needed to 'start' the output sequence for a stream
    }

    /// See Kit::Framing::IDestination
    bool appendOutput( const void* srcBuffer, Kit::Type::SSize_T numBytes ) noexcept override
    {
        if ( srcBuffer == nullptr || m_dstPtr == nullptr )
        {
            return false;
        }

        return m_dstPtr->write( srcBuffer, numBytes );
    }

    /// See Kit::Framing::IDestination
    bool endOutput() noexcept override
    {
        return m_dstPtr != nullptr;  // No special handling needed to 'end' the output sequence for a stream
    }

    /// Getter for the underlying output stream
    inline Kit::Io::IOutput* getStream() noexcept
    {
        return m_dstPtr;
    }

protected:
    /// Underlying output stream
    Kit::Io::IOutput* m_dstPtr;
};


}  // end namespaces
}
#endif  // end header latch
