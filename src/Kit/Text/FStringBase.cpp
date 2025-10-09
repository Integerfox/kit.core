/*-----------------------------------------------------------------------------
 * This file is part of the Colony.Core Project.  The Colony.Core Project is an
 * open source project with a BSD type of licensing agreement.  See the license
 * agreement (license.txt) in the top/ directory or on the Internet at
 * http://integerfox.com/colony.core/license.txt
 *
 * Copyright (c) 2014-2025  John T. Taylor
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/

#include "FStringBase.h"
#include <string.h>
#include <stdio.h>
#include <algorithm>  // Added for std::min
#include <inttypes.h>

//------------------------------------------------------------------------------
namespace Kit {
namespace Text {

//
char FStringBase::m_noMemory[1] = { '\0' };


///////////////////////////////
FStringBase::FStringBase( const char* string, char* internalString, int maxLen ) noexcept
    : StringBase( internalString ), m_internalMaxlen( maxLen )
{
    // trap the case of a NULL initializer string
    if ( string == nullptr )
    {
        internalString[0] = '\0';
    }
    else
    {
        int inStrLen = (int)strlen( string );
        m_truncated  = inStrLen <= maxLen ? false : true;
        strncpy( internalString, string, maxLen );
        internalString[maxLen] = '\0';
    }
}

FStringBase::FStringBase( char c, char* internalString, int maxLen ) noexcept
    : StringBase( internalString ), m_internalMaxlen( maxLen )
{
    if ( maxLen > 0 )
    {
        internalString[0] = c;
        internalString[1] = '\0';
    }

    // Handle the case of 'memsize' is zero
    else
    {
        internalString[0] = '\0';
    }
}

/// Constructor
FStringBase::FStringBase( int num, char* internalString, int maxLen ) noexcept
    : StringBase( internalString ), m_internalMaxlen( maxLen )
{
    int flen = snprintf( m_strPtr, m_internalMaxlen + 1, "%d", num );
    validateSizeAfterFormat( m_internalMaxlen, flen, m_internalMaxlen );
}


/// Constructor
FStringBase::FStringBase( unsigned num, char* internalString, int maxLen ) noexcept
    : StringBase( internalString ), m_internalMaxlen( maxLen )
{
    int flen = snprintf( m_strPtr, m_internalMaxlen + 1, "%u", num );
    validateSizeAfterFormat( m_internalMaxlen, flen, m_internalMaxlen );
}

/// Constructor
FStringBase::FStringBase( long num, char* internalString, int maxLen ) noexcept
    : StringBase( internalString ), m_internalMaxlen( maxLen )
{
    int flen = snprintf( m_strPtr, m_internalMaxlen + 1, "%ld", num );
    validateSizeAfterFormat( m_internalMaxlen, flen, m_internalMaxlen );
}

/// Constructor
FStringBase::FStringBase( long long num, char* internalString, int maxLen ) noexcept
    : StringBase( internalString ), m_internalMaxlen( maxLen )
{
    int flen = snprintf( m_strPtr, m_internalMaxlen + 1, "%lld", num );
    validateSizeAfterFormat( m_internalMaxlen, flen, m_internalMaxlen );
}


/// Constructor
FStringBase::FStringBase( unsigned long num, char* internalString, int maxLen ) noexcept
    : StringBase( internalString ), m_internalMaxlen( maxLen )
{
    int flen = snprintf( m_strPtr, m_internalMaxlen + 1, "%lu", num );
    validateSizeAfterFormat( m_internalMaxlen, flen, m_internalMaxlen );
}

/// Constructor
FStringBase::FStringBase( unsigned long long num, char* internalString, int maxLen ) noexcept
    : StringBase( internalString ), m_internalMaxlen( maxLen )
{
    int flen = snprintf( m_strPtr, m_internalMaxlen + 1, "%llu", num );
    validateSizeAfterFormat( m_internalMaxlen, flen, m_internalMaxlen );
}


///////////////////////////////
void FStringBase::copyIn( const char* src, int len ) noexcept
{
    // Trap null pointer for 'src' string
    if ( !src )
    {
        m_truncated = true;
        m_strPtr[0] = '\0';
    }
    else
    {
        m_truncated = len <= m_internalMaxlen ? false : true;
        len         = std::min( m_internalMaxlen, len );
        strncpy( m_strPtr, src, len );
        m_strPtr[len] = '\0';
    }
}

void FStringBase::appendTo( const char* string, int len ) noexcept
{
    // Note: Do NOTHING if null string pointer is passed
    if ( string )
    {
        int curlen  = strlen( m_strPtr );
        int avail   = m_internalMaxlen - curlen;
        int copylen = std::min( len, avail );
        m_truncated = copylen == len ? false : true;
        strncat( m_strPtr, string, copylen );
        m_strPtr[curlen + copylen] = '\0';
    }
}


void FStringBase::insertAt( int insertOffset, const char* stringToInsert ) noexcept
{
    // Insure offset is NOT negative
    if ( insertOffset < 0 )
    {
        insertOffset = 0;
    }

    // If insertOffset is past the '\0', then simply append stringToInsert
    int curlen    = strlen( m_strPtr );
    int insertlen = strlen( stringToInsert );
    if ( insertOffset >= curlen )
    {
        appendTo( stringToInsert, insertlen );
        return;
    }

    // Valid insertOffset
    int avail        = m_internalMaxlen - insertOffset;     // Amount left in the buffer starting with from insertOffset
    int copylen      = std::min( insertlen, avail );        // Amount of stringToInsert that can be copied in
    int remainderlen = avail - copylen;                     // Amount of room left after stringToInsert has been copied in
    int shiftlen     = curlen - insertOffset;               // Amount of characters needing to be shifted
    int movelen      = std::min( shiftlen, remainderlen );  // Amount of characters that can be SAFELY shifted
    if ( movelen > 0 )
    {
        memmove( m_strPtr + insertOffset + copylen, m_strPtr + insertOffset, movelen );
        m_strPtr[insertOffset + copylen + movelen] = '\0';
    }
    memmove( m_strPtr + insertOffset, stringToInsert, copylen );
    m_truncated                = movelen != shiftlen || copylen != insertlen ? true : false;
    m_strPtr[m_internalMaxlen] = '\0';
}

int FStringBase::maxLength() const noexcept
{
    return m_internalMaxlen;
}

////////////////////////////
IString& FStringBase::operator=( int num ) noexcept
{
    format( "%d" , num );
    return *this;
}

IString& FStringBase::operator=( unsigned num ) noexcept
{
    format( "%u" , num );
    return *this;
}

IString& FStringBase::operator=( long num ) noexcept
{
    format( "%ld", num );
    return *this;
}

IString& FStringBase::operator=( unsigned long num ) noexcept
{
    format( "%lu", num );
    return *this;
}

IString& FStringBase::operator+=( int num ) noexcept
{
    formatAppend( "%d", num );
    return *this;
}

IString& FStringBase::operator+=( unsigned num ) noexcept
{
    formatAppend( "%u", num );
    return *this;
}

IString& FStringBase::operator+=( long num ) noexcept
{
    formatAppend( "%ld", num );
    return *this;
}

IString& FStringBase::operator+=( unsigned long num ) noexcept
{
    formatAppend( "%lu", num );
    return *this;
}


}  // end namespaces
}
//------------------------------------------------------------------------------