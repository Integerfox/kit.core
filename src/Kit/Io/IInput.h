#ifndef KIT_IO_IINPUT_H_
#define KIT_IO_IINPUT_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Io/Types.h"
#include "Kit/Text/IString.h"
#include "Kit/Io/IClose.h"
#include "Kit/Io/IEos.h"



///
namespace Kit {
///
namespace Io {

/** This partially abstract class defines a interface for operating on an
    input stream (example of a stream is 'stdin' or a socket connection).
    All Read calls on the stream are 'blocking' - i.e. the calls do not
    return until one or more bytes have been read from the stream.

    Note: There is really only just one read() method (the one the returns
          'bytesRead').  All of the other read() are convenience methods
          and as such a default implementation is provided for these methods.

 */
class IInput : virtual public IClose, virtual public IEos
{
public:
    /** Reads a single byte from the stream.  Returns true if successful,
        or false if End-of-Stream was encountered.

        If the method fails, the contents of the 'c' is undefined.
     */
    virtual bool read( char& c ) noexcept;

    /** Reads N bytes into the String's internal buffer.  The number of
        bytes read will be less or equal to the String's max length.  The
        String is guaranteed to be terminated by a '\0'.  The placement
        of the '\0' is determined by the number of bytes read from the
        stream (e.g. buffer[bytesRead] = '\0').   Returns true if successful,
        or false if End-of-Stream was encountered.

        If the method fails, the contents of the 'destString' is undefined.

        NOTE: NO FILTERING of 'non-printable' characters is done!  Therefore
              it is up to the client application to deal with the problem!
     */
    virtual bool read( Kit::Text::IString& destString ) noexcept;

    /** This method is similar to read( Kit::Text::IString& destString ), expect
        that it will NOT return until 'numBytesToRead' have been read.

        If the method fails, the contents of the 'destString' is undefined.
     */
    virtual bool read( Kit::Text::IString& destString, ByteCount_T numBytesToRead ) noexcept;

    /** Attempts to read the specified number of bytes from the stream in the
        supplied buffer.  The actual number of bytes read is returned via
        'bytesRead'. Returns true if successful, or false if End-of-Stream
        was encountered.

        If the method fails, the contents of the 'buffer' is undefined.

        The caller is responsible for providing a buffer that is large
        enough to hold the requested number of bytes.
     */
    virtual bool read( void* buffer, ByteCount_T numBytes, ByteCount_T& bytesRead ) noexcept = 0;

    /** This method is similar to read( void* buffer, int numBytes, int& bytesRead ),
        except that it will NOT return until 'numBytesToRead' have been read.
        
        If the method fails, the contents of the 'buffer' is undefined.
     */
    virtual bool read( void* buffer, ByteCount_T numBytesToRead ) noexcept;

    /** Returns true if there data available to be read from the stream.

        NOTE: The implementation of this method is VERY PLATFORM dependent! If
              your code uses it - it may not be portable to all platforms.
              If a platform does not/can not support this method it is
              guaranteed to return 'true'
     */
    virtual bool available() noexcept = 0;


public:
    /// Lets the make the destructor virtual
    virtual ~IInput() noexcept = default;

};


};      // end namespaces
};
#endif  // end header latch
