/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Logging/Framework/NullPackage.h"
#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include "catch2/catch_test_macros.hpp"


using namespace Kit::Logging::Framework;

#define SECT_                      "_0test"

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "NullPackage" )
{
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();
    NullPackage uut;

    SECTION( "basic" )
    {
        REQUIRE( uut.packageId() == IPackage::NULL_PKG_ID );
        REQUIRE( strcmp( uut.packageIdString(), IPackage::NULL_PKG_ID_TEXT ) == 0 );

        const char* subSystemText = nullptr;
        const char* messageText   = nullptr;
        REQUIRE( uut.subSystemAndMessageIdsToString( 1, subSystemText, 1, messageText ) == false );
        REQUIRE( subSystemText == nullptr );
        REQUIRE( messageText == nullptr );
    }   

    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}
