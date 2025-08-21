/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Format.h"

//------------------------------------------------------------------------------
namespace Kit {
namespace Text {

////////////////////////////
bool Format::string( const void* buffer,
                     int         len,
                     IString&    destString,
                     bool        appendToString ) noexcept
{
    if ( !buffer || len <= 0 )
    {
        return false;  // Do nothing!
    }

    // Clear Destination
    if ( !appendToString )
    {
        destString.clear();
    }

    // Convert the data
    const char* ptr = static_cast<const char*>( buffer );
    char        c;
    int         i;
    for ( i = 0; i < len; i++, ptr++ )
    {
        c           = *ptr < ' ' || *ptr > '~' ? '.' : *ptr;
        destString += c;
    }

    return !destString.truncated();
}

bool Format::asciiHex( const void* binaryData,
                       int         len,
                       IString&    destString,
                       bool        upperCase,
                       bool        appendToString,
                       char        separator ) noexcept
{
    if ( !binaryData || len <= 0 )
    {
        return false;  // Do nothing!
    }

    // Select look-up table based on a requested 'case'
    static const char lowerCaseTable[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
    static const char upperCaseTable[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
    const char*       tableP           = upperCase ? upperCaseTable : lowerCaseTable;

    // Clear Destination
    if ( !appendToString )
    {
        destString.clear();
    }

    // Convert the data
    const char* ptr = static_cast<const char*>( binaryData );
    char        c;
    int         i;
    for ( i = 0; i < len; i++, ptr++ )
    {
        c           = *ptr;
        destString += tableP[( c >> 4 ) & 0x0F];
        destString += tableP[c & 0x0F];
        if ( separator != '\0' && ( i + 1 ) < len )
        {
            destString += separator;
        }
    }

    return !destString.truncated();
}

bool Format::asciiBinary( const void* binaryData,
                          int         len,
                          IString&    destString,
                          bool        appendToString,
                          bool        reverse ) noexcept
{
    static const char* bitRep[16] = { "0000", "0001", "0010", "0011", "0100", "0101", "0110", "0111", "1000", "1001", "1010", "1011", "1100", "1101", "1110", "1111" };

    if ( !binaryData || len <= 0 )
    {
        return false;  // Do nothing!
    }

    // Clear Destination
    if ( !appendToString )
    {
        destString.clear();
    }
    const uint8_t* binPtr    = static_cast<const uint8_t*>( binaryData );
    const uint8_t* ptr       = reverse ? binPtr + len - 1 : binPtr;
    int            direction = reverse ? -1 : 1;
    while ( len-- )
    {
        destString += bitRep[*ptr >> 4];
        destString += bitRep[*ptr & 0x0F];
        ptr        += direction;
    }

    return !destString.truncated();
}

bool Format::viewer( const void* binaryData,
                     int         len,
                     IString&    destString,
                     int         bytesPerLine,
                     const char* separator,
                     bool        upperCase,
                     bool        appendToString ) noexcept
{
    if ( !Format::asciiHex( binaryData, len, destString, upperCase, appendToString ) )
    {
        return false;
    }

    // Fill in 'missing' bytes
    for ( int i = len; i < bytesPerLine; i++ )
    {
        destString += "  ";
    }

    // Add separator
    destString += separator;
    if ( destString.truncated() )
    {
        return false;
    }

    return Format::string( binaryData, len, destString, true );
}

bool Format::formatMsec( IString&  buffer,
                         long long elapsedMsec,
                         bool      encodeMsec,
                         bool      encodeDay,
                         bool      appendToString ) noexcept
{
    long long t, dd, hh, mm, ss;
    lldiv_t   result;

    // Separate seconds and milliseconds
    result                = lldiv( elapsedMsec, 1000LL );
    long long msec        = result.rem;
    long long elapsedSecs = result.quot;

    // Get hours
    result = lldiv( elapsedSecs, 60L * 60LL );
    hh     = result.quot;
    t      = result.rem;

    // Get Minutes & Seconds
    result = lldiv( t, 60L );
    mm     = result.quot;
    ss     = result.rem;

    // Format string
    if ( encodeDay )
    {
        result = lldiv( hh, 24LL );
        dd     = result.quot;
        hh     = result.rem;

        if ( encodeMsec )
        {
            buffer.formatOpt( appendToString,
                              "%02u %02u:%02u:%02u.%03u",
                              static_cast<unsigned>( dd ),
                              static_cast<unsigned>( hh ),
                              static_cast<unsigned>( mm ),
                              static_cast<unsigned>( ss ),
                              static_cast<unsigned>( msec ) );
        }
        else
        {
            buffer.formatOpt( appendToString,
                              "%02u %02u:%02u:%02u",
                              static_cast<unsigned>( dd ),
                              static_cast<unsigned>( hh ),
                              static_cast<unsigned>( mm ),
                              static_cast<unsigned>( ss ) );
        }
    }
    else
    {
        if ( encodeMsec )
        {
            buffer.formatOpt( appendToString,
                              "%02u:%02u:%02u.%03u",
                              static_cast<unsigned>( hh ),
                              static_cast<unsigned>( mm ),
                              static_cast<unsigned>( ss ),
                              static_cast<unsigned>( msec ) );
        }
        else
        {
            buffer.formatOpt( appendToString,
                              "%02u:%02u:%02u",
                              static_cast<unsigned>( hh ),
                              static_cast<unsigned>( mm ),
                              static_cast<unsigned>( ss ) );
        }
    }

    return !buffer.truncated();
}


}  // end namespaces
}
//------------------------------------------------------------------------------