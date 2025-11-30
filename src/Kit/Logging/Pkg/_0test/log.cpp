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
#include "Kit/Logging/Pkg/Package.h"
#include "Kit/Text/FString.h"
#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include "catch2/catch_test_macros.hpp"


using namespace Kit::Logging::Pkg;

#define SECT_ "_0test"

static Kit::Logging::Framework::LogResult_T result_ = Kit::Logging::Framework::LogResult_T::QUEUE_FULL;
static uint8_t                              lastClassificationId_;
static uint8_t                              lastPackageId_;
static uint8_t                              lastSubSystemId_;
static uint8_t                              lastMessageId_;
static Kit::Text::FString<256>              lastInfoText_;

// Mocked vlogf implementation
Kit::Logging::Framework::LogResult_T Kit::Logging::Framework::vlogf( uint8_t     classificationId,
                                                                     uint8_t     packageId,
                                                                     uint8_t     subSystemId,
                                                                     uint8_t     messageId,
                                                                     const char* formatInfoText,
                                                                     va_list     ap ) noexcept
{
    lastClassificationId_ = classificationId;
    lastPackageId_        = packageId;
    lastSubSystemId_      = subSystemId;
    lastMessageId_        = messageId;
    lastInfoText_.vformat( formatInfoText, ap );
    
    return result_;
}

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Log" )
{
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();
    Package uut;

    SECTION( "System" )
    {
        auto result = Kit::Logging::Pkg::logfSystem( ClassificationId::EVENT, SystemMsgId::SHUTDOWN, "System log message: %u", 123 );
        REQUIRE( result == result_ );
        REQUIRE( lastClassificationId_ == ClassificationId::EVENT );
        REQUIRE( lastPackageId_ == Package::PACKAGE_ID );
        REQUIRE( lastSubSystemId_ == SubSystemId::SYSTEM );
        REQUIRE( lastMessageId_ == SystemMsgId::SHUTDOWN );
        REQUIRE( strcmp( lastInfoText_.getString(), "System log message: 123" ) == 0 );
    }

    SECTION( "Driver" )
    {
        auto result = Kit::Logging::Pkg::logfDriver( ClassificationId::INFO, DriverMsgId::ERROR, "Driver log message: %s", "Hello" );
        REQUIRE( result == result_ );
        REQUIRE( lastClassificationId_ == ClassificationId::INFO );
        REQUIRE( lastPackageId_ == Package::PACKAGE_ID );
        REQUIRE( lastSubSystemId_ == SubSystemId::DRIVER );
        REQUIRE( lastMessageId_ == DriverMsgId::ERROR );
        REQUIRE( strcmp( lastInfoText_.getString(), "Driver log message: Hello" ) == 0 );
    }

    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}
