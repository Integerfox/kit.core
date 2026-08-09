#ifndef KIT_FRAMING_IENCODER_H_
#define KIT_FRAMING_IENCODER_H_
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

/** This abstract class defines an interface for 'creating/ouput' (encoding) a
    frame.  The concrete instance determines the output destination, e.g. a
    Stream, RAM buffer, etc. See the README.md file for details about a what a
    Frame is.
 */
class IEncoder
{
public:
    /** Begins the frame.  The method will return false if there an
        error occurred while writing to the output destination; else true is
        returned.  If this method is called twice - without an intervening
        call to endFrame() - a NEW frame is started and true is returned

        NOTE: On error, the framing sequence is left in an undefined state
              and should be re-started by calling startFrame()
     */
    virtual bool startFrame( void ) noexcept = 0;


    /** Outputs 'numBytes' of data (from 'srcBuffer') to the output destination.
        The  method will return false if there an error occurred while
        writing to  the output destination; else true is returned.  If this
        method is called without a previous call to startFrame(), i.e. the
        frame has NOT been started, false is returned.

        NOTE: On error, the framing sequence is left in an undefined state
              and should be re-started by calling startFrame()
     */
    virtual bool output( const void* srcBuffer, Kit::Type::SSize_T numBytes ) noexcept = 0;

    /// Convenience/Alias method to output a single byte at a time
    inline bool output( uint8_t srcByte ) noexcept
    {
        return output( &srcByte, 1 );
    }

    /** Ends the frame.  The method will return false if there an error
        occurred while writing to the output destination; else true is
        returned.  If this method is called twice - without an intervening
        call to startFrame() - false is returned.

        NOTE: On error, the framing sequence is left in an undefined state
              and should be re-started by calling startFrame()
     */
    virtual bool endFrame( void ) noexcept = 0;

public:
    /// Virtual Destructor
    virtual ~IEncoder() = default;
};


}  // end namespaces
}
#endif  // end header latch
