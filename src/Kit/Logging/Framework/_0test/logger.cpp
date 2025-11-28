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
#include "Kit/Logging/Framework/IApplication.h"
#include "Kit/Logging/Framework/IPackage.h"
#include "Kit/Logging/Framework/Log.h"
#include "Kit/Logging/Pkg/Log.h"
#include "Kit/Logging/Framework/Mocked4Test/KitOnly.h"
#include "Kit/Logging/Framework/types.h"
#include "Kit/System/Trace.h"
#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include "Kit/Time/BootTime.h"
#include "Kit/System/Api.h"
#include "catch2/catch_test_macros.hpp"


using namespace Kit::Logging::Framework;
using namespace Kit::Logging::Pkg;

#define SECT_                      "_0test"

#define MY_FATAL                   Kit::Logging::Pkg::ClassificationId::FATAL
#define MY_WARNING                 Kit::Logging::Pkg::ClassificationId::WARNING
#define MY_PACKAGE_ID              Kit::Logging::Pkg::Package::PACKAGE_ID
#define MY_EVENT                   Kit::Logging::Pkg::ClassificationId::EVENT
#define MY_INFO                    Kit::Logging::Pkg::ClassificationId::INFO

#define OVERFLOW_CLASSIFICATION_ID Kit::Logging::Pkg::ClassificationId::WARNING
#define OVERFLOW_PACKAGE_ID        MY_PACKAGE_ID
#define OVERFLOW_SUBSYSTEM_ID      Kit::Logging::Pkg::SubSystemId::SYSTEM
#define OVERFLOW_MESSAGE_ID        Kit::Logging::Pkg::SystemMsgId::LOGGING


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
        LogResult_T result = logf( MY_EVENT, MY_PACKAGE_ID, 0, 0, "should be (classification) filtered = %u", arg );

        REQUIRE( result == LogResult_T::FILTERED );
        REQUIRE( logApp_.getLogQueueCount() == 0u );
        REQUIRE( logApp_.m_logFifo.remove( entry ) == false );

        Kit::Logging::Framework::enableClassification( eventMask );
        result = logf( MY_EVENT, MY_PACKAGE_ID, 0, 0, "Pass the classification filter = %u", ++arg );
        REQUIRE( result == LogResult_T::ADDED );
        REQUIRE( logApp_.getLogQueueCount() == 1u );
        REQUIRE( logApp_.m_logFifo.remove( entry ) );
        REQUIRE( entry.m_classificationId == MY_EVENT );
        REQUIRE( entry.m_packageId == MY_PACKAGE_ID );
        REQUIRE( entry.m_subSystemId == 0u );
        REQUIRE( entry.m_messageId == 0u );
        REQUIRE( strcmp( entry.m_infoText, "Pass the classification filter = 44" ) == 0 );
        REQUIRE( entry.m_timestamp >= now );

        logApp_.reset();
        setClassificationMask( eventMask );
        result = logf( MY_EVENT, MY_PACKAGE_ID, 0, 0, "Pass the classification filter = %u", ++arg );
        REQUIRE( result == LogResult_T::ADDED );
        REQUIRE( logApp_.getLogQueueCount() == 1u );
        REQUIRE( logApp_.m_logFifo.remove( entry ) );
        REQUIRE( getClassificationEnabledMask() == eventMask );

        setClassificationMask( eventMask << 1 );
        result = logf( MY_EVENT, MY_PACKAGE_ID, 0, 0, "should be (classification) filtered = %u", ++arg );
        REQUIRE( result == LogResult_T::FILTERED );
        REQUIRE( logApp_.getLogQueueCount() == 0u );
        REQUIRE( logApp_.m_logFifo.remove( entry ) == false );
        REQUIRE( getClassificationEnabledMask() == eventMask << 1 );
    }

    SECTION( "package-filter" )
    {
        REQUIRE( logApp_.getLogQueueCount() == 0 );
        REQUIRE( logApp_.getVLogfCallCount() == 0 );
        KitLoggingClassificationMask_T pkgMask = packageIdToMask( MY_PACKAGE_ID );
        Kit::Logging::Framework::disablePackage( pkgMask );

        unsigned    arg    = 44;
        LogResult_T result = logf( MY_EVENT, MY_PACKAGE_ID, 0, 0, "should be (package) filtered = %u", arg );
        REQUIRE( logApp_.getVLogfCallCount() == 1u );
        REQUIRE( result == LogResult_T::FILTERED );
        REQUIRE( logApp_.getLogQueueCount() == 0u );
        REQUIRE( logApp_.m_logFifo.remove( entry ) == false );

        Kit::Logging::Framework::enablePackage( pkgMask );
        result = logf( MY_EVENT, MY_PACKAGE_ID, 0, 0, "Pass the package filter = %u", ++arg );
        REQUIRE( logApp_.getVLogfCallCount() == 2u );
        REQUIRE( result == LogResult_T::ADDED );
        REQUIRE( logApp_.getLogQueueCount() == 1u );
        REQUIRE( logApp_.m_logFifo.remove( entry ) );
        REQUIRE( entry.m_classificationId == MY_EVENT );
        REQUIRE( entry.m_packageId == MY_PACKAGE_ID );
        REQUIRE( entry.m_subSystemId == 0u );
        REQUIRE( entry.m_messageId == 0u );
        REQUIRE( strcmp( entry.m_infoText, "Pass the package filter = 45" ) == 0 );
        REQUIRE( entry.m_timestamp >= now );

        logApp_.reset();
        REQUIRE( logApp_.getVLogfCallCount() == 0u );
        setPackageMask( pkgMask );
        result = logf( MY_EVENT, MY_PACKAGE_ID, 0, 0, "Pass the package filter = %u", ++arg );
        REQUIRE( logApp_.getVLogfCallCount() == 1u );
        REQUIRE( result == LogResult_T::ADDED );
        REQUIRE( logApp_.getLogQueueCount() == 1u );
        REQUIRE( logApp_.m_logFifo.remove( entry ) );
        REQUIRE( getPackageEnabledMask() == pkgMask );
        setPackageMask( pkgMask << 1 );
        result = logf( MY_EVENT, MY_PACKAGE_ID, 0, 0, "should be (package) filtered = %u", ++arg );
        REQUIRE( logApp_.getVLogfCallCount() == 2u );
        REQUIRE( result == LogResult_T::FILTERED );
        REQUIRE( logApp_.getLogQueueCount() == 0u );
        REQUIRE( logApp_.m_logFifo.remove( entry ) == false );
        REQUIRE( getPackageEnabledMask() == pkgMask << 1 );
    }

    SECTION( "vlogf-errors" )
    {
        REQUIRE( logApp_.getLogQueueCount() == 0 );

        unsigned    arg    = 420;
        LogResult_T result = logf( MY_EVENT, 200, 0, 0, "My message = %u", arg );
        REQUIRE( result == LogResult_T::FILTERED );
        REQUIRE( logApp_.getLogQueueCount() == 0 );
        REQUIRE( logApp_.m_logFifo.remove( entry ) == false );

        result = logf( 210, MY_PACKAGE_ID, 0, 0, "My message = %u", ++arg );
        REQUIRE( result == LogResult_T::FILTERED );
        REQUIRE( logApp_.getLogQueueCount() == 0 );
        REQUIRE( logApp_.m_logFifo.remove( entry ) == false );

        result = logf( 0, MY_PACKAGE_ID, 0, 0, "My message = %u", ++arg );
        REQUIRE( result == LogResult_T::FILTERED );
        REQUIRE( logApp_.getLogQueueCount() == 0 );
        REQUIRE( logApp_.m_logFifo.remove( entry ) == false );

        result = logf( MY_EVENT, 100, 0, 0, "My message = %u", ++arg );
        REQUIRE( result == LogResult_T::FILTERED );
        REQUIRE( logApp_.getLogQueueCount() == 0 );
        REQUIRE( logApp_.m_logFifo.remove( entry ) == false );

        result = logf( MY_EVENT, 0, 0, 0, "My message = %u", ++arg );
        REQUIRE( result == LogResult_T::FILTERED );
        REQUIRE( logApp_.getLogQueueCount() == 0 );
        REQUIRE( logApp_.m_logFifo.remove( entry ) == false );
    }

    SECTION( "overflow" )
    {
        Kit::System::sleep( 10 );  // no-zero time stamp

        REQUIRE( logApp_.getLogQueueCount() == 0 );

        // Fill the log queue
        for ( unsigned i = 0; i < OPTION_KIT_LOGGING_FRAMEWORK_MOCKED4TEST_MAX_LOG_ENTRIES; ++i )
        {
            unsigned    arg    = i;
            LogResult_T result = logf( MY_INFO, MY_PACKAGE_ID, 0, 0, "Filling log queue message = %u", arg );
            REQUIRE( result == LogResult_T::ADDED );
        }
        REQUIRE( logApp_.getLogQueueCount() == OPTION_KIT_LOGGING_FRAMEWORK_MOCKED4TEST_MAX_LOG_ENTRIES );

        // Now try to add one more entry - should cause overflow
        unsigned    arg    = 9999;
        LogResult_T result = logf( MY_INFO, MY_PACKAGE_ID, 0, 0, "This message should cause overflow = %u", arg );
        REQUIRE( result == LogResult_T::QUEUE_FULL );
        REQUIRE( logApp_.isLogQueOverflowed() == true );
        REQUIRE( logApp_.getOverflowedLogEntryCount() == 1u );

        // Remove all entries
        unsigned removedCount = 0;
        while ( logApp_.m_logFifo.remove( entry ) )
        {
            removedCount++;
        }
        REQUIRE( removedCount == OPTION_KIT_LOGGING_FRAMEWORK_MOCKED4TEST_MAX_LOG_ENTRIES );

        // Now add another entry - should succeed and clear overflow state
        now = Kit::Time::getBootTime();
        Kit::System::sleep( 10 );  // ensure the log entry time stamp != overflow timestamp
        result = logf( MY_INFO, MY_PACKAGE_ID, 0, 0, "Post-overflow message = %u", ++arg );
        REQUIRE( result == LogResult_T::ADDED );
        REQUIRE( logApp_.isLogQueOverflowed() == false );
        REQUIRE( logApp_.getOverflowedLogEntryCount() == 0u );
        REQUIRE( logApp_.getLogQueueCount() == 2u );
        REQUIRE( logApp_.m_logFifo.remove( entry ) );
        REQUIRE( entry.m_classificationId == OVERFLOW_CLASSIFICATION_ID );
        REQUIRE( entry.m_packageId == OVERFLOW_PACKAGE_ID );
        REQUIRE( entry.m_subSystemId == OVERFLOW_SUBSYSTEM_ID );
        REQUIRE( entry.m_messageId == OVERFLOW_MESSAGE_ID );
#define OVERFLOW_TEXT_PREFIX "OVERFLOW! Num entries lost=1 (0:1970-01-01 00:00:0"
        REQUIRE( strncmp( entry.m_infoText, OVERFLOW_TEXT_PREFIX, strlen( OVERFLOW_TEXT_PREFIX ) ) == 0 );
        REQUIRE( entry.m_timestamp >= now );
        REQUIRE( logApp_.m_logFifo.remove( entry ) );
        REQUIRE( entry.m_classificationId == MY_INFO );
        REQUIRE( entry.m_packageId == MY_PACKAGE_ID );
        REQUIRE( entry.m_subSystemId == 0 );
        REQUIRE( entry.m_messageId == 0 );
        REQUIRE( strcmp( entry.m_infoText, "Post-overflow message = 10000" ) == 0 );
        REQUIRE( entry.m_timestamp >= now );
        REQUIRE( logApp_.getLogQueueCount() == 0 );
    }

    SECTION( "overflow-hysteresis" )
    {
        Kit::System::sleep( 10 );  // no-zero time stamp

        REQUIRE( logApp_.getLogQueueCount() == 0 );

        // Fill the log queue
        for ( unsigned i = 0; i < OPTION_KIT_LOGGING_FRAMEWORK_MOCKED4TEST_MAX_LOG_ENTRIES; ++i )
        {
            unsigned    arg    = i;
            LogResult_T result = logf( MY_INFO, MY_PACKAGE_ID, 0, 0, "Filling log queue message = %u", arg );
            REQUIRE( result == LogResult_T::ADDED );
        }
        REQUIRE( logApp_.getLogQueueCount() == OPTION_KIT_LOGGING_FRAMEWORK_MOCKED4TEST_MAX_LOG_ENTRIES );

        // Now try to add one more entry - should cause overflow
        unsigned    arg    = 9999;
        LogResult_T result = logf( MY_INFO, MY_PACKAGE_ID, 0, 0, "This message should cause overflow = %u", arg );
        REQUIRE( result == LogResult_T::QUEUE_FULL );
        REQUIRE( logApp_.isLogQueOverflowed() == true );
        REQUIRE( logApp_.getOverflowedLogEntryCount() == 1u );
        REQUIRE( logApp_.getLogQueueCount() == OPTION_KIT_LOGGING_FRAMEWORK_MOCKED4TEST_MAX_LOG_ENTRIES );

        // Remove some entries
        for ( unsigned i = 0; i < OPTION_KIT_LOGGING_FRAMEWORK_MIN_QUEUE_SPACE - 1; ++i )
        {
            REQUIRE( logApp_.m_logFifo.remove( entry ) );
        }
        REQUIRE( logApp_.getLogQueueCount() == 2 );     // Still two entries left

        // Now attempt more entries - it should fail
        auto oldNow = now;
        now         = Kit::Time::getBootTime();
        Kit::System::sleep( 10 );  // ensure the log entry time stamp != overflow timestamp
        result = logf( MY_INFO, MY_PACKAGE_ID, 0, 0, "Should be dropped = %u", ++arg );
        REQUIRE( result == LogResult_T::QUEUE_FULL );
        result = logf( MY_INFO, MY_PACKAGE_ID, 0, 0, "Should be dropped = %u", ++arg );
        REQUIRE( result == LogResult_T::QUEUE_FULL );

        // Remove one more entry to reach the hysteresis limit
        REQUIRE( logApp_.m_logFifo.remove( entry ) );
        REQUIRE( logApp_.getLogQueueCount() == 1 );     // Still one entries left

        // Now add another entry - should succeed and clear overflow state
        now = Kit::Time::getBootTime();
        Kit::System::sleep( 10 );  // ensure the log entry time stamp != overflow timestamp
        REQUIRE( logApp_.getOverflowedLogEntryCount() == 3u );
        result = logf( MY_INFO, MY_PACKAGE_ID, 0, 0, "Post-overflow message = %u", ++arg );
        REQUIRE( result == LogResult_T::ADDED );
        REQUIRE( logApp_.isLogQueOverflowed() == false );
        REQUIRE( logApp_.getOverflowedLogEntryCount() == 0u );
        REQUIRE( logApp_.getLogQueueCount() == ( 1 + 2 ) ); // 1 previous entry + overflow log entry + 1 new entry
        REQUIRE( logApp_.m_logFifo.remove( entry ) );
        REQUIRE( entry.m_classificationId == MY_INFO );
        REQUIRE( entry.m_packageId == MY_PACKAGE_ID );
        REQUIRE( entry.m_subSystemId == 0 );
        REQUIRE( entry.m_messageId == 0 );
        REQUIRE( entry.m_timestamp >= oldNow );
        REQUIRE( entry.m_timestamp < now );
        REQUIRE( logApp_.m_logFifo.remove( entry ) );
        REQUIRE( entry.m_classificationId == OVERFLOW_CLASSIFICATION_ID );
        REQUIRE( entry.m_packageId == OVERFLOW_PACKAGE_ID );
        REQUIRE( entry.m_subSystemId == OVERFLOW_SUBSYSTEM_ID );
        REQUIRE( entry.m_messageId == OVERFLOW_MESSAGE_ID );
        REQUIRE( entry.m_timestamp >= now );
        REQUIRE( logApp_.m_logFifo.remove( entry ) );
        REQUIRE( entry.m_classificationId == MY_INFO );
        REQUIRE( entry.m_packageId == MY_PACKAGE_ID );
        REQUIRE( entry.m_subSystemId == 0 );
        REQUIRE( entry.m_messageId == 0 );
        REQUIRE( entry.m_timestamp >= now );
        REQUIRE( logApp_.getLogQueueCount() == 0 );
    }

    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}
