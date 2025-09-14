#ifndef Cpl_Io_LineReader_h_
#define Cpl_Io_LineReader_h_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Io/ILineReader.h"
#include "Kit/Io/IInput.h"


///
namespace Kit {
///
namespace Io {


/** This concrete class implements a Line Reader stream using a previously
    opened Input stream.
 */
class LineReader : public ILineReader
{
public:
    /** Constructor.
     */
    LineReader( IInput& stream, const char* newline = Kit::Io::newline );


public:
    /// See ILineReader
    bool readln( Kit::Text::IString& destString ) noexcept override;

    /// See ILineReader
    bool available() noexcept override;

    /// See ILineReader
    void close() noexcept override;

protected:
    /// Data source
    IInput& m_stream;

    /// Newline
    const char* m_newline;
};

}  // end namespaces
}
#endif  // end header latch
