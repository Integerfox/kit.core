/*-----------------------------------------------------------------------------
* COPYRIGHT_HEADER_TO_BE_FILLED_LATER
*----------------------------------------------------------------------------*/

#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include "catch2/catch_test_macros.hpp"
#include "Kit/Math/integer.h"
#include "Kit/System/Trace.h"



#define SECT_     "_0test"

/// 
using namespace Kit::Math;

#define NUM_ELEMENTS        12

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "largeInteger-big" )
{
    KIT_SYSTEM_TRACE_FUNC( SECT_ );
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();


    // Increment
    {
        uint8_t apple[NUM_ELEMENTS] = { 0,0, 0,0, 0,0, 0,0, 0,0, 0,0 };
        const uint8_t expectedValues[NUM_ELEMENTS] = { 0,0, 0,0, 0,0, 0,0, 0,0, 0,1 };
        incrementLargeInteger( apple, sizeof( apple ), 1, false );
        REQUIRE( memcmp( apple, expectedValues, NUM_ELEMENTS ) == 0 );
    }

    // Increment
    {
        uint8_t apple[NUM_ELEMENTS] = { 0,0, 0,0, 0,0, 0,0, 0,0, 0,1 };
        const uint8_t expectedValues[NUM_ELEMENTS] = { 0,0, 0,0, 0,0, 0,0, 0,0, 0,2 };
        incrementLargeInteger( apple, sizeof( apple ), 1, false );
        REQUIRE( memcmp( apple, expectedValues, NUM_ELEMENTS ) == 0 );
    }

    // Increment
    {
        uint8_t apple[NUM_ELEMENTS] = { 0,0xFF, 0xFF,0xFF, 0xFF,0xFF, 0xFF,0xFF, 0xFF,0xFF, 0xFF, 1 };
        const uint8_t expectedValues[NUM_ELEMENTS] = { 1,0, 0,0, 0,0, 0,0, 0,0, 0,0 };
        incrementLargeInteger( apple, sizeof( apple ), 0xFF, false );
        REQUIRE( memcmp( apple, expectedValues, NUM_ELEMENTS ) == 0 );
    }

    // Increment
    {
        uint8_t apple[NUM_ELEMENTS] = { 0,0, 0,0, 0,0, 0,0, 0,0, 1,0 };
        const uint8_t expectedValues[NUM_ELEMENTS] = { 0,0, 0,0, 0,0, 0,0, 0,0, 1,0xF };
        incrementLargeInteger( apple, sizeof( apple ), 0xF, false );
        REQUIRE( memcmp( apple, expectedValues, NUM_ELEMENTS ) == 0 );
    }

    // Increment
    {
        uint8_t apple[NUM_ELEMENTS] = { 0xFF,0xFF, 0xFF,0xFF, 0xFF,0xFF, 0xFF,0xFF, 0xFF,0xFF, 0xFF, 2 };
        const uint8_t expectedValues[NUM_ELEMENTS] = { 0,0, 0,0, 0,0, 0,0, 0,0, 0,0 };
        incrementLargeInteger( apple, sizeof( apple ), 0xFe, false );
        REQUIRE( memcmp( apple, expectedValues, NUM_ELEMENTS ) == 0 );
    }

    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}

TEST_CASE( "largeInteger-little" )
{
    KIT_SYSTEM_TRACE_FUNC( SECT_ );
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();


    // Increment
    {
        uint8_t apple[NUM_ELEMENTS] = { 0,0, 0,0, 0,0, 0,0, 0,0, 0,0 };
        const uint8_t expectedValues[NUM_ELEMENTS] = { 1,0, 0,0, 0,0, 0,0, 0,0, 0,0 };
        incrementLargeInteger( apple, sizeof( apple ), 1 );
        REQUIRE( memcmp( apple, expectedValues, NUM_ELEMENTS ) == 0 );
    }

    // Increment
    {
        uint8_t apple[NUM_ELEMENTS] = { 1,0, 0,0, 0,0, 0,0, 0,0, 0,0 };
        const uint8_t expectedValues[NUM_ELEMENTS] = { 2,0, 0,0, 0,0, 0,0, 0,0, 0,0 };
        incrementLargeInteger( apple, sizeof( apple ), 1 );
        REQUIRE( memcmp( apple, expectedValues, NUM_ELEMENTS ) == 0 );
    }

    // Increment
    {
        uint8_t apple[NUM_ELEMENTS] = { 1,0xFF, 0xFF,0xFF, 0xFF,0xFF, 0xFF,0xFF, 0xFF,0xFF, 0xFF, 0 };
        const uint8_t expectedValues[NUM_ELEMENTS] = { 0,0, 0,0, 0,0, 0,0, 0,0, 0,1 };
        incrementLargeInteger( apple, sizeof( apple ), 0xFF );
        REQUIRE( memcmp( apple, expectedValues, NUM_ELEMENTS ) == 0 );
    }

    // Increment
    {
        uint8_t apple[NUM_ELEMENTS] = { 0,1, 0,0, 0,0, 0,0, 0,0, 0,0 };
        const uint8_t expectedValues[NUM_ELEMENTS] = { 0xF,1, 0,0, 0,0, 0,0, 0,0, 0,0};
        incrementLargeInteger( apple, sizeof( apple ), 0xF  );
        REQUIRE( memcmp( apple, expectedValues, NUM_ELEMENTS ) == 0 );
    }

    // Increment
    {
        uint8_t apple[NUM_ELEMENTS] = { 2,0xFF, 0xFF,0xFF, 0xFF,0xFF, 0xFF,0xFF, 0xFF,0xFF, 0xFF, 0xFF };
        const uint8_t expectedValues[NUM_ELEMENTS] = { 0,0, 0,0, 0,0, 0,0, 0,0, 0,0 };
        incrementLargeInteger( apple, sizeof( apple ), 0xFe );
        REQUIRE( memcmp( apple, expectedValues, NUM_ELEMENTS ) == 0 );
    }

    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}