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

    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}
