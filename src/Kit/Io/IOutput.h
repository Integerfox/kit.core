#ifndef KIT_IO_IOUTPUT_H_
#define KIT_IO_IOUTPUT_H_
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
#include "Kit/System/printfchecker.h"
#include <stdarg.h>


///
namespace Kit {
///
namespace Io {

/** This partially abstract class defines a interface for operating on an
    output stream (example of a stream is 'stdout' or a socket connection).

    All Write calls on the stream are 'blocking' - i.e. the calls do not
    return until one or more bytes have been written to the stream.

    Note: There is really only just one write() method (the one the returns
          'bytesWritten').  All of the other write() are convenience methods
          and as such a default implementation is provided for these methods.
 */
class IOutput : virtual public IClose, virtual public IEos
{
public:
    /** Writes a single byte to the stream.  Returns true if successful,
        or false if End-of-Stream was encountered.
     */
    virtual bool write( char c ) noexcept;

    /** Writes a string to the stream.  The string's trailing '\0' is stripped
        off and not outputted to the stream. Returns true if successful, or false
        if End-of-Stream was encountered.  The method does not return until all
        characters in the string have been written to the Output stream.
     */
    virtual bool write( const char* string ) noexcept;

    /** Writes a string to the stream.  The string's trailing '\0' is stripped
        off and not outputted to the stream. Returns true if successful, or false
        if End-of-Stream was encountered. The method does not return until all
        characters in the string have been written to the Output stream.
     */
    virtual bool write( const Kit::Text::IString& string ) noexcept;

    /** Formatted write to the stream.  The formatting syntax/semantics is the
        same as printf(). The string's trailing '\0' is stripped off and not
        outputted to the stream.  The client is required to provide storage for
        a temporary buffer used to format the outgoing data.  The contents of
        'formatBuffer' will match what was written to the stream. Returns true
        if successful, or false if End-of-Stream was encountered. The method
        does not return until the all of the characters in the 'formatBuffer'
        (after the contents have been 'formatted') is written to the Output
        stream.
     */
    KIT_SYSTEM_PRINTF_CHECKER(3, 4)
    virtual bool write( Kit::Text::IString& formatBuffer, const char* format, ... ) noexcept;

    /** Same as write( String& formatBuffer, const char* format,...), except that
        it is called with a va_list instead of a variable number of arguments.
     */
    virtual bool vwrite( Kit::Text::IString& formatBuffer, const char* format, va_list ap ) noexcept;

    /** Writes the content of the buffer to the stream. Returns true if
        successful, or false if End-of-Stream as encountered. The method does
        not return until 'numBytes' have been written to the Output stream.
     */
    virtual bool write( const void* buffer, ByteCount_T numBytes ) noexcept;

    /** Writes the content of the buffer to the stream. At most 'maxBytes'
        will be outputted.  The actual number of bytes written is returned
        via 'bytesWritten'. Returns true if successful, or false if End-of-Stream
        was encountered.
     */
    virtual bool write( const void* buffer, ByteCount_T maxBytes, ByteCount_T& bytesWritten ) noexcept = 0;

    /** Forces all buffered data (if any) to be written to the stream media.
        The actual behavior of this method for how/when the data is flushed -
        is stream-media and platform dependent.
     */
    virtual void flush() noexcept = 0;


public:
    /// Lets the make the destructor virtual
    virtual ~IOutput() noexcept = default;

};

}       // end namespaces
}
#endif  // end header latch
