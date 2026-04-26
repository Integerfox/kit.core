/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */


#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include "catch2/catch_test_macros.hpp"
#include "Kit/System/Trace.h"
#include "Kit/Driver/NV/Null.h"
#include <cstdint>
#include <string.h>
#include <inttypes.h>


#define SECT_ "_0test"

///
using namespace Kit::Driver::NV;


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Null" )
{
    KIT_SYSTEM_TRACE_FUNC( SECT_ );
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();
    Null uut;

    SECTION( "start/stop" )
    {
        REQUIRE( uut.start() == true );
        REQUIRE( uut.start() == true );
        uut.stop();
        uut.stop();
    }

    SECTION( "read/write")
    {
        constexpr size_t numBytes = 10;
        uint8_t srcData[numBytes];
        uint8_t dstData[numBytes];
        uint8_t expectedData[numBytes]; // The read() method always returns all zeros
        memset( srcData, 0xA5, numBytes );
        memset( dstData, 0xFF, numBytes );
        memset( expectedData, 0x00, numBytes );

        REQUIRE( uut.write( 0, srcData, numBytes ) == true );
        REQUIRE( uut.read( 0, dstData, sizeof(dstData), numBytes ) == true );
        REQUIRE( memcmp( expectedData, dstData, numBytes ) == 0 );

        REQUIRE( uut.read( 0, dstData, sizeof(dstData), sizeof(dstData) + 1 ) == false ); 
    }

    SECTION( "page info" )
    {
        REQUIRE( uut.getPageSize() == OPTION_KIT_DRIVER_NV_NULL_PAGE_SIZE );
        REQUIRE( uut.getNumPages() == OPTION_KIT_DRIVER_NV_NULL_TOTAL_PAGES );
        REQUIRE( uut.getTotalSize() == ( OPTION_KIT_DRIVER_NV_NULL_PAGE_SIZE * OPTION_KIT_DRIVER_NV_NULL_TOTAL_PAGES ) );
    }

    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}
