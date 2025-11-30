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
#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include "catch2/catch_test_macros.hpp"
#include "Kit/Container/RingBufferAllocate.h"  // Only needed for test access to log queue
#include "Kit/Logging/Framework/EntryData.h"   // Only needed for test access to log queue

using namespace Kit::Logging::Pkg;
using namespace Kit::Logging::Framework;

#define SECT_ "_0test"


// Get access to the application's log queue to validate log entries
extern Kit::Container::RingBufferAllocate<Kit::Logging::Framework::EntryData_T, 5> g_logFifo;

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Kit" )
{
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();
    LogResult_T result;
    EntryData_T entry;

    SECTION( "basic" )
    {
        result = logfSystem( ClassificationId::FATAL, SystemMsgId::SHUTDOWN, "Fatal Classification --> mapped to WARNING" );
        REQUIRE( result == LogResult_T::ADDED );
        REQUIRE( g_logFifo.getNumItems() == 1u );
        REQUIRE( g_logFifo.remove( entry ) );
        REQUIRE( entry.m_classificationId == ClassificationId::FATAL );
        REQUIRE( entry.m_packageId == Package::PACKAGE_ID );
        REQUIRE( entry.m_subSystemId == SubSystemId::SYSTEM );
        REQUIRE( entry.m_messageId == SystemMsgId::SHUTDOWN );

        result = logfDriver( ClassificationId::EVENT, DriverMsgId::START_ERR, "EVENT 1-1 classification mapping --> mapped to EVENT" );
        REQUIRE( result == LogResult_T::ADDED );
        REQUIRE( g_logFifo.getNumItems() == 1u );
        REQUIRE( g_logFifo.remove( entry ) );
        REQUIRE( entry.m_classificationId == ClassificationId::EVENT );
        REQUIRE( entry.m_packageId == Package::PACKAGE_ID );
        REQUIRE( entry.m_subSystemId == SubSystemId::DRIVER );
        REQUIRE( entry.m_messageId == DriverMsgId::START_ERR );
    }

    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}
