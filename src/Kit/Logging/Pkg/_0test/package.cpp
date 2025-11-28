/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Logging/Pkg/Log.h"
#include "Kit/Logging/Pkg/Package.h"
#include "Kit/System/Trace.h"
#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include "catch2/catch_test_macros.hpp"


using namespace Kit::Logging::Pkg;

#define SECT_ "_0test"


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Package" )
{
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();
    Package uut;

    SECTION( "basic" )
    {
        REQUIRE( uut.packageId() == KIT_LOGGING_PKG_PACKAGE_ID );
        REQUIRE( strcmp( uut.packageIdString(), OPTION_KIT_LOGGING_PKG_PACKAGE_ID_TEXT ) == 0 );

        REQUIRE( strcmp( uut.subSystemIdToString( SubSystemId::SYSTEM ), "SYSTEM" ) == 0 );
        REQUIRE( strcmp( uut.subSystemIdToString( 0xFF ), OPTION_KIT_LOGGING_FRAMEWORK_UNKNOWN_SUBSYSTEM_ID_TEXT ) == 0 );

        REQUIRE( strcmp( uut.messageIdToString( SubSystemId::SYSTEM, SystemMsgId::SHUTDOWN ), "SHUTDOWN" ) == 0 );
        REQUIRE( strcmp( uut.messageIdToString( SubSystemId::SYSTEM, 0xFF ), OPTION_KIT_LOGGING_FRAMEWORK_UNKNOWN_MESSAGE_ID_TEXT ) == 0 );

        REQUIRE( strcmp( uut.messageIdToString( SubSystemId::DRIVER, DriverMsgId::STOP_ERR ), "STOP_ERR" ) == 0 );
        REQUIRE( strcmp( uut.messageIdToString( SubSystemId::DRIVER, 0xFF ), OPTION_KIT_LOGGING_FRAMEWORK_UNKNOWN_MESSAGE_ID_TEXT ) == 0 );

        REQUIRE( strcmp( uut.messageIdToString( 0xFF, SystemMsgId::SHUTDOWN ), OPTION_KIT_LOGGING_FRAMEWORK_UNKNOWN_MESSAGE_ID_TEXT ) == 0 );
    }

    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}
