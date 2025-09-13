#ifndef KIT_IO_ILINE_READER_H_
#define KIT_IO_ILINE_READER_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Text/IString.h"


///
namespace Kit {
///
namespace Io {


/** This abstract class defines a interface for a stream line reader.  A
    line reader allows the client to read one line at time from the stream.
    A line is consider a stream of ASCII character terminated by '\n' a.k.a
    newline. The actual newline character(s) are handled by the interface
    and the client is not aware of the actual newline implementation.
 */
class ILineReader 
{
public:
    /** Reads a single line from the stream.  If the line is greater than the
        will fit in 'destString', the line content is truncated.  The next
        subsequent readln() reads the next line (NOT the truncated characters!).

        The newline character(s) are NOT copied into 'destString' but is
        replaced with '\0'.  Returns true if successful, or false if End-of-Stream
        was encountered.

        NOTE: This call will not return until a newline character is encounter!
     */
    virtual bool readln( Kit::Text::IString& destString ) noexcept = 0;

    /** Returns true if there is data available to be read from the stream.

        NOTE: The implementation of this method is VERY PLATFORM dependent! If
              your code uses it - it may not be portable to all platforms.
              If a platform does not/can not support this method it is
              guaranteed to return 'true'
     */
    virtual bool available() noexcept = 0;

    /** Closes the reader and the underlying input stream.
     */
    virtual void close() noexcept = 0;


public:
    /// Lets the make the destructor virtual
    virtual ~ILineReader() noexcept = default;
};

}  // end namespaces
}
#endif  // end header latch
