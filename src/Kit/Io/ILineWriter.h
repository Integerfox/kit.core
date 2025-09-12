#ifndef KIT_IO_ILINE_WRITER_H_
#define KIT_IO_ILINE_WRITER_H_
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
#include "Kit/Container/ListItem.h"
#include "Kit/System/printfchecker.h"
#include <stdarg.h>


///
namespace Kit {
///
namespace Io {

/** This abstract class defines a interface for a stream line writer.  A
    line writer allows the client to write lines to a stream.  The interface
    also handles the output of the 'new line' character(s).
 */
class ILineWriter : public Kit::Container::ListItem
{
public:
    /** Outputs the specified string to the stream.  No newline character(s)
        are written. Returns true if successful, or false if End-of-Stream was
        encountered.
     */
    virtual bool print( const char* srcstring ) noexcept = 0;

    /** Outputs the specified string to the stream and then appends the
        newline character(s) to the stream. Returns true if successful, or
        false if End-of-Stream was encountered.
     */
    virtual bool println( const char* srcstring ) noexcept = 0;

    /** Outputs the first 'numbytes' bytes of the specified string to the
        stream.  No newline character(s) are written. Returns true if
        successful, or false if End-of-Stream was encountered.
     */
    virtual bool print( const char* srcstring, int numbytes ) noexcept = 0;

    /** Outputs the first 'numbytes' bytes of the specified string to the
        stream and then appends the newline character(s) to the stream.
        Returns true if successful, or false if End-of-Stream was
        encountered.
     */
    virtual bool println( const char* srcstring, int numbytes ) noexcept = 0;


    /** Outputs the newline character(s) to the stream.
     */
    virtual bool println() noexcept = 0;

    /** Formatted output to the stream.  The formatting syntax/semantics is the
        same as printf(). The number of characters actually outputted to
        stream is limited by the size of 'formatBuffer'.  No newline character(s)
        are written to the stream.  Returns true if successful, or false if
        End-of-Stream was encountered.
     */
    KIT_SYSTEM_PRINTF_CHECKER( 3, 4 )
    virtual bool print( Kit::Text::IString& formatBuffer, const char* format, ... ) noexcept = 0;

    /** Same as above, except newline character(s) are appended to the end of
        the formatted output.
     */
    KIT_SYSTEM_PRINTF_CHECKER( 3, 4 )
    virtual bool println( Kit::Text::IString& formatBuffer, const char* format, ... ) noexcept = 0;

    /** Same as print( String& formatBuffer, const char* format,...), except that
        it is called with a va_list instead of a variable number of arguments.
     */
    virtual bool vprint( Kit::Text::IString& formatBuffer, const char* format, va_list ap ) noexcept = 0;

    /** Same as println( String& formatBuffer, const char* format,...), except that
        it is called with a va_list instead of a variable number of arguments.
     */
    virtual bool vprintln( Kit::Text::IString& formatBuffer, const char* format, va_list ap ) noexcept = 0;

    /// Forces all buffered data (if any) to be written to the stream media.
    virtual void flush() noexcept = 0;

    /// Closes the writer and the underlying output stream.
    virtual void close() noexcept = 0;


public:
    /// Lets the make the destructor virtual
    virtual ~ILineWriter() noexcept = default;
};


}  // end namespaces
}
#endif  // end header latch
