/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Strip.h"
#include <ctype.h>
#include <stdlib.h>

//------------------------------------------------------------------------------
namespace Kit {
namespace System {



////////////////
const char* space( const char *s )
{
    if ( !s )
    {
        return nullptr;
    }

    while ( *s && isspace( (int) *s ) )
    {
        s++;
    }
    return s;
}


const char* notSpace( const char *s )
{
    if ( !s )
    {
        return nullptr;
    }

    while ( *s && !( isspace( (int) *s ) ) )
    {
        s++;
    }
    return s;
}


const char* trailingSpace( const char *s )
{
    if ( !s )
    {
        return nullptr;
    }

    // do nothing if empty string
    size_t len = strlen( s );
    if ( !len )
    {
        return s;
    }

    // Walk the string right to left
    const char* p = s + len - 1;
    while ( p != s && isspace( (int) *p ) )
    {
        p--;
    }
    return p;
}


void removeTrailingSpace( char* s )
{
    if ( !s )
    {
        return;
    }

    char* ptr = (char*) stripTrailingSpace( s );
    if ( isspace( (int) *ptr ) )
    {
        *ptr = '\0';
    }
    else
    {
        *( ptr + 1 ) = '\0';
    }
}


//////////////////////	
const char* stripChars( const char *s, const char* charsSet )
{
    if ( !s )
    {
        return 0;
    }

    if ( !charsSet )
    {
        return s;
    }

    while ( *s && isCharInString( charsSet, *s ) )
    {
        s++;
    }
    return s;
}


const char* stripNotChars( const char* s, const char* charsSet )
{
    if ( !s )
    {
        return 0;
    }

    if ( !charsSet )
    {
        return s;
    }

    while ( *s && !( isCharInString( charsSet, *s ) ) )
    {
        s++;
    }
    return s;
}


const char* stripTrailingChars( const char *s, const char* charsSet )
{
    if ( !s )
    {
        return 0;
    }

    if ( !charsSet )
    {
        return s;
    }

    // do nothing if empty string
    size_t len = strlen( s );
    if ( !len )
    {
        return s;
    }

    // Walk the string right to left
    const char* p = s + len - 1;
    while ( p != s && isCharInString( charsSet, *p ) )
    {
        p--;
    }
    return p;
}


void removeTrailingChars( char* s, const char* charsSet )
{
    if ( !s || !charsSet )
    {
        return;
    }

    char* ptr = (char*) stripTrailingChars( (const char*) s, charsSet );
    if ( isCharInString( charsSet, *ptr ) )
    {
        *ptr = '\0';        // trap the case of string is ALL spaces
    }
    else
    {
        *( ptr + 1 ) = '\0';
    }
}

} // end namespaces
}
//------------------------------------------------------------------------------