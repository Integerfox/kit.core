/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/System/Watchdog/Hal.h"
#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include "catch2/catch_test_macros.hpp"
#include "Kit/System/Trace.h"
#include "Kit/System/Api.h"
#include "Kit/System/Thread.h"
#include "Kit/EventQueue/Server.h"
#include "Kit/System/Watchdog/WatchedEventThread.h"
#include "Kit/System/Watchdog/Supervisor.h"
#include "wdoghal.h"

#define SECT_ "_0test"


//------------------------------------------------------------------------------
// Test Constants
//------------------------------------------------------------------------------

#define WDOG_TIMEOUT_MS             500
#define WDOG_THREAD_HEALTH_CHECK_MS 100

using namespace Kit::System;
using namespace Kit::System::Watchdog;
using namespace Kit::EventQueue;

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "watchdog" )
{
    KIT_SYSTEM_TRACE_FUNC( SECT_ );
    resetWatchdogHalMocks();

    SECTION( "server - with watchdog" )
    {
        // Server with with the thread's watchdog "enabled"
        WatchedEventThread wdogSetup( WDOG_TIMEOUT_MS, WDOG_THREAD_HEALTH_CHECK_MS, true );
        Server             uut( OPTION_KIT_SYSTEM_EVENT_LOOP_TIMEOUT_PERIOD, nullptr, &wdogSetup );

        auto* t1 = Kit::System::Thread::create( uut, "WATCHED-THREAD" );
        REQUIRE( t1 );

        Supervisor::enableWdog();
        uint32_t delayMs = WDOG_TIMEOUT_MS * 2;
        sleep( delayMs );

        REQUIRE( g_watchdogEnabled == true );
        REQUIRE( g_kickCount >= ( (unsigned long)( ( delayMs / OPTION_KIT_SYSTEM_EVENT_LOOP_TIMEOUT_PERIOD / OPTION_KIT_SYSTEM_WATCHDOG_SUPERVISOR_TICK_DIVIDER ) * 0.9 ) ) );
        REQUIRE( g_tripCount == 0 );

        uut.pleaseStop();
        Kit::System::Thread::destroy( *t1, 100 );
    }

    SECTION( "server - no watchdog" )
    {
        // Server with with the thread's watchdog "enabled"
        WatchedEventThread wdogSetup( WDOG_TIMEOUT_MS, WDOG_THREAD_HEALTH_CHECK_MS, true );
        Server             uut( OPTION_KIT_SYSTEM_EVENT_LOOP_TIMEOUT_PERIOD );

        auto* t1 = Kit::System::Thread::create( uut, "WATCHED-THREAD" );
        REQUIRE( t1 );

        Supervisor::enableWdog();
        uint32_t delayMs = WDOG_TIMEOUT_MS * 2;
        sleep( delayMs );

        REQUIRE( g_watchdogEnabled == true );
        REQUIRE( g_kickCount == 0 );
        REQUIRE( g_kickCount == 0 );

        uut.pleaseStop();
        Kit::System::Thread::destroy( *t1, 100 );
    }

    REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
}