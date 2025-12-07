#ifndef KIT_TYPE_ENDIAN_H_
#define KIT_TYPE_ENDIAN_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file

    This file provides a set of macros/method to convert between the 'host'
    platform and Big/Little endian multi-byte layouts. NOTE: The application
    should ALWAYS use the macros, i.e. never directly call the internal
    methods.

    Usage:
    The application defines one of the following macros based on the endianess
    of the platform that the application executes on:
        USE_KIT_TYPE_ENDIAN_LE_HOST
        USE_KIT_TYPE_ENDIAN_BE_HOST

    The code is derived from the public domain source in the repo:
    https://github.com/rofl0r/endianness.h/tree/master
*/

#include "kit_config.h"
#include <stdint.h>
#include <string.h>


#if defined( USE_KIT_TYPE_ENDIAN_LE_HOST )
/// Swaps (if need) the Host/MCU byte order of a 16-bit integer to Big-Endian
#define KIT_TYPE_HTOBE16( x )  Kit::Type::endianBswap16( x )

/// Swaps (if need) Big-Endian byte order of a 16-bit integer to the Host/MCU byte order
#define KIT_TYPE_BE16TOH( x )  Kit::Type::endianBswap16( x )

/// Swaps (if need) the Host/MCU byte order of a 16-bit integer to Big-Endian
#define KIT_TYPE_HTOBE32( x )  Kit::Type::endianBswap32( x )

/// Swaps (if need) Big-Endian byte order of a 32-bit integer to the Host/MCU byte order
#define KIT_TYPE_BE32TOH( x )  Kit::Type::endianBswap32( x )

/// Swaps (if need) the Host/MCU byte order of a 64-bit integer to Big-Endian
#define KIT_TYPE_HTOBE64( x )  Kit::Type::endianBswap64( x )

/// Swaps (if need) Big-Endian byte order of a 64-bit integer to the Host/MCU byte order
#define KIT_TYPE_BE64TOH( x )  Kit::Type::endianBswap64( x )

/// Swaps (if need) the Host/MCU byte order of a float to Big-Endian
#define KIT_TYPE_HTOBEF32( x ) Kit::Type::endianBswapF32( x )

/// Swaps (if need) Big-Endian byte order of a float to the Host/MCU byte order
#define KIT_TYPE_BEF32TOH( x ) Kit::Type::endianBswapF32( x )

/// Swaps (if need) the Host/MCU byte order of a double to Big-Endian
#define KIT_TYPE_HTOBEF64( x ) Kit::Type::endianBswapF64( x )

/// Swaps (if need) Big-Endian byte order of a double to the Host/MCU byte order
#define KIT_TYPE_BEF64TOH( x ) Kit::Type::endianBswapF64( x )



/// Swaps (if need) the Host/MCU byte order of a 16-bit integer to Little-Endian
#define KIT_TYPE_HTOLE16( x )  ( uint16_t )( x )

/// Swaps (if need) Little-Endian byte order of a 16-bit integer to the Host/MCU byte order
#define KIT_TYPE_LE16TOH( x )  ( uint16_t )( x )

/// Swaps (if need) the Host/MCU byte order of a 32-bit integer to Little-Endian
#define KIT_TYPE_HTOLE32( x )  ( uint32_t )( x )

/// Swaps (if need) Little-Endian byte order of a 32-bit integer to the Host/MCU byte order
#define KIT_TYPE_LE32TOH( x )  ( uint32_t )( x )

/// Swaps (if need) the Host/MCU byte order of a 64-bit integer to Little-Endian
#define KIT_TYPE_HTOLE64( x )  ( uint64_t )( x )

/// Swaps (if need) Little-Endian byte order of a 64-bit integer to the Host/MCU byte order
#define KIT_TYPE_LE64TOH( x )  ( uint64_t )( x )

/// Swaps (if need) the Host/MCU byte order of a float to Little-Endian
#define KIT_TYPE_HTOLEF32( x ) (float)( x )

/// Swaps (if need) Little-Endian byte order of a float to the Host/MCU byte order
#define KIT_TYPE_LEF32TOH( x ) (float)( x )

/// Swaps (if need) the Host/MCU byte order of a double to Little-Endian
#define KIT_TYPE_HTOLEF64( x ) (double)( x )

/// Swaps (if need) Little-Endian byte order of a double to the Host/MCU byte order
#define KIT_TYPE_LEF64TOH( x ) (double)( x )


#elif defined( USE_KIT_TYPE_ENDIAN_BE_HOST )
#define KIT_TYPE_HTOBE16( x )  ( uint16_t )( x )
#define KIT_TYPE_BE16TOH( x )  ( uint16_t )( x )
#define KIT_TYPE_HTOBE32( x )  ( uint32_t )( x )
#define KIT_TYPE_BE32TOH( x )  ( uint32_t )( x )
#define KIT_TYPE_HTOBE64( x )  ( uint64_t )( x )
#define KIT_TYPE_BE64TOH( x )  ( uint64_t )( x )
#define KIT_TYPE_HTOBEF32( x ) (float)( x )
#define KIT_TYPE_BEF32TOH( x ) (float)( x )
#define KIT_TYPE_HTOBEF64( x ) (double)( x )
#define KIT_TYPE_BEF64TOH( x ) (double)( x )
#define KIT_TYPE_HTOLE16( x )  Kit::Type::endianBswap16( x )
#define KIT_TYPE_LE16TOH( x )  Kit::Type::endianBswap16( x )
#define KIT_TYPE_HTOLE32( x )  Kit::Type::endianBswap32( x )
#define KIT_TYPE_LE32TOH( x )  Kit::Type::endianBswap32( x )
#define KIT_TYPE_HTOLE64( x )  Kit::Type::endianBswap64( x )
#define KIT_TYPE_LE64TOH( x )  Kit::Type::endianBswap64( x )
#define KIT_TYPE_HTOLEF32( x ) Kit::Type::endianBswapF32( x )
#define KIT_TYPE_LEF32TOH( x ) Kit::Type::endianBswapF32( x )
#define KIT_TYPE_HTOLEF64( x ) Kit::Type::endianBswapF64( x )
#define KIT_TYPE_LEF64TOH( x ) Kit::Type::endianBswapF64( x )

// Require the application to explicitly define the platform's endianess
#else
#error "Must define one of the following: USE_KIT_TYPE_ENDIAN_LE_HOST or USE_KIT_TYPE_ENDIAN_BE_HOST"
#endif

///
namespace Kit {
///
namespace Type {

/// Internal method.  Do not call directly - always use the macros above
inline uint16_t endianBswap16( uint16_t x )
{
    return ( x << 8 ) | ( x >> 8 );
}

/// Internal method.  Do not call directly - always use the macros above
inline uint32_t endianBswap32( uint32_t x )
{
    return ( x >> 24 ) | ( x >> 8 & 0xff00 ) | ( x << 8 & 0xff0000 ) | ( x << 24 );
}

/// Internal method. Do not call directly - always use the macros above
inline float endianBswapF32( float x )
{
    uint32_t tmp;
    memcpy( &tmp, &x, sizeof( tmp ) );
    tmp = endianBswap32( tmp );
    float result;
    memcpy( &result, &tmp, sizeof( result ) );
    return result;
}

/// Internal method.  Do not call directly - always use the macros above
inline uint64_t endianBswap64( uint64_t x )
{
    return ( ( endianBswap32( (uint32_t)x ) + 0ULL ) << 32 ) | ( endianBswap32( x >> 32 ) );
}

/// Internal method.  Do not call directly - always use the macros above
inline double endianBswapF64( double x )
{
    uint64_t tmp;
    memcpy( &tmp, &x, sizeof( tmp ) );
    tmp = endianBswap64( tmp );
    double result;
    memcpy( &result, &tmp, sizeof( result ) );
    return result;
}

}  // End namespace(s)
}


#endif  // end header latch