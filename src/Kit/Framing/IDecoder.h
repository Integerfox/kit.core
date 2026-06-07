#ifndef KIT_FRAMING_IDECODER_H_
#define KIT_FRAMING_IDECODER_H_
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

/** This abstract class defines an interface for 'receiving' (decoding) a frame.
    See the README.md file for details about a what a Frame is
 */
class IDecoder
{
public:
    /** This method reads from an Input source (which is defined/provided by
        the concrete implementation) until a valid frame is found or an error
        occurred.  If a valid frame was found, true will be returned and the
        frame will be stored in 'frameBuffer'.  The length, in bytes, of the frame found
        is returned via 'frameSize'.  False is returned if a error was
        encountered while reading the Input source.
     */
    virtual bool scan( Kit::Type::SSize_T  maxSizeOfFrameBuffer,
                       uint8_t*            frameBuffer,
                       Kit::Type::SSize_T& frameSize ) noexcept = 0;

    /** This method is similar to the above scan() method, except that it does
        NOT block till a 'frame' has found, instead it indicates when a 'frame'
        has been found by setting the 'isEof' flag to true.

        False is returned if a error was encountered while reading the Input
        source.
     */
    virtual bool scan( Kit::Type::SSize_T  maxSizeOfFrameBuffer,
                       uint8_t*            frameBuffer,
                       Kit::Type::SSize_T& frameSize,
                       bool&               isEof ) noexcept = 0;

public:
    /** This method allows 'out-of-band' reading of the input source.  If the
        scanner is in-a-frame the method return false and does nothing.

        CAUTION: Most client/consumers of the decoder should never use this
                 method.  This method only has meaning/usefulness when the
                 application KNOWS when the input source is NOT in a frame AND
                 that there is 'non-framed' data that can be consumed.

        Attempts to read the specified number of bytes from the stream into the
        supplied buffer.  The caller is responsible for ensuring that the buffer
        is large enough to hold 'numBytes'. The actual number of bytes read is
        returned via 'bytesRead'.

        Returns true if successful, or false if End-of-Stream was encountered or
        if the input source is in a frame.
     */
    virtual bool oobRead( uint8_t*            dstBuffer,
                          Kit::Type::SSize_T  numBytes,
                          Kit::Type::SSize_T& bytesRead ) noexcept = 0;

public:
    /// Virtual Destructor
    virtual ~IDecoder() = default;
};


}  // end namespaces
}
#endif  // end header latch
