/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Logging/Framework/EntryData.h"
#include "Kit/Logging/Framework/_0test/_3pkgs/Foo/Logging/App/Foo.h"
#include "Kit/Logging/Framework/_0test/_3pkgs/Foo/Logging/Pkg/Log.h"
#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include "catch2/catch_test_macros.hpp"
#include "Kit/Container/RingBufferAllocate.h"

using namespace Foo::Logging::Pkg;
using namespace Kit::Logging::Framework;

#define SECT_ "_0test"


// Create the Application's Logging instance for the tests. 
// Note: Not static so it can be shared with other tests
Kit::Container::RingBufferAllocate<EntryData_T, 5> g_logFifo;
Foo::Logging::App::Foo g_logApp( g_logFifo );

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "App" )
{
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();
    LogResult_T result;
    EntryData_T entry;

    SECTION( "basic" )
    {
        result = logfApi( ClassificationId::WARNING, ApiMsgId::AUTH_FAILED, "API auth failed for user %s", "test_user" );

        REQUIRE( result == LogResult_T::ADDED );
        REQUIRE( g_logFifo.getNumItems() == 1u );
        REQUIRE( g_logFifo.remove( entry ) );
        REQUIRE( entry.m_classificationId == ClassificationId::WARNING );
        REQUIRE( entry.m_packageId == Foo::Logging::Pkg::Package::PACKAGE_ID );
        REQUIRE( entry.m_subSystemId ==SubSystemId::API );
        REQUIRE( entry.m_messageId == ApiMsgId::AUTH_FAILED );
    }

    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}
