/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Logging/Framework/ILog.h"
#include "Kit/System/Trace.h"
#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include "catch2/catch_test_macros.hpp"


using namespace Kit::Logging::Framework;

#define SECT_ "_0test"

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "ILog" )
{
    KIT_SYSTEM_TRACE_FUNC( SECT_ );
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();

    REQUIRE( sizeof( KitLoggingClassificationMask_T ) == 4u );
    REQUIRE( sizeof( KitLoggingPackageMask_T ) == 4u );

    REQUIRE( ILog::classificationIdToMask( 0 ) == 0u );
    REQUIRE( ILog::classificationIdToMask( 1 ) == 0x00000001u );
    REQUIRE( ILog::classificationIdToMask( 2 ) == 0x00000002u );
    REQUIRE( ILog::classificationIdToMask( 32 ) == 0x80000000u );
    REQUIRE( ILog::classificationIdToMask( 33 ) == 0u );
    REQUIRE( ILog::maskToClassificationId( 0x00000001u ) == 1u );
    REQUIRE( ILog::maskToClassificationId( 0x00000002u ) == 2u );
    REQUIRE( ILog::maskToClassificationId( 0x80000000u ) == 32u );
    REQUIRE( ILog::maskToClassificationId( 0x00000003u ) == 0u );

    REQUIRE( ILog::packageIdToMask( 0 ) == 0u );
    REQUIRE( ILog::packageIdToMask( 1 ) == 0x00000001u );
    REQUIRE( ILog::packageIdToMask( 2 ) == 0x00000002u );
    REQUIRE( ILog::packageIdToMask( 32 ) == 0x80000000u );
    REQUIRE( ILog::packageIdToMask( 33 ) == 0u );
    REQUIRE( ILog::maskToPackageId( 0x00000001u ) == 1u );
    REQUIRE( ILog::maskToPackageId( 0x00000002u ) == 2u );
    REQUIRE( ILog::maskToPackageId( 0x80000000u ) == 32u );
    REQUIRE( ILog::maskToPackageId( 0x00000003u ) == 0u );

    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}
