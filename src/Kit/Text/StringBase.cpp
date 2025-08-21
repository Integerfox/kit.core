/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "StringBase.h"
#include "Strip.h"
#include "strapi.h"
#include <string.h>
#include <stdio.h>

//------------------------------------------------------------------------------
namespace Kit {
namespace Text {


///////////////////////////////
StringBase::StringBase( char* internalString ) noexcept
    : m_strPtr( internalString )
    , m_truncated( false )
{
}

///////////////////////////////
void StringBase::clear() noexcept
{
    m_strPtr[0] = '\0';
    m_truncated = false;
}

StringBase::operator char() const noexcept
{
    return m_strPtr[0];
}

int StringBase::length() const noexcept
{
    return strlen( m_strPtr );
}

bool StringBase::isEmpty() const noexcept
{
    return *( m_strPtr ) == '\0';
}

bool StringBase::truncated() const noexcept
{
    return m_truncated;
}

bool StringBase::operator==( const char* string ) const noexcept
{
    return string ? strcmp( m_strPtr, string ) == 0 : false;
}

bool StringBase::operator==( const IString& string ) const noexcept
{
    return strcmp( m_strPtr, string.getString() ) == 0;
}

bool StringBase::operator!=( const char* string ) const noexcept
{
    return string ? strcmp( m_strPtr, string ) != 0 : true;
}

bool StringBase::operator!=( const IString& string ) const noexcept
{
    return strcmp( m_strPtr, string.getString() ) != 0;
}

int StringBase::compare( const char* string ) const noexcept
{
    return string ? strcmp( m_strPtr, string ) : -1;
}

int StringBase::compareIgnoreCase( const char* string ) const noexcept
{
    return string ? strcasecmp( m_strPtr, string ) : -1;
}


bool StringBase::isEqualIgnoreCase( const char* string ) const noexcept
{
    return string ? strcasecmp( m_strPtr, string ) == 0 : false;
}

bool StringBase::isEqualSubstring( int startpos, int endpos, const char* string ) const noexcept
{
    if ( !string )
    {
        return false;
    }

    int len = strlen( m_strPtr );
    if ( startpos < 0 )
    {
        startpos = 0;
    }
    if ( startpos >= len )
    {
        startpos = len - 1;
    }
    if ( endpos >= len )
    {
        endpos = len - 1;
    }
    if ( endpos < startpos )
    {
        endpos = startpos;
    }
    return strncmp( m_strPtr + startpos, string, endpos - startpos + 1 ) == 0;
}

bool StringBase::isEqualSubstringIgnoreCase( int startpos, int endpos, const char* string ) const noexcept
{
    if ( !string )
    {
        return false;
    }

    int len = strlen( m_strPtr );
    if ( startpos < 0 )
    {
        startpos = 0;
    }
    if ( startpos >= len )
    {
        startpos = len - 1;
    }
    if ( endpos >= len )
    {
        endpos = len - 1;
    }
    if ( endpos < startpos )
    {
        endpos = startpos;
    }

    return strncasecmp( m_strPtr + startpos, string, endpos - startpos + 1 ) == 0;
}

bool StringBase::operator==( const char c ) const noexcept
{
    return c == m_strPtr[0];
}

bool StringBase::startsWith( const char* string ) const noexcept
{
    return StringBase::startsWith( string, 0 );
}

bool StringBase::startsWith( const char* string, int startOffset ) const noexcept
{
    if ( !string )
    {
        return false;
    }

    int len  = strlen( m_strPtr );
    int len2 = strlen( string );
    if ( startOffset < 0 || ( startOffset + len2 ) > len || len2 == 0 )
    {
        return false;
    }

    return strncmp( m_strPtr + startOffset, string, len2 ) == 0;
}

bool StringBase::endsWith( const char* string ) const noexcept
{
    if ( !string )
    {
        return false;
    }

    int len  = strlen( m_strPtr );
    int len2 = strlen( string );
    if ( len2 > len || len2 == 0 )
    {
        return false;
    }

    return strncmp( m_strPtr + len - len2, string, len2 ) == 0;
}


void StringBase::format( const char* format, ... ) noexcept
{
    if ( !format )
    {
        return;
    }

    va_list ap;
    va_start( ap, format );
    vformat( format, ap );
    va_end( ap );
}

void StringBase::formatAppend( const char* format, ... ) noexcept
{
    if ( !format )
    {
        return;
    }

    va_list ap;
    va_start( ap, format );
    vformatAppend( format, ap );
    va_end( ap );
}

void StringBase::formatOpt( bool appendFlag, const char* format, ... ) noexcept
{
    if ( !format )
    {
        return;
    }

    va_list ap;
    va_start( ap, format );
    if ( appendFlag )
    {
        vformatAppend( format, ap );
    }
    else
    {
        vformat( format, ap );
    }

    va_end( ap );
}

void StringBase::vformat( const char* format, va_list ap ) noexcept
{
    if ( !format )
    {
        return;
    }

    int flen = vsnprintf( m_strPtr, maxLength() + 1, format, ap );
    validateSizeAfterFormat( maxLength(), flen, maxLength() );
}

void StringBase::vformatAppend( const char* format, va_list ap ) noexcept
{
    if ( !format )
    {
        return;
    }

    int   len   = strlen( m_strPtr );
    int   avail = maxLength() - len;
    char* ptr   = m_strPtr + len;
    int   flen  = vsnprintf( ptr, avail + 1, format, ap );
    validateSizeAfterFormat( avail, flen, maxLength() );
}

void StringBase::vformatOpt( bool appendFlag, const char* format, va_list ap ) noexcept
{
    if ( !format )
    {
        return;
    }

    if ( appendFlag )
    {
        vformatAppend( format, ap );
    }
    else
    {
        vformat( format, ap );
    }
}


const char*
StringBase::toUpper() noexcept
{
    strupr( m_strPtr );
    return m_strPtr;
}

const char*
StringBase::toLower() noexcept
{
    strlwr( m_strPtr );
    return m_strPtr;
}

void StringBase::removeLeadingSpaces() noexcept
{
    m_truncated   = false;
    const char* p = Strip::space( m_strPtr );

    // do nothing if there is no leading spaces
    if ( p == m_strPtr )
    {
        return;
    }

    // else re-adjust the string to remove the spaces
    size_t i = 0;
    for ( ; i < strlen( p ); i++ )
    {
        m_strPtr[i] = p[i];
    }

    m_strPtr[i] = '\0';
}

void StringBase::removeTrailingSpaces() noexcept
{
    m_truncated = false;
    Strip::removeTrailingSpace( m_strPtr );
}

void StringBase::removeLeadingChars( const char* charsSet ) noexcept
{
    m_truncated   = false;
    const char* p = Strip::chars( m_strPtr, charsSet );

    // do nothing if there is no leading chars
    if ( p == m_strPtr )
    {
        return;
    }

    // else re-adjust the string to remove the chars
    size_t i = 0;
    for ( ; i < strlen( p ); i++ )
    {
        m_strPtr[i] = p[i];
    }

    m_strPtr[i] = '\0';
}

void StringBase::removeTrailingChars( const char* charsSet ) noexcept
{
    m_truncated = false;
    Strip::removeTrailingChars( m_strPtr, charsSet );
}

int StringBase::indexOf( char c ) const noexcept
{
    return StringBase::indexOf( c, 0 );
}

int StringBase::indexOf( char c, int startpos ) const noexcept
{
    int len = strlen( m_strPtr );
    if ( startpos < 0 || startpos >= len )
    {
        return -1;
    }

    char* ptr = strchr( m_strPtr + startpos, c );
    if ( ptr )
    {
        return ptr - m_strPtr;
    }

    return -1;
}

int StringBase::indexOf( const char* str ) const noexcept
{
    return StringBase::indexOf( str, 0 );
}

int StringBase::indexOf( const char* str, int startpos ) const noexcept
{
    int len = strlen( m_strPtr );
    if ( startpos < 0 || startpos >= len || !str )
    {
        return -1;
    }

    char* ptr = strstr( m_strPtr + startpos, str );
    if ( ptr )
    {
        return ptr - m_strPtr;
    }

    return -1;
}
int StringBase::lastIndexOf( char c ) const noexcept
{
    char* ptr = strrchr( m_strPtr, c );
    if ( ptr )
    {
        return ptr - m_strPtr;
    }

    return -1;
}

int StringBase::count( char c ) const noexcept
{
    int   count = 0;
    char* ptr   = m_strPtr;
    while ( ( ptr = strchr( ptr, c ) ) )
    {
        count++;
        ptr++;
    }

    return count;
}

void StringBase::cut( int startpos, int endpos ) noexcept
{
    m_truncated = false;
    int len     = strlen( m_strPtr );
    if ( startpos < 0 || startpos > endpos || startpos >= len || endpos >= len )
    {
        return;
    }

    if ( endpos + 1 >= len )
    {
        trimRight( len - startpos );
        return;
    }
    int movelen = len - ( endpos + 1 );
    memmove( m_strPtr + startpos, m_strPtr + endpos + 1, movelen );
    m_strPtr[startpos + movelen] = '\0';
}

void StringBase::trimRight( int n ) noexcept
{
    m_truncated = false;
    int len     = strlen( m_strPtr );
    int index   = len - n;
    if ( index < 0 || n < 0 )
    {
        return;  // Do nothing -->error case: 'n' is greater than the length of the string OR 'n' is negative
    }
    else
    {
        m_strPtr[index] = '\0';
    }
}

void StringBase::setChar( int atPosition, char newchar ) noexcept
{
    m_truncated = false;
    int len     = strlen( m_strPtr );
    if ( atPosition < 0 || atPosition >= len )
    {
        return;
    }

    m_strPtr[atPosition] = newchar;
}

int StringBase::replace( char targetChar, char newChar ) noexcept
{
    int replaceCount = 0;
    if ( targetChar != newChar )
    {
        char* ptr = m_strPtr;
        while ( *ptr )
        {
            if ( *ptr == targetChar )
            {
                *ptr = newChar;
                replaceCount++;
            }
            ptr++;
        }
    }

    m_truncated = false;
    return replaceCount;
}

const char* StringBase::getString() const noexcept
{
    return m_strPtr;
}

char* StringBase::getBuffer( int& maxAllowedLength ) noexcept
{
    m_truncated      = false;
    maxAllowedLength = maxLength();
    return m_strPtr;
}


////////////////////////////////////
int StringBase::compareKey( const Key& key ) const noexcept
{
    unsigned    otherLen = 0;
    const char* otherPtr = (const char*)key.getRawKey( &otherLen );
    return Kit::Container::KeyStringBuffer::compare( m_strPtr, strlen( m_strPtr ), otherPtr, otherLen );
}

const void* StringBase::getRawKey( unsigned* returnRawKeyLenPtr ) const noexcept
{
    if ( returnRawKeyLenPtr )
    {
        *returnRawKeyLenPtr = strlen( m_strPtr );
    }

    return m_strPtr;
}


////////////////////////////////////
void StringBase::validateSizeAfterFormat( int availableLen, int formattedLen, int maxlen ) noexcept
{
    if ( formattedLen <= availableLen && formattedLen >= 0 )
    {
        m_truncated = false;
    }
    else
    {
        m_truncated      = true;
        m_strPtr[maxlen] = '\0';  // ENSURE the string is terminated
    }
}

////////////////////////////////////
IString& StringBase::operator=( const IString& string ) noexcept
{
    copyIn( string, string.length() );
    return *this;
}

IString& StringBase::operator=( const StringBase& string ) noexcept
{
    copyIn( string, string.length() );
    return *this;
}

IString& StringBase::operator=( const char* string ) noexcept
{
    copyIn( string, string ? strlen( string ) : 0 );
    return *this;
}

IString& StringBase::operator=( char c ) noexcept
{
    copyIn( &c, 1 );
    return *this;
}

IString& StringBase::operator+=( const IString& string ) noexcept
{
    appendTo( string, string.length() );
    return *this;
}

IString& StringBase::operator+=( const char* string ) noexcept
{
    appendTo( string, string ? strlen( string ) : 0 );
    return *this;
}

IString& StringBase::operator+=( char c ) noexcept
{
    appendTo( &c, 1 );
    return *this;
}


}  // end namespaces
}
//------------------------------------------------------------------------------