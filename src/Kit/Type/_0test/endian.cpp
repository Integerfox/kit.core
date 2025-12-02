/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#define USE_KIT_TYPE_ENDIAN_LE_HOST  // Force x86 architecture

#include "Kit/Type/endian.h"
#include "catch2/catch_test_macros.hpp"
#include "Kit/System/Trace.h"
#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include "Kit/Text/FString.h"
#include "Kit/Text/Format.h"

#define SECT_ "_0test"

using namespace Kit::Type;
///


////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "endian" )
{
    KIT_SYSTEM_TRACE_FUNC( SECT_ );
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();

    SECTION( "swap" )
    {
        uint16_t wIn  = 0x1234;
        uint16_t wOut = Kit::Type::endianBswap16( wIn );
        REQUIRE( wOut == 0x3412 );

        uint32_t dwIn  = 0x12345678;
        uint32_t dwOut = Kit::Type::endianBswap32( dwIn );
        REQUIRE( dwOut == 0x78563412 );

        uint64_t qwIn  = 0x123456789ABCDEF0LL;
        uint64_t qwOut = Kit::Type::endianBswap64( qwIn );
        REQUIRE( qwOut == 0xF0DEBC9A78563412LL );

        Kit::Text::FString<128> tmp;
        float fin = 3.14F;
        Kit::Text::Format::asciiHex( &fin, sizeof(fin), tmp );
        printf( "fin =%s\n", tmp.getString() );
		float fout = Kit::Type::endianBswapF32(fin);
        Kit::Text::Format::asciiHex( &fout, sizeof(fout), tmp );
        printf( "fout=%s\n", tmp.getString() );
        REQUIRE( fout != fin );
        fout = Kit::Type::endianBswapF32(fout);
        REQUIRE( fout == 3.14F );
        Kit::Text::Format::asciiHex( &fout, sizeof(fout), tmp );
        printf( "fout=%s\n", tmp.getString() );

        double dfin = 3.14;
        Kit::Text::Format::asciiHex( &dfin, sizeof(dfin), tmp );
        printf( "dfin =%s\n", tmp.getString() );
		double dfout = Kit::Type::endianBswapF64(dfin);
        Kit::Text::Format::asciiHex( &dfout, sizeof(dfout), tmp );
        printf( "dfout=%s\n", tmp.getString() );
        REQUIRE( dfout != dfin );
        dfout = Kit::Type::endianBswapF64(dfout);
        REQUIRE( dfout == 3.14 );
        Kit::Text::Format::asciiHex( &dfout, sizeof(dfout), tmp );
        printf( "dfout=%s\n", tmp.getString() );
    }

    SECTION( "swap2" )
    {
        int16_t wIn  = 0x1234;
        int16_t wOut = Kit::Type::endianBswap16( wIn );
        REQUIRE( wOut == 0x3412 );

        int32_t dwIn  = 0x12345678;
        int32_t dwOut = Kit::Type::endianBswap32( dwIn );
        REQUIRE( dwOut == 0x78563412 );

        int64_t qwIn  = 0x123456789ABCDEF0LL;
        int64_t qwOut = Kit::Type::endianBswap64( qwIn );
        REQUIRE( qwOut == -1090226688147180526LL ); // aka 0xF0DEBC9A78563412
    }

    SECTION( "roundtrip" )
    {
        uint16_t wIn  = 0x1234;
        uint16_t wOut = KIT_TYPE_BE16TOH( KIT_TYPE_HTOBE16( wIn ) );
        REQUIRE( wOut == wIn );

        uint32_t dwIn  = 0x12345678;
        uint32_t dwOut = KIT_TYPE_BE32TOH( KIT_TYPE_HTOBE32( dwIn ) );
        REQUIRE( dwOut == dwIn );

        uint64_t qwIn  = 0x123456789ABCDEF0LL;
        uint64_t qwOut = KIT_TYPE_BE64TOH( KIT_TYPE_HTOBE64( qwIn ) );
        REQUIRE( qwOut == qwIn );
    }

    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}
