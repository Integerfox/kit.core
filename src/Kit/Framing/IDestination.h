#ifndef KIT_FRAMING_IDESTINATION_H_
#define KIT_FRAMING_IDESTINATION_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Type/SSize.h"


///
namespace Kit {
///
namespace Framing {

/** This abstract class defines an interface of the output destination for an
    Encoder instance. The concrete instance determines the 'destination', e.g. a
    Stream, RAM buffer, etc.
 */
class IDestination
{
public:
    /** Used to initialize the "frame output sequence".
        Returns true if successful, false if no space is available
        in the destination, or an error was encountered.
    */
    virtual bool startOutput() noexcept = 0;

    /** Outputs the next N bytes to the destination.
        Returns true if successful, false if 'no space' is available
        in the destination, or an error was encountered.
    */
    virtual bool appendOutput( const void* srcBuffer, Kit::Type::SSize_T numBytes ) noexcept = 0;

    /** Convenience/Alias method to output a single byte at a time */
    inline bool appendOutput( uint8_t srcByte ) noexcept
    {
        return appendOutput( &srcByte, 1 );
    }

    /** Used to finalize the "frame output sequence".
        Returns true if successful, false if 'no space' is available
        in the destination, or an error was encountered.

        NOTE: There is NO guarantee that endOutput() will always be called
              before starting a new frame.  What is guaranteed is that startOutput()
              will be called with each new attempted frame output sequence.
    */
    virtual bool endOutput() noexcept = 0;

public:
    /// Virtual Destructor
    virtual ~IDestination() = default;
};


}  // end namespaces
}
#endif  // end header latch
