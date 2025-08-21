/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "StringTo.h"
#include "Strip.h"


//------------------------------------------------------------------------------
namespace Kit {
namespace Text {

///////////////////
static bool conversionOk( const char* originalString, char* endPtr, const char* validStopChars )
{
    // No conversion occurred
    if ( !originalString || *originalString == '\0' || endPtr == originalString )
    {
        return false;
    }

    // Valid conversion (stopped on End-of-String)
    if ( *endPtr == '\0' )
    {
        return true;
    }

    // At least a partial conversion occurred, determine if it is OK
    while ( validStopChars && *validStopChars != '\0' )
    {
        if ( *endPtr == *validStopChars )
        {
            return true;
        }
        validStopChars++;
    }

    return false;
}

bool StringTo::a2ll( long long& convertedValue, const char* string, int base, const char* validStopChars, const char** end ) noexcept
{
    long long value  = 0;
    char*     endPtr = 0;
    if ( string )
    {
        value = strtoll( string, &endPtr, base );
    }
    if ( end )
    {
        *end = endPtr;
    }

    // Only update the client's variable if the conversion was successful
    if ( conversionOk( string, endPtr, validStopChars ) )
    {
        convertedValue = value;
        return true;
    }

    return false;
}

bool StringTo::a2ull( unsigned long long& convertedValue, const char* string, int base, const char* validStopChars, const char** end ) noexcept
{
    unsigned long long value  = 0;
    char*              endPtr = 0;
    if ( string )
    {
        const char* strPtr = Strip::space( string );
        if ( *strPtr == '-' )
        {
            // Negative numbers are not allowed for unsigned types
            return false;
        }
        value = strtoull( strPtr, &endPtr, base );
    }
    if ( end )
    {
        *end = endPtr;
    }

    // Only update the client's variable if the conversion was successful
    if ( conversionOk( string, endPtr, validStopChars ) )
    {
        convertedValue = value;
        return true;
    }

    return false;
}

bool StringTo::floating( float&       convertedValue,
                         const char*  string,
                         const char*  validStopChars,
                         const char** end ) noexcept

{
    float value;
    char* endPtr = nullptr;
    if ( string )
    {
        value = strtof( string, &endPtr );
    }
    if ( end )
    {
        *end = endPtr;
    }

    // Only update the client's variable if the conversion was successful
    if ( conversionOk( string, endPtr, validStopChars ) )
    {
        convertedValue = value;
        return true;
    }
    return false;
}

bool StringTo::floating( double&      convertedValue,
                         const char*  string,
                         const char*  validStopChars,
                         const char** end ) noexcept

{
    double value;
    char*  endPtr = nullptr;
    if ( string )
    {
        value = strtod( string, &endPtr );
    }
    if ( end )
    {
        *end = endPtr;
    }

    // Only update the client's variable if the conversion was successful
    if ( conversionOk( string, endPtr, validStopChars ) )
    {
        convertedValue = value;
        return true;
    }
    return false;
}


///////////////////
bool StringTo::boolean( bool& convertedValue, const char* string, const char* trueToken, const char* falseToken, const char** end ) noexcept
{
    size_t tokenSize = 0;
    bool   result    = false;

    if ( !string || !trueToken || !falseToken )
    {
        return false;
    }

    if ( strncmp( string, trueToken, strlen( trueToken ) ) == 0 )
    {
        convertedValue = true;
        tokenSize      = strlen( trueToken );
        result         = true;
    }
    else if ( strncmp( string, falseToken, strlen( falseToken ) ) == 0 )
    {
        convertedValue = false;
        tokenSize      = strlen( falseToken );
        result         = true;
    }

    if ( end )
    {
        *end = string + tokenSize;
    }

    return result;
}


}  // end namespaces
}
//------------------------------------------------------------------------------