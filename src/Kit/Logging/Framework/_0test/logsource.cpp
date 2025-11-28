/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Logging/Framework/LogSource.h"
#include "Kit/Logging/Pkg/ClassificationId.h"
#include "Kit/Logging/Pkg/Log.h"
#include "Kit/Logging/Pkg/SubSystemId.h"
#include "Kit/System/Trace.h"
#include "Kit/Logging/Framework/Mocked4Test/KitOnly.h"
#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include "catch2/catch_test_macros.hpp"


using namespace Kit::Logging::Pkg;
using namespace Kit::Logging::Framework;
using namespace Kit::Logging::Framework::Mocked4Test;

#define SECT_ "_0test"

// Create a LogSource instance for the tests
static KitOnly logApp_;

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "LogSource" )
{
    KIT_SYSTEM_TRACE_FUNC( SECT_ );
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();

    SECTION( "basic" )
    {
        unsigned               arg    = 42;
        LogSource::LogResult_T result = logfSystem( ClassificationId::EVENT, SubSystemId::SYSTEM, SystemMsgId::SHUTDOWN, "shutdown message = %u", arg );

        bool result = Formatter::toString( logApp, logEntry, formattedText );
        KIT_SYSTEM_TRACE_MSG( SECT_, "%s", formattedText.getString() );
        REQUIRE( result == true );
        REQUIRE( formattedText == "(5:2022-12-31 23:59:59.123) WARNING-KIT-SYSTEM-SHUTDOWN: Test log message" );

        logEntry.m_timestamp        = Kit::Time::constructBootTime( 60000, 1672531199123 );  // 2022/12/31-23:59:59.123 UTC
        logEntry.m_classificationId = ClassificationId::INFO;
        logEntry.m_packageId        = Package::PACKAGE_ID;
        logEntry.m_subSystemId      = SubSystemId::DRIVER;
        logEntry.m_messageId        = DriverMsgId::STOP_ERR;
        msgText                     = "With Persistent Storage ID log message";
        strcpy( logEntry.m_infoText, msgText );

        result = Formatter::toString( logApp, logEntry, formattedText, 12 );
        KIT_SYSTEM_TRACE_MSG( SECT_, "%s", formattedText.getString() );
        REQUIRE( result == true );
        REQUIRE( formattedText == "[12] (60000:2022-12-31 23:59:59.123) INFO-KIT-DRIVER-STOP_ERR: With Persistent Storage ID log message" );
    }

    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}