/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Logging/Framework/Logger.h"
#include "Kit/Logging/Framework/EntryData.h"
#include "Kit/Logging/Framework/Log.h"
#include "Kit/Logging/Framework/Mocked4Test/KitOnly.h"
#include "Kit/Logging/Framework/types.h"
#include "Kit/System/Trace.h"
#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include "Kit/Time/BootTime.h"
#include "catch2/catch_test_macros.hpp"


using namespace Kit::Logging::Framework;
using namespace Kit::Logging::Pkg;

#define SECT_         "_0test"

#define MY_FATAL      KIT_LOGGING_API_CLASSIFICATION_ID_FATAL_MAPCFG
#define MY_WARNING    KIT_LOGGING_API_CLASSIFICATION_ID_WARNING_MAPCFG
#define MY_EVENT      KIT_LOGGING_API_CLASSIFICATION_ID_EVENT_MAPCFG
#define MY_INFO       KIT_LOGGING_API_CLASSIFICATION_ID_INFO_MAPCFG
#define MY_PACKAGE_ID KIT_LOGGING_PKG_PACKAGE_ID_MAPCFG


// Create a Log-Application instance for the tests. Also provide whitebox testing support
// Note: Not static so it can be shared with formatter.cpp test
Kit::Logging::Framework::Mocked4Test::KitOnly logApp_;

/// helper method to call vlogf
KIT_SYSTEM_PRINTF_CHECKER( 5, 6 )
static inline LogResult_T logf( uint8_t catId, uint8_t pkgId, uint8_t subSysId, uint8_t msgId, const char* msgTextFormat, ... ) noexcept
{
    va_list ap;
    va_start( ap, msgTextFormat );
    auto result = vlogf( catId, pkgId, subSysId, msgId, msgTextFormat, ap );
    va_end( ap );
    return result;
}

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Logger" )
{
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();
    EntryData_T entry;
    logApp_.reset();
    uint64_t now = Kit::Time::getBootTime();

    SECTION( "masks" )
    {
        REQUIRE( sizeof( KitLoggingClassificationMask_T ) == 4u );
        REQUIRE( sizeof( KitLoggingPackageMask_T ) == 4u );

        REQUIRE( classificationIdToMask( 0 ) == 0u );
        REQUIRE( classificationIdToMask( 1 ) == 0x00000001u );
        REQUIRE( classificationIdToMask( 2 ) == 0x00000002u );
        REQUIRE( classificationIdToMask( 32 ) == 0x80000000u );
        REQUIRE( classificationIdToMask( 33 ) == 0u );
        REQUIRE( maskToClassificationId( 0x00000001u ) == 1u );
        REQUIRE( maskToClassificationId( 0x00000002u ) == 2u );
        REQUIRE( maskToClassificationId( 0x80000000u ) == 32u );
        REQUIRE( maskToClassificationId( 0x00000003u ) == 0u );

        REQUIRE( packageIdToMask( 0 ) == 0u );
        REQUIRE( packageIdToMask( 1 ) == 0x00000001u );
        REQUIRE( packageIdToMask( 2 ) == 0x00000002u );
        REQUIRE( packageIdToMask( 32 ) == 0x80000000u );
        REQUIRE( packageIdToMask( 33 ) == 0u );
        REQUIRE( maskToPackageId( 0x00000001u ) == 1u );
        REQUIRE( maskToPackageId( 0x00000002u ) == 2u );
        REQUIRE( maskToPackageId( 0x80000000u ) == 32u );
        REQUIRE( maskToPackageId( 0x00000003u ) == 0u );
    }

    SECTION( "vlogf" )
    {
        REQUIRE( logApp_.getLogQueueCount() == 0 );

        unsigned    arg    = 42;
        LogResult_T result = logf( MY_EVENT, MY_PACKAGE_ID, 0, 0, "My message = %u", arg );

        REQUIRE( result == LogResult_T::ADDED );
        REQUIRE( logApp_.getLogQueueCount() == 1u );
        REQUIRE( logApp_.m_logFifo.remove( entry ) );
        REQUIRE( entry.m_classificationId == MY_EVENT );
        REQUIRE( entry.m_packageId == MY_PACKAGE_ID );
        REQUIRE( entry.m_subSystemId == 0u );
        REQUIRE( entry.m_messageId == 0u );
        REQUIRE( strcmp( entry.m_infoText, "My message = 42" ) == 0 );
        REQUIRE( entry.m_timestamp >= now );
    }

    SECTION( "classification-filter" )
    {
        REQUIRE( logApp_.getLogQueueCount() == 0 );
        KitLoggingClassificationMask_T eventMask = classificationIdToMask( MY_EVENT );
        Kit::Logging::Framework::disableClassification( eventMask );
        
        unsigned    arg    = 43;
        LogResult_T result = logf( MY_EVENT, MY_PACKAGE_ID, 0, 0, "should be filtered = %u", arg );

        REQUIRE( result == LogResult_T::FILTERED );
        REQUIRE( logApp_.getLogQueueCount() == 0u );
        REQUIRE( logApp_.m_logFifo.remove( entry ) == false );

        Kit::Logging::Framework::enableClassification( eventMask );
        result = logf( MY_EVENT, MY_PACKAGE_ID, 0, 0, "Pass the filtered = %u", ++arg );
        REQUIRE( result == LogResult_T::ADDED );
        REQUIRE( logApp_.getLogQueueCount() == 1u );
        REQUIRE( logApp_.m_logFifo.remove( entry ) );
        REQUIRE( entry.m_classificationId == MY_EVENT );
        REQUIRE( entry.m_packageId == MY_PACKAGE_ID );
        REQUIRE( entry.m_subSystemId == 0u );
        REQUIRE( entry.m_messageId == 0u );
        REQUIRE( strcmp( entry.m_infoText, "Pass the filtered = 44" ) == 0 );
        REQUIRE( entry.m_timestamp >= now );
    }

    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}
