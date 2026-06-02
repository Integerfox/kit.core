/*-----------------------------------------------------------------------------
* COPYRIGHT_HEADER_TO_BE_FILLED_LATER
*----------------------------------------------------------------------------*/

#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include "catch2/catch_test_macros.hpp"
#include "Kit/Json/ObjectDetector.h"


using namespace Kit::Json;


TEST_CASE( "ObjectDetector-detects-object-across-calls" )
{
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();

    ObjectDetector detector;
    size_t         startOffset = 0;
    size_t         endOffset   = 0;

    const char firstChunk[]  = "noise {\"msg\": \"hel";
    const char secondChunk[] = "lo {ignored}\", \"value\": 7}";

    REQUIRE( detector.scan( firstChunk, sizeof( firstChunk ) - 1, startOffset, endOffset ) == false );
    REQUIRE( detector.scan( secondChunk, sizeof( secondChunk ) - 1, startOffset, endOffset ) == true );
    REQUIRE( startOffset == 6u );
    REQUIRE( endOffset == 43u );

    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}


TEST_CASE( "ObjectDetector-found-state-persists-on-empty-scan" )
{
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();

    ObjectDetector detector;
    size_t         startOffset = 99;
    size_t         endOffset   = 99;
    const char     payload[]   = "{\"a\":1}";

    REQUIRE( detector.scan( payload, sizeof( payload ) - 1, startOffset, endOffset ) == true );
    REQUIRE( startOffset == 0u );
    REQUIRE( endOffset == 6u );

    startOffset = 99;
    endOffset   = 99;
    REQUIRE( detector.scan( payload, 0, startOffset, endOffset ) == true );
    REQUIRE( startOffset == 0u );
    REQUIRE( endOffset == 0u );

    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}