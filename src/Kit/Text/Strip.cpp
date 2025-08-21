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
namespace Text {



////////////////
const char* Strip::space( const char *s )
{
    if ( s == nullptr )
    {
        return nullptr;
    }

    while ( *s && isspace( (int) *s ) )
    {
        s++;
    }
    return s;
}


const char* Strip::notSpace( const char *s )
{
    if ( s == nullptr )
    {
        return nullptr;
    }

    while ( *s && !( isspace( (int) *s ) ) )
    {
        s++;
    }
    return s;
}


const char* Strip::trailingSpace( const char *s )
{
    if ( s == nullptr )
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


void Strip::removeTrailingSpace( char* s )
{
    if ( s == nullptr )
    {
        return;
    }

    char* ptr = (char*) trailingSpace( s );
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
const char* Strip::chars( const char *s, const char* charsSet )
{
    if ( s == nullptr )
    {
        return nullptr;
    }

    if ( charsSet == nullptr )
    {
        return s;
    }

    while ( *s && isCharInString( charsSet, *s ) )
    {
        s++;
    }
    return s;
}


const char* Strip::notChars( const char* s, const char* charsSet )
{
    if ( s == nullptr )
    {
        return nullptr;
    }

    if ( charsSet == nullptr )
    {
        return s;
    }

    while ( *s && !( isCharInString( charsSet, *s ) ) )
    {
        s++;
    }
    return s;
}


const char* Strip::trailingChars( const char *s, const char* charsSet )
{
    if ( s == nullptr )
    {
        return nullptr;
    }

    if ( charsSet == nullptr )
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


void Strip::removeTrailingChars( char* s, const char* charsSet )
{
    if ( s == nullptr || charsSet == nullptr )
    {
        return;
    }

    char* ptr = (char*) trailingChars( (const char*) s, charsSet );
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