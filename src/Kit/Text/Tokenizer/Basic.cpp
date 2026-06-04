/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Basic.h"
#include "Kit/Text/Strip.h"


//------------------------------------------------------------------------------
namespace Kit {
namespace Text {
namespace Tokenizer {


///////////////////////////////
const char* Basic::next() noexcept
{
    // Trap error: null pointer for input string
    if ( !m_base )
    {
        return nullptr;
    }

    // Find the start of the next token (and validate the returned pointer)
    const char* startptr = Strip::chars( m_ptr, m_delimiters );
    if ( startptr == nullptr )
    {
        return nullptr;
    }

    // Have not yet reach end-of-string
    if ( *startptr != '\0' )
    {
        // Get next delimiter (and validate the returned pointer)
        m_ptr = const_cast<char*>( Strip::notChars( startptr, m_delimiters ) );
        if ( m_ptr == nullptr )
        {
            return nullptr;
        }

        // Replace the delimiter with null terminator (when not already at the end of the string)
        if ( *m_ptr != '\0' )
        {
            *m_ptr++ = '\0';
        }

        m_count++;
        return startptr;
    }

    return nullptr;
}


///////////////////////////////
const char* Basic::getToken( unsigned n ) const noexcept
{
    // Trap out-of-bounds index
    if ( n >= m_count || !m_base )
    {
        return nullptr;
    }

    // Traverse string for the Nth token
    const char* token = Strip::chars( m_base, m_delimiters ); // Note: will not return nullptr because m_base has already been verified to not be nullptr
    for ( unsigned i=0; i < n; i++ )
    {
        while ( *token != '\0' )
        {
            token++;
        }
        token = Strip::chars( ++token, m_delimiters );
    }

    return token;
}

} // end namespace
}
}
//------------------------------------------------------------------------------