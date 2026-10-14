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
#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include "Kit/Time/BootTime.h"
#include "catch2/catch_test_macros.hpp"
#include <string.h>


using namespace Kit::Logging::Framework;

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "EntryData" )
{
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();

    EntryData_T src;
    EntryData_T dst;
    uint8_t     buffer[EntryData_T::entryLen];

    SECTION( "default-constructed" )
    {
        REQUIRE( src.m_timestamp == 0 );
        REQUIRE( src.m_classificationId == 0 );
        REQUIRE( src.m_packageId == 0 );
        REQUIRE( src.m_subSystemId == 0 );
        REQUIRE( src.m_messageId == 0 );
        REQUIRE( src.m_applicationState == 0 );
        REQUIRE( src.m_infoText[0] == '\0' );
        REQUIRE( src.getMaxPayloadSize() == EntryData_T::entryLen );
    }

    SECTION( "round-trip" )
    {
        src.m_timestamp        = Kit::Time::constructBootTime( 7, 1672531199123 );
        src.m_classificationId = 3;
        src.m_packageId        = 1;
        src.m_subSystemId      = 2;
        src.m_messageId        = 4;
        src.m_applicationState = 0x81;
        strcpy( src.m_infoText, "round trip" );

        REQUIRE( src.copyTo( buffer, sizeof( buffer ) ) == EntryData_T::entryLen );
        REQUIRE( dst.copyFrom( buffer, sizeof( buffer ) ) );
        REQUIRE( dst.m_timestamp == src.m_timestamp );
        REQUIRE( dst.m_classificationId == src.m_classificationId );
        REQUIRE( dst.m_packageId == src.m_packageId );
        REQUIRE( dst.m_subSystemId == src.m_subSystemId );
        REQUIRE( dst.m_messageId == src.m_messageId );
        REQUIRE( dst.m_applicationState == src.m_applicationState );
        REQUIRE( strcmp( dst.m_infoText, src.m_infoText ) == 0 );
    }

    SECTION( "buffer-too-small" )
    {
        REQUIRE( src.copyTo( buffer, EntryData_T::entryLen - 1 ) == 0 );
        REQUIRE( dst.copyFrom( buffer, EntryData_T::entryLen - 1 ) == false );
    }

    SECTION( "wire-format" )
    {
        // Pins the on-media layout. Changing any offset invalidates previously persisted entries
        constexpr unsigned stateOffset = 12;
        constexpr unsigned stateLen    = sizeof( KitLoggingApplicationState_T );
        REQUIRE( EntryData_T::entryLen == stateOffset + stateLen + OPTION_KIT_LOGGING_FRAMEWORK_MAX_MSG_TEXT_LEN + 1 );

        memset( buffer, 0, sizeof( buffer ) );
        src.m_timestamp        = 0x0102030405060708ULL;
        src.m_classificationId = 0x11;
        src.m_packageId        = 0x22;
        src.m_subSystemId      = 0x33;
        src.m_messageId        = 0x44;
        src.m_applicationState = 0x55;
        strcpy( src.m_infoText, "ab" );
        REQUIRE( src.copyTo( buffer, sizeof( buffer ) ) == EntryData_T::entryLen );

        // Timestamp occupies bytes [0-7] using the configured media byte ordering
#if OPTION_KIT_PERSISTENCE_MEDIA_LITTLE_ENDIAN > 0
        REQUIRE( buffer[0] == 0x08 );
        REQUIRE( buffer[7] == 0x01 );
#else
        REQUIRE( buffer[0] == 0x01 );
        REQUIRE( buffer[7] == 0x08 );
#endif
        REQUIRE( buffer[8] == 0x11 );
        REQUIRE( buffer[9] == 0x22 );
        REQUIRE( buffer[10] == 0x33 );
        REQUIRE( buffer[11] == 0x44 );

        // Application state sits between the message ID and the info text
        KIT_PERSISTENCE_MEDIA_CURSOR check( buffer, sizeof( buffer ) );
        REQUIRE( check.setPosition( stateOffset ) );
        KitLoggingApplicationState_T stateFromWire = 0;
        REQUIRE( check.read( stateFromWire ) );
        REQUIRE( stateFromWire == 0x55 );
        REQUIRE( buffer[stateOffset + stateLen] == 'a' );
        REQUIRE( buffer[stateOffset + stateLen + 1] == 'b' );
    }

    SECTION( "widened-application-state" )
    {
        // KitLoggingApplicationState_T can be LConfig'd wider than a byte -->verify the
        // cursor's multi-byte path matches the configured media byte ordering
        uint8_t                      wire[4] = { 0, 0, 0, 0 };
        KIT_PERSISTENCE_MEDIA_CURSOR writeCursor( wire, sizeof( wire ) );
        REQUIRE( writeCursor.write( static_cast<uint16_t>( 0xA1B2 ) ) );
#if OPTION_KIT_PERSISTENCE_MEDIA_LITTLE_ENDIAN > 0
        REQUIRE( wire[0] == 0xB2 );
        REQUIRE( wire[1] == 0xA1 );
#else
        REQUIRE( wire[0] == 0xA1 );
        REQUIRE( wire[1] == 0xB2 );
#endif

        uint16_t                     readBack = 0;
        KIT_PERSISTENCE_MEDIA_CURSOR readCursor( wire, sizeof( wire ) );
        REQUIRE( readCursor.read( readBack ) );
        REQUIRE( readBack == 0xA1B2 );
    }

    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}
