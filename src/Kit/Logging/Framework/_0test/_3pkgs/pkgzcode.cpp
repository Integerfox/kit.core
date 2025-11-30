/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Logging/Framework/_0test/_3pkgs/PkgZ/Logging/Pkg/Log.h"
#include "Kit/Logging/Framework/_0test/_3pkgs/PkgZ/Logging/Pkg/MsgId.h"
#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include "catch2/catch_test_macros.hpp"
#include "Kit/Container/RingBufferAllocate.h"  // Only needed for test access to log queue
#include "Kit/Logging/Framework/EntryData.h"   // Only needed for test access to log queue

using namespace PkgZ::Logging::Pkg;
using namespace Kit::Logging::Framework;

#define SECT_ "_0test"


// Get access to the application's log queue to validate log entries
extern Kit::Container::RingBufferAllocate<Kit::Logging::Framework::EntryData_T, 5> g_logFifo;

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "PkgZ" )
{
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();
    LogResult_T result;
    EntryData_T entry;

    SECTION( "basic" )
    {
        result = PkgZ::Logging::Pkg::logfCore(ClassificationId::METRICS, CoreMsgId::CONFIG_ERROR, "Metrics Classification --> mapped to EVENT" );
        REQUIRE( result == LogResult_T::ADDED );
        REQUIRE( g_logFifo.getNumItems() == 1u );
        REQUIRE( g_logFifo.remove( entry ) );
        REQUIRE( entry.m_classificationId == ClassificationId::METRICS );
        REQUIRE( entry.m_packageId == Package::PACKAGE_ID );
        REQUIRE( entry.m_subSystemId == SubSystemId::CORE );
        REQUIRE( entry.m_messageId == CoreMsgId::CONFIG_ERROR );

        result = logfNetwork( ClassificationId::DEBUG, NetworkMsgId::SEND_ERROR, "DEBUG 1-1 classification mapping --> mapped to DEBUG" );
        REQUIRE( result == LogResult_T::ADDED );
        REQUIRE( g_logFifo.getNumItems() == 1u );
        REQUIRE( g_logFifo.remove( entry ) );
        REQUIRE( entry.m_classificationId == ClassificationId::DEBUG );
        REQUIRE( entry.m_packageId == Package::PACKAGE_ID );
        REQUIRE( entry.m_subSystemId == SubSystemId::NETWORK );
        REQUIRE( entry.m_messageId == NetworkMsgId::SEND_ERROR );
    }

    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}
