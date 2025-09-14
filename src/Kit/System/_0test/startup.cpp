/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "kit_config.h"
#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include "catch2/catch_test_macros.hpp"
#include "Kit/System/Trace.h"
#include "Kit/System/PrivateStartup.h"
#include "Kit/System/ElapsedTime.h"
#include "Kit/System/api.h"

#define SECT_ "_0test"
///
using namespace Kit::System;

// Native timestamp - since KIT's elapsed time uses the IStartupHook interface.  Must support a duration of at least 10s, with at least 1ms resolution
extern uint32_t getNativeTimestamp();

//////////////////////////////////////////////
namespace {  // anonymous namespace
class RegisterInitHandler : public IStartupHook
{
public:
    uint32_t timestamp;
    /// Constructor
    RegisterInitHandler( InitLevel myInitLevel ) noexcept
        : IStartupHook( myInitLevel )
        , timestamp( 0 )
    {
    }

protected:
    ///
    void notify( InitLevel init_level ) noexcept override
    {
        timestamp = getNativeTimestamp();
        sleep(10);  // ensure that timestamps are different
    }
};

}  // end anonymous namespace

static RegisterInitHandler uutApplication_( IStartupHook::APPLICATION );  
static RegisterInitHandler uutSystem_( IStartupHook::SYSTEM );
static RegisterInitHandler uutMiddleWare_( IStartupHook::MIDDLE_WARE );
static RegisterInitHandler uutTestInfra_( IStartupHook::TEST_INFRA );

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "startup" )
{
    KIT_SYSTEM_TRACE_FUNC( SECT_ );
    ShutdownUnitTesting::clearAndUseCounter();

    REQUIRE( uutTestInfra_.timestamp != 0u );   
    REQUIRE( uutSystem_.timestamp > uutTestInfra_.timestamp );   
    REQUIRE( uutMiddleWare_.timestamp > uutSystem_.timestamp );   
    REQUIRE( uutApplication_.timestamp > uutMiddleWare_.timestamp );

    REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
}
