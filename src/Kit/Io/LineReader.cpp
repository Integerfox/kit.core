/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "LineReader.h"
#include "Kit/Text/FString.h"
#include "Kit/System/Assert.h"
#include <string.h>

//------------------------------------------------------------------------------
namespace Kit {
namespace Io {

static bool discardRemainingLine_( IInput& fd, const char* newline, Kit::Text::IString& destString, char lastChar, char overflowChar );


///////////////////////////////
LineReader::LineReader( IInput& stream, const char* newline )
    : m_stream( stream )
    , m_newline( newline )
{
    KIT_SYSTEM_ASSERT( newline != nullptr );
}

///////////////////////////////
bool LineReader::available() noexcept
{
    return m_stream.available();
}


void LineReader::close() noexcept
{
    m_stream.close();
}


// NOT the best/most-efficient algo - but it works for now
bool LineReader::readln( Kit::Text::IString& destString ) noexcept
{
    bool io;
    char nextChar;
    char prevChar = ' ';

    // Make sure the destination string is empty
    destString.clear();

    // Loop till newline is found
    while ( ( io = m_stream.read( nextChar ) ) )
    {
        // Read one character at a time
        destString += nextChar;

        // Trap the case: the current line exceeds the size of my destination buffer
        if ( destString.truncated() )
        {
            io = discardRemainingLine_( m_stream, m_newline, destString, prevChar, nextChar );
            break;  // Exit once newline was found
        }

        // Match newline
        else if ( destString.endsWith( m_newline ) )
        {
            destString.trimRight( strlen( m_newline ) );
            break;
        }

        // cache the previous/last character read
        prevChar = nextChar;

        // Check for EOS/EOF
        if ( m_stream.isEos() )
        {
            io = false;
            break;
        }
    }

    // Return the underlying stream status
    return io;
}


///////////////////
bool discardRemainingLine_( IInput& fd, const char* newline, Kit::Text::IString& destString, char lastChar, char overflowChar )
{
    bool io = true;

    // CASE: Single character newline
    if ( strlen( newline ) == 1 )
    {
        // Drain the input till newline is found (staring the overflowChar from the 'line read')
        while ( io && overflowChar != *newline )
        {
            io = fd.read( overflowChar );
        }
    }


    // CASE: Newline is two characters
    else
    {
        Kit::Text::FString<2> buffer( lastChar );
        buffer += overflowChar;

        // Trap the case where newline is 2 characters and is the 1st character is in the client's buffer
        if ( buffer.startsWith( newline ) )
        {
            destString.trimRight( 1 );  // Remove the partial newline from the client's buffer
            return true;
        }

        // Search for newline characters
        while ( io && !buffer.startsWith( newline ) )
        {
            char inChar;

            buffer.trimLeft( 1 );
            io      = fd.read( inChar );
            buffer += inChar;
        }
    }

    return io;
}

}  // end namespace
}
//------------------------------------------------------------------------------