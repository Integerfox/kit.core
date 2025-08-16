/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Parse.h"
#include "Strip.h"
#include "StringTo.h"
#include <string.h>

//------------------------------------------------------------------------------
namespace Kit {
namespace Text {

size_t Parse::asciiHex( void* dstBinary, const char* srcP, size_t dstMaxLen ) noexcept
{
    if ( dstBinary == nullptr || srcP == nullptr || dstMaxLen == 0 )
    {
        return SIZE_MAX;
    }

    uint8_t* dstP = (uint8_t*)dstBinary;
    size_t   len  = strlen( srcP );

    // Length must be even
    if ( ( len & 1 ) == 1 )
    {
        return SIZE_MAX;
    }

    // Do NOT exceed the destination buffer
    if ( len / 2 > dstMaxLen )
    {
        return SIZE_MAX;
    }

    // Convert the string
    if ( !asciiHexUnsafe( srcP, len, dstP ) )
    {
        // Error -->end conversion
        return SIZE_MAX;
    }

    return len / 2;
}

size_t Parse::asciiBinary( void* dstBinary, const char* srcP, size_t dstMaxLen, bool reverse ) noexcept
{
    if ( dstBinary == nullptr || srcP == nullptr || dstMaxLen == 0 )
    {
        return SIZE_MAX;
    }

    // Calculate the 'start' and 'end' of the data
    size_t inputCharLen = strlen( srcP );
    size_t inputBinLen  = ( inputCharLen + 7 ) / 8;
    dstMaxLen           = inputBinLen > dstMaxLen ? dstMaxLen : inputBinLen;
    uint8_t* ptr        = reverse ? ( (uint8_t*)dstBinary ) + dstMaxLen - 1 : (uint8_t*)dstBinary;
    int      direction  = reverse ? -1 : 1;

    // Loop through the string.  Note: for each 'byte' in the string - MSb ordering is assumed
    long convertedBits = 0;
    while ( *srcP != '\0' && dstMaxLen )
    {
        uint8_t mask = 0x80;
        uint8_t data = 0;
        for ( int i = 0; i < 8 && *srcP != '\0'; i++, mask >>= 1, srcP++, convertedBits++ )
        {
            if ( *srcP == '1' )
            {
                data |= mask;
            }
            else if ( *srcP != '0' )
            {
                return SIZE_MAX;
            }
        }

        *ptr  = data;
        ptr  += direction;
        dstMaxLen--;
    }

    return convertedBits;
}


///////////////////
bool Parse::asciiHexUnsafe( const char* inString, size_t numCharToScan, uint8_t* outData ) noexcept
{
    size_t inIdx;
    size_t outIdx;

    for ( inIdx = 0, outIdx = 0; inIdx < numCharToScan; outIdx++, inIdx += 2 )
    {
        uint8_t hi = hexChar( inString[inIdx] );
        uint8_t lo = hexChar( inString[inIdx + 1] );
        if ( 0xff == hi || 0xff == lo )
        {
            return false;
        }
        outData[outIdx] = ( hi << 4 ) | ( lo & 0xf );
    }

    return true;
}

uint8_t Parse::hexChar( char c ) noexcept
{
    // Characters: 0-9
    if ( '0' <= c && c <= '9' )
    {
        return c - '0';
    }

    // Characters: A-F
    else if ( 'A' <= c && c <= 'F' )
    {
        return c - 'A' + 0xa;
    }

    // Characters: a-f
    else if ( 'a' <= c && c <= 'f' )
    {
        return c - 'a' + 0xa;
    }

    // INVALID character
    else
    {
        return 0xff;
    }
}

////////////////////////////////////////
static bool parseTime( const char* time, uint64_t& convertedValue )
{
    uint16_t    hh;
    const char* endPtr;
    if ( StringTo::unsignedInt<uint16_t>( hh, time, 10, ":", &endPtr ) && *endPtr != '\0' )
    {
        uint16_t mm;
        if ( StringTo::unsignedInt<uint16_t>( mm, endPtr + 1, 10, ":", &endPtr ) && *endPtr != '\0' )
        {
            uint16_t ss;
            if ( StringTo::unsignedInt<uint16_t>( ss, endPtr + 1, 10, ".", &endPtr ) )
            {
                uint64_t seconds = ss + ( mm * 60LL ) + ( hh * 60 * 60LL );
                uint16_t msecs   = 0;

                // Milliseconds is optional
                if ( *endPtr != '\0' && StringTo::unsignedInt<uint16_t>( msecs, endPtr + 1 ) )
                {
                    if ( msecs < 1000 )
                    {
                        msecs = strlen( endPtr + 1 ) == 1   ? msecs * 100
                                : strlen( endPtr + 1 ) == 2 ? msecs * 10
                                                            : msecs;
                    }
                }

                convertedValue = seconds * 1000LL + msecs;
                return true;
            }
        }
    }

    // If I get here - the parse failed
    return false;
}

// format: [DD ]HH:MM:SS[.sss]
bool Parse::timestamp( const char* timestampToParse, uint64_t& convertedValue ) noexcept
{
    // Some error checking
    if ( timestampToParse == nullptr )
    {
        return false;
    }

    const char* firstArg  = Strip::space( timestampToParse );
    const char* secondArg = Strip::space( Strip::notSpace( firstArg ) );

    // No day field
    if ( secondArg == nullptr || *secondArg == '\0' )
    {
        return parseTime( firstArg, convertedValue );
    }

    // Have a day field
    else
    {
        uint16_t day;
        if ( StringTo::unsignedInt<uint16_t>( day, firstArg, 10, " " ) )
        {
            if ( parseTime( secondArg, convertedValue ) )
            {
                convertedValue += day * ( 60 * 60 * 24 * 1000LL );
                return true;
            }
        }
    }

    // If I get here, then the parse failed
    return false;
}

bool Parse::timestamp( const char* timestampToParse, uint32_t& convertedValue ) noexcept
{
    uint64_t temp;
    if ( timestamp( timestampToParse, temp ) )
    {
        if ( temp > UINT32_MAX )
        {
            return false;
        }
        convertedValue = static_cast<uint32_t>( temp );
        return true;
    }
    
    return false;
}

}  // end namespaces
}
//------------------------------------------------------------------------------