#ifndef KIT_IO_LINE_WRITER_H_
#define KIT_IO_LINE_WRITER_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Io/ILineWriter.h"
#include "Kit/Io/IOutput.h"


///
namespace Kit {
///
namespace Io {


/** This concrete class implements a Line Writer stream using a previously
    opened IOutput stream.

    NOTE: The LineWriter class does NOT provide any multi-thread mechanisms
          and/or protections.  In addition, using an instance of the
          AtomicOutputApi interface as the underlying IOutput stream will NOT
          make output of the println() calls atomic! The intended way to
          have an 'Atomic' LineWriter is to use the AtomicOutput callback
          mechanism (i.e. requestOutputs()) and within the callback create
          the desired LineWriter using the supplied IOutput stream.
 */
class LineWriter : public ILineWriter
{
public:
    /** Constructor.
     */
    LineWriter( IOutput& stream, const char* newline = Kit::Io::newline );

public:
    /// See ILineWriter
    bool print( const char* srcstring ) noexcept override;

    /// See ILineWriter
    bool println( const char* srcstring ) noexcept override;

    /// See ILineWriter
    bool println() noexcept override;

    /// See ILineWriter
    bool print( const char* srcstring, int numbytes ) noexcept override;

    /// See ILineWriter
    bool println( const char* srcstring, int numbytes ) noexcept override;

    /// See ILineWriter
    KIT_SYSTEM_PRINTF_CHECKER( 3, 4 )
    bool print( Kit::Text::IString& formatBuffer, const char* format, ... ) noexcept override;

    /// See ILineWriter
    KIT_SYSTEM_PRINTF_CHECKER( 3, 4 )
    bool println( Kit::Text::IString& formatBuffer, const char* format, ... ) noexcept override;

    /// See ILineWriter
    bool vprint( Kit::Text::IString& formatBuffer, const char* format, va_list ap ) noexcept override;

    /// See ILineWriter
    bool vprintln( Kit::Text::IString& formatBuffer, const char* format, va_list ap ) noexcept override;

    /// See ILineWriter
    void flush() noexcept override;

    /// See ILineWriter
    void close() noexcept override;

protected:
    /// Data source
    IOutput& m_stream;

    /// Newline
    const char* m_newline;
};

}  // end namespaces
}
#endif  // end header latch
