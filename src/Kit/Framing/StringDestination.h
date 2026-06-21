#ifndef KIT_FRAMING_STRINGDESTINATION_H_
#define KIT_FRAMING_STRINGDESTINATION_H_
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
#include "Kit/Text/IString.h"

///
namespace Kit {
///
namespace Framing {

/** This concrete class implements the IDestination interface using a Kit::Text::IString
    instance.

    This class is NOT thread-safe.
 */
class StringDestination : public IDestination
{
public:
    /// Constructor
    StringDestination( Kit::Text::IString& outputString ) noexcept
        : m_dstPtr( &outputString )
    {
    }

    /// Constructor
    StringDestination() noexcept
        : m_dstPtr( nullptr )
    {
    }

    /// Allows the consumer to change/set the Output string after construction
    void setOutput( Kit::Text::IString& outputString ) noexcept
    {
        m_dstPtr = &outputString;
    }

public:
    /// See Kit::Framing::IDestination
    bool startOutput() noexcept override
    {
        if ( m_dstPtr == nullptr )
        {
            return false;
        }
        m_dstPtr->clear();
        return true;
    }

    /// See Kit::Framing::IDestination
    bool appendOutput( const void* srcBuffer, Kit::Type::SSize_T numBytes ) noexcept override
    {
        if ( srcBuffer == nullptr || m_dstPtr == nullptr )
        {
            return false;
        }

        m_dstPtr->appendTo( static_cast<const char*>(srcBuffer), numBytes );
        return m_dstPtr->truncated() == false;  // If truncated, then the entire buffer was not appended
    }

    /// See Kit::Framing::IDestination
    bool endOutput() noexcept override
    {
        return m_dstPtr != nullptr;  // No special handling needed to 'end' the output sequence for a string
    }

protected:
    /// Underlying string output
    Kit::Text::IString* m_dstPtr;
};


}  // end namespaces
}
#endif  // end header latch
