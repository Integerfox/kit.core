/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Logging/Framework/IApplication.h"
#include "Kit/Logging/Pkg/Log.h"
#include "Kit/Logging/Pkg/Package.h"
#include "Kit/Logging/Pkg/MsgId.h"
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

        const char* subSystemText = nullptr;
        const char* messageText   = nullptr;
        REQUIRE( uut.subSystemAndMessageIdsToString(SubSystemId::SYSTEM , subSystemText, SystemMsgId::SHUTDOWN, messageText));
        REQUIRE( strcmp( subSystemText, "SYSTEM" ) == 0 );
        REQUIRE( strcmp( messageText, "SHUTDOWN" ) == 0 );

        REQUIRE( uut.subSystemAndMessageIdsToString(100, subSystemText, DriverMsgId::STOP_ERR, messageText) == false );
        REQUIRE( subSystemText == nullptr );
        REQUIRE( messageText == nullptr );      

        REQUIRE( uut.subSystemAndMessageIdsToString(SubSystemId::DRIVER , subSystemText, 200, messageText) == false );
        REQUIRE( strcmp( subSystemText, "DRIVER" ) == 0 );
        REQUIRE( messageText == nullptr );

        REQUIRE( uut.subSystemAndMessageIdsToString(SubSystemId::LOGGING , subSystemText, LoggingMsgId::UNKNOWN_MESSAGE_ID, messageText));
        REQUIRE( strcmp( subSystemText, "LOGGING" ) == 0 );
        REQUIRE( strcmp( messageText, "UNKNOWN_MESSAGE_ID" ) == 0 );
    }

    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}
