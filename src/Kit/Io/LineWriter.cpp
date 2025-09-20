/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "LineWriter.h"
#include "Kit/System/Assert.h"

//------------------------------------------------------------------------------
namespace Kit {
namespace Io {

///////////////////////////////
LineWriter::LineWriter( IOutput& stream, const char* newline )
    : m_stream( stream )
    , m_newline( newline )
{
    KIT_SYSTEM_ASSERT( newline != nullptr );
}


///////////////////////////////
bool LineWriter::print( const char* srcstring ) noexcept
{
    return m_stream.write( srcstring );
}


bool LineWriter::println( const char* srcstring ) noexcept
{
    bool io = m_stream.write( srcstring );
    return io && m_stream.write( m_newline );
}

bool LineWriter::println() noexcept
{
    return m_stream.write( m_newline );
}


bool LineWriter::print( const char* srcstring, int numbytes ) noexcept
{
    return m_stream.write( srcstring, numbytes );
}


bool LineWriter::println( const char* srcstring, int numbytes ) noexcept
{
    bool io = m_stream.write( srcstring, numbytes );
    return io && m_stream.write( m_newline );
}


bool LineWriter::print( Kit::Text::IString& formatBuffer, const char* format, ... ) noexcept
{
    KIT_SYSTEM_ASSERT( format != nullptr );

    va_list ap;
    va_start( ap, format );
    bool io = m_stream.vwrite( formatBuffer, format, ap );
    va_end( ap );
    return io;
}


bool LineWriter::println( Kit::Text::IString& formatBuffer, const char* format, ... ) noexcept
{
    KIT_SYSTEM_ASSERT( format != nullptr );

    va_list ap;
    va_start( ap, format );
    bool io = m_stream.vwrite( formatBuffer, format, ap );
    va_end( ap );
    return io && m_stream.write( m_newline );
}


bool LineWriter::vprint( Kit::Text::IString& formatBuffer, const char* format, va_list ap ) noexcept
{
    return m_stream.vwrite( formatBuffer, format, ap );
}


bool LineWriter::vprintln( Kit::Text::IString& formatBuffer, const char* format, va_list ap ) noexcept
{
    bool io = m_stream.vwrite( formatBuffer, format, ap );
    return io && m_stream.write( m_newline );
}


void LineWriter::flush() noexcept
{
    m_stream.flush();
}


void LineWriter::close() noexcept
{
    m_stream.close();
}

}  // end namespace
}
//------------------------------------------------------------------------------