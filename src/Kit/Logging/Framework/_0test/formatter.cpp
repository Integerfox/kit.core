/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Logging/Framework/Formatter.h"
#include "Kit/Logging/Framework/IPackage.h"
#include "Kit/Logging/Pkg/Log.h"
#include "Kit/Time/BootTime.h"
#include "Kit/System/Trace.h"
#include "Kit/Text/FString.h"
#include "Kit/Logging/Framework/Mocked4Test/KitOnly.h"
#include "Kit/Logging/Framework/_0test/StatefulApp.h"
#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include "catch2/catch_test_macros.hpp"
#include <string.h>


using namespace Kit::Logging::Pkg;
using namespace Kit::Logging::Framework;
using namespace Kit::Logging::Framework::Mocked4Test;

#define SECT_ "_0test"

// Use the same static instance as the Logger test to avoid pointer corruption
extern KitOnly logApp_;

namespace {

/// Renders every field at its configured maximum length - i.e. the worst case for OPTION_KIT_LOGGING_FORMATTER_MAX_TEXT_LEN
class MaxLenApp : public Kit::Logging::Framework::IApplication,
                  public Kit::Logging::Framework::IPackage
{
public:
    MaxLenApp() noexcept
    {
        fillMax( m_stateText, sizeof( m_stateText ) );
        fillMax( m_classText, sizeof( m_classText ) );
        fillMax( m_pkgText, sizeof( m_pkgText ) );
        fillMax( m_subSysText, sizeof( m_subSysText ) );
        fillMax( m_msgText, sizeof( m_msgText ) );
    }

public:
    bool isClassificationIdValid( uint8_t ) noexcept override { return true; }
    const char* classificationIdToString( uint8_t ) noexcept override { return m_classText; }
    Kit::Logging::Framework::IPackage* getPackage( uint8_t ) noexcept override { return this; }
    const char* applicationStateToString( KitLoggingApplicationState_T ) noexcept override { return m_stateText; }

public:
    uint8_t packageId() noexcept override { return Package::PACKAGE_ID; }
    const char* packageIdString() noexcept override { return m_pkgText; }
    bool subSystemAndMessageIdsToString( uint8_t, const char*& dstSubSystemText, uint8_t, const char*& dstMessageText ) noexcept override
    {
        dstSubSystemText = m_subSysText;
        dstMessageText   = m_msgText;
        return true;
    }

protected:
    static void fillMax( char* dst, size_t sizeofDst ) noexcept
    {
        memset( dst, 'M', sizeofDst - 1 );
        dst[sizeofDst - 1] = '\0';
    }

    char m_stateText[OPTION_KIT_LOGGING_FORMATTER_APPLICATION_STATE_TEXT_LEN + 1];
    char m_classText[OPTION_KIT_LOGGING_FRAMEWORK_MAX_LEN_CLASSIFICATION_ID_TEXT + 1];
    char m_pkgText[OPTION_KIT_LOGGING_FRAMEWORK_MAX_LEN_PACKAGE_ID_TEXT + 1];
    char m_subSysText[OPTION_KIT_LOGGING_FRAMEWORK_MAX_LEN_SUBSYSTEM_ID_TEXT + 1];
    char m_msgText[OPTION_KIT_LOGGING_FRAMEWORK_MAX_LEN_MESSAGE_ID_TEXT + 1];
};

/// Returns a state marker that exceeds OPTION_KIT_LOGGING_FORMATTER_APPLICATION_STATE_TEXT_LEN
class LongMarkerApp : public StatefulApp4Test
{
public:
    LongMarkerApp( Kit::Logging::Framework::IApplication& delegate ) noexcept
        : StatefulApp4Test( delegate )
    {
    }

    const char* applicationStateToString( KitLoggingApplicationState_T ) noexcept override { return "0123456789"; }
};

}  // end anonymous namespace

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Formatter" )
{
    KIT_SYSTEM_TRACE_SCOPE( SECT_ , "Formatter" );
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();
    logApp_.reset();
    EntryData_T                                                   logEntry;
    Kit::Text::FString<OPTION_KIT_LOGGING_FORMATTER_MAX_TEXT_LEN> formattedText;

    SECTION( "basic" )
    {
        logEntry.m_timestamp        = Kit::Time::constructBootTime( 5, 1672531199123 );  // 2022/12/31-23:59:59.123 UTC
        logEntry.m_classificationId = ClassificationId::WARNING;
        logEntry.m_packageId        = Package::PACKAGE_ID;
        logEntry.m_subSystemId      = SubSystemId::SYSTEM;
        logEntry.m_messageId        = SystemMsgId::SHUTDOWN;
        strcpy( logEntry.m_infoText, "Test log message" );

        bool result = Formatter::toString( logApp_, logEntry, formattedText );
        KIT_SYSTEM_TRACE_MSG( SECT_, "%s", formattedText.getString() );
        REQUIRE( result == true );
        REQUIRE( formattedText == "(5:2022-12-31 23:59:59.123) WARNING-KIT-SYSTEM-SHUTDOWN: Test log message" );

        logEntry.m_timestamp        = Kit::Time::constructBootTime( 60000, 1672531199123 );  // 2022/12/31-23:59:59.123 UTC
        logEntry.m_classificationId = ClassificationId::INFO;
        logEntry.m_packageId        = Package::PACKAGE_ID;
        logEntry.m_subSystemId      = SubSystemId::DRIVER;
        logEntry.m_messageId        = DriverMsgId::STOP_ERR;
        strcpy( logEntry.m_infoText, "With Persistent Storage ID log message" );

        result = Formatter::toString( logApp_, logEntry, formattedText, 0x1C );
        KIT_SYSTEM_TRACE_MSG( SECT_, "%s", formattedText.getString() );
        REQUIRE( result == true );
        REQUIRE( formattedText == "[1C] (60000:2022-12-31 23:59:59.123) INFO-KIT-DRIVER-STOP_ERR: With Persistent Storage ID log message" );
    }

    SECTION( "errors" )
    {
        logEntry.m_timestamp        = Kit::Time::constructBootTime( 5, 1672531199123 );  // 2022/12/31-23:59:59.123 UTC
        logEntry.m_classificationId = ClassificationId::WARNING;
        logEntry.m_packageId        = Package::PACKAGE_ID;
        logEntry.m_subSystemId      = IPackage::NULL_SUBSYS_ID;
        logEntry.m_messageId        = SystemMsgId::SHUTDOWN;
        strcpy( logEntry.m_infoText, "Test log message2" );

        bool result = Formatter::toString( logApp_, logEntry, formattedText );
        KIT_SYSTEM_TRACE_MSG( SECT_, "%s", formattedText.getString() );
        REQUIRE( result == true );
        REQUIRE( formattedText == "(5:2022-12-31 23:59:59.123) WARNING-KIT-?-?: Test log message2" );

        logEntry.m_timestamp        = Kit::Time::constructBootTime( 5, 1672531199123 );  // 2022/12/31-23:59:59.123 UTC
        logEntry.m_classificationId = ClassificationId::WARNING;
        logEntry.m_packageId        = Package::PACKAGE_ID;
        logEntry.m_subSystemId      = SubSystemId::SYSTEM;
        logEntry.m_messageId        = IPackage::NULL_MSG_ID;
        strcpy( logEntry.m_infoText, "Test log message3" );

        result = Formatter::toString( logApp_, logEntry, formattedText );
        KIT_SYSTEM_TRACE_MSG( SECT_, "%s", formattedText.getString() );
        REQUIRE( result == true );
        REQUIRE( formattedText == "(5:2022-12-31 23:59:59.123) WARNING-KIT-SYSTEM-?: Test log message3" );
    }

    SECTION( "application-state" )
    {
        StatefulApp4Test stateApp( logApp_ );

        logEntry.m_timestamp        = Kit::Time::constructBootTime( 5, 1672531199123 );  // 2022/12/31-23:59:59.123 UTC
        logEntry.m_classificationId = ClassificationId::WARNING;
        logEntry.m_packageId        = Package::PACKAGE_ID;
        logEntry.m_subSystemId      = SubSystemId::SYSTEM;
        logEntry.m_messageId        = SystemMsgId::SHUTDOWN;
        logEntry.m_applicationState = StatefulApp4Test::STATE_TAINTED;
        strcpy( logEntry.m_infoText, "Test log message" );

        bool result = Formatter::toString( stateApp, logEntry, formattedText );
        KIT_SYSTEM_TRACE_MSG( SECT_, "%s", formattedText.getString() );
        REQUIRE( result == true );
        REQUIRE( formattedText == "(5:2022-12-31 23:59:59.123) *WARNING-KIT-SYSTEM-SHUTDOWN: Test log message" );

        // No state -->no marker
        logEntry.m_applicationState = 0;
        result                      = Formatter::toString( stateApp, logEntry, formattedText );
        KIT_SYSTEM_TRACE_MSG( SECT_, "%s", formattedText.getString() );
        REQUIRE( result == true );
        REQUIRE( formattedText == "(5:2022-12-31 23:59:59.123) WARNING-KIT-SYSTEM-SHUTDOWN: Test log message" );

        // An application that does not opt-in never renders a marker
        logEntry.m_applicationState = StatefulApp4Test::STATE_TAINTED;
        result                      = Formatter::toString( logApp_, logEntry, formattedText );
        KIT_SYSTEM_TRACE_MSG( SECT_, "%s", formattedText.getString() );
        REQUIRE( result == true );
        REQUIRE( formattedText == "(5:2022-12-31 23:59:59.123) WARNING-KIT-SYSTEM-SHUTDOWN: Test log message" );
    }

    SECTION( "application-state-text-truncated" )
    {
        // A non-conforming IApplication cannot exceed its text budget
        LongMarkerApp longApp( logApp_ );

        logEntry.m_timestamp        = Kit::Time::constructBootTime( 5, 1672531199123 );  // 2022/12/31-23:59:59.123 UTC
        logEntry.m_classificationId = ClassificationId::WARNING;
        logEntry.m_packageId        = Package::PACKAGE_ID;
        logEntry.m_subSystemId      = SubSystemId::SYSTEM;
        logEntry.m_messageId        = SystemMsgId::SHUTDOWN;
        logEntry.m_applicationState = StatefulApp4Test::STATE_TAINTED;
        strcpy( logEntry.m_infoText, "Test log message" );

        bool result = Formatter::toString( longApp, logEntry, formattedText );
        KIT_SYSTEM_TRACE_MSG( SECT_, "%s", formattedText.getString() );
        REQUIRE( result == true );
        REQUIRE( formattedText == "(5:2022-12-31 23:59:59.123) 0WARNING-KIT-SYSTEM-SHUTDOWN: Test log message" );
    }

    SECTION( "max-formatted-length" )
    {
        // Every field - including the storage ID - at its configured maximum length
        MaxLenApp maxApp;

        logEntry.m_timestamp        = Kit::Time::constructBootTime( 65535, 1672531199123 );
        logEntry.m_classificationId = ClassificationId::WARNING;
        logEntry.m_packageId        = Package::PACKAGE_ID;
        logEntry.m_subSystemId      = SubSystemId::SYSTEM;
        logEntry.m_messageId        = SystemMsgId::SHUTDOWN;
        logEntry.m_applicationState = StatefulApp4Test::STATE_TAINTED;
        memset( logEntry.m_infoText, 'X', OPTION_KIT_LOGGING_FRAMEWORK_MAX_MSG_TEXT_LEN );
        logEntry.m_infoText[OPTION_KIT_LOGGING_FRAMEWORK_MAX_MSG_TEXT_LEN] = '\0';

        // 16 hex digits -->the widest a uint64_t storage ID can render
        bool result = Formatter::toString( maxApp, logEntry, formattedText, 0xFEDCBA9876543210ULL );
        KIT_SYSTEM_TRACE_MSG( SECT_, "%s", formattedText.getString() );
        REQUIRE( result == true );

        // The '-1' is the single byte of headroom in the budget: the storage ID allotment
        // includes a field separator that the '[<hex>] ' rendering does not need
        REQUIRE( formattedText.length() == OPTION_KIT_LOGGING_FORMATTER_MAX_TEXT_LEN - 1 );
    }

    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}
