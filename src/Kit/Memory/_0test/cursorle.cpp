/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Memory/CursorLE.h"
#include "catch2/catch_test_macros.hpp"
#include "Kit/System/Trace.h"
#include "Kit/System/_testsupport/ShutdownUnitTesting.h"

#define SECT_ "_0test"

using namespace Kit::Memory;


#define BUFFER_SIZE 40
static uint8_t bufferMemory_[BUFFER_SIZE];

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "CursorLE" )
{
    KIT_SYSTEM_TRACE_FUNC( SECT_ );
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();
    uint8_t  byte;
    uint16_t word;
    uint32_t dword;
    uint64_t qword;
    float    real32;
    double   real64;

    SECTION( "basic" )
    {
        uint8_t  array[] = { 0xAA, 0xBB, 0xCC };
        CursorLE cursor( bufferMemory_, sizeof( bufferMemory_ ) );
        REQUIRE( cursor.getRemainingCount() == BUFFER_SIZE );
        REQUIRE( cursor.errorOccurred() == false );
        REQUIRE( cursor.writeU8( 0x11 ) );
        REQUIRE( cursor.getRemainingCount() == ( BUFFER_SIZE - 1 ) );
        REQUIRE( cursor.writeU16( 0x2122 ) );
        REQUIRE( cursor.getRemainingCount() == ( BUFFER_SIZE - 1 - 2 ) );
        REQUIRE( cursor.writeU32( 0x41424344 ) );
        REQUIRE( cursor.getRemainingCount() == ( BUFFER_SIZE - 1 - 2 - 4 ) );
        REQUIRE( cursor.writeU64( 0x8182838485868788 ) );
        REQUIRE( cursor.getRemainingCount() == ( BUFFER_SIZE - 1 - 2 - 4 - 8 ) );
        REQUIRE( cursor.write( array, sizeof( array ) ) );
        REQUIRE( cursor.getRemainingCount() == ( BUFFER_SIZE - 1 - 2 - 4 - 8 - sizeof( array ) ) );
        REQUIRE( cursor.fill( 3, 0x55 ) );
        REQUIRE( cursor.getRemainingCount() == ( BUFFER_SIZE - 1 - 2 - 4 - 8 - sizeof( array ) - 3 ) );
        REQUIRE( cursor.writeF32( (float)3.14 ) );
        REQUIRE( cursor.getRemainingCount() == ( BUFFER_SIZE - 1 - 2 - 4 - 8 - sizeof( array ) - 3 - 4 ) );
        REQUIRE( cursor.writeF64( (double)3.14159 ) );
        REQUIRE( cursor.getRemainingCount() == ( BUFFER_SIZE - 1 - 2 - 4 - 8 - sizeof( array ) - 3 - 4 - 8 ) );

        uint8_t expectedVal[] = { 0x11, 0x22, 0x21, 0x44, 0x43, 0x42, 0x41, 0x88, 0x87, 0x86, 0x85, 0x84, 0x83, 0x82, 0x81, 0xAA, 0xBB, 0xCC, 0x55, 0x55, 0x55 };
        REQUIRE( memcmp( bufferMemory_, expectedVal, sizeof( expectedVal ) ) == 0 );

        cursor.setPosition( 0 );
        REQUIRE( cursor.getRemainingCount() == BUFFER_SIZE );
        uint8_t array2[3] = { 0 };
        uint8_t array3[3] = { 0 };
        REQUIRE( cursor.readU8( byte ) );
        REQUIRE( cursor.getRemainingCount() == ( BUFFER_SIZE - 1 ) );
        REQUIRE( cursor.readU16( word ) );
        REQUIRE( cursor.getRemainingCount() == ( BUFFER_SIZE - 1 - 2 ) );
        REQUIRE( cursor.readU32( dword ) );
        REQUIRE( cursor.getRemainingCount() == ( BUFFER_SIZE - 1 - 2 - 4 ) );
        REQUIRE( cursor.readU64( qword ) );
        REQUIRE( cursor.getRemainingCount() == ( BUFFER_SIZE - 1 - 2 - 4 - 8 ) );
        REQUIRE( cursor.read( array2, sizeof( array2 ) ) );
        REQUIRE( cursor.getRemainingCount() == ( BUFFER_SIZE - 1 - 2 - 4 - 8 - sizeof( array ) ) );
        REQUIRE( cursor.read( array3, sizeof( array3 ) ) );
        REQUIRE( cursor.getRemainingCount() == ( BUFFER_SIZE - 1 - 2 - 4 - 8 - sizeof( array ) - sizeof( array3 ) ) );
        REQUIRE( cursor.readF32( real32 ) );
        REQUIRE( cursor.getRemainingCount() == ( BUFFER_SIZE - 1 - 2 - 4 - 8 - sizeof( array ) - sizeof( array3 ) - 4 ) );
        REQUIRE( cursor.readF64( real64 )) ;
        REQUIRE( cursor.getRemainingCount() == ( BUFFER_SIZE - 1 - 2 - 4 - 8 - sizeof( array ) - sizeof( array3 ) - 4 - 8 ) );
        REQUIRE( byte == 0x11 );
        REQUIRE( word == 0x2122 );
        REQUIRE( dword == 0x41424344 );
        REQUIRE( qword == 0x8182838485868788 );
        REQUIRE( real32 == 3.14F );
        REQUIRE( real64 == 3.14159 );
        REQUIRE( memcmp( array, array2, sizeof( array2 ) ) == 0 );
        uint8_t expectedArray3[3] = { 0x55, 0x55, 0x55 };
        REQUIRE( memcmp( expectedArray3, array3, sizeof( expectedArray3 ) ) == 0 );
    }

    SECTION( "basic2" )
    {
        uint8_t  array[] = { 0xAA, 0xBB, 0xCC };
        CursorLE cursor( bufferMemory_, sizeof( bufferMemory_ ) );
        REQUIRE( cursor.errorOccurred() == false );
        REQUIRE( cursor.writeU8( 0xF1 ) );
        REQUIRE( cursor.writeU16( 0xE1E2 ) );
        REQUIRE( cursor.writeU32( 0xD1D2D3D4 ) );
        REQUIRE( cursor.writeU64( 0x0102030405060708 ) );
        REQUIRE( cursor.write( array, sizeof( array ) ) );
        REQUIRE( cursor.writeF32( (float)3.14 ) );
        REQUIRE( cursor.writeF64( (double)3.14159 ) );
        

        uint8_t  expectedVal[] = { 0xF1, 0xE2, 0xE1, 0xD4, 0xD3, 0xD2, 0xD1, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0xAA };
        REQUIRE( memcmp( bufferMemory_, expectedVal, sizeof( expectedVal ) ) == 0 );

        cursor.setPosition( 0 );
        uint8_t array2[3] = { 0 };
        REQUIRE( cursor.readU8( byte ) );
        REQUIRE( cursor.readU16( word ) );
        REQUIRE( cursor.readU32( dword ) );
        REQUIRE( cursor.readU64( qword ) );
        REQUIRE( cursor.read( array2, sizeof( array2 ) ) );
        REQUIRE( cursor.readF32( real32 ) );
        REQUIRE( cursor.readF64( real64 ) );
        REQUIRE( byte == 0xF1 );
        REQUIRE( word == 0xE1E2 );
        REQUIRE( dword == 0xD1D2D3D4 );
        REQUIRE( qword == 0x0102030405060708 );
        REQUIRE( real32 == 3.14F );
        REQUIRE( real64 == 3.14159 );
        REQUIRE( memcmp( array, array2, sizeof( array2 ) ) == 0 );
    }

    SECTION( "basic3" )
    {
        CursorLE cursor( bufferMemory_, sizeof( bufferMemory_ ) );
        REQUIRE( cursor.writeI8( 42 ) );
        REQUIRE( cursor.writeI16( 10234 ) );
        REQUIRE( cursor.writeI32( 305450464 ) );
        REQUIRE( cursor.writeI64( 811183134362 ) );

        REQUIRE( cursor.setPosition( 0 ) );
        int8_t  sbyte;
        int16_t sword;
        int32_t sdword;
        int64_t sqword;

        REQUIRE( cursor.readI8( sbyte ) );
        REQUIRE( cursor.readI16( sword ) );
        REQUIRE( cursor.readI32( sdword ) );
        REQUIRE( cursor.readI64( sqword ) );
        REQUIRE( sbyte == 42 );
        REQUIRE( sword == 10234 );
        REQUIRE( sdword == 305450464 );
        REQUIRE( sqword == 811183134362 );
    }

    SECTION( "basic4" )
    {
        CursorLE cursor( bufferMemory_, sizeof( bufferMemory_ ) );
        REQUIRE( cursor.writeI8( (int8_t)-42 ) );
        REQUIRE( cursor.writeI16( (int16_t)-10234 ) );
        REQUIRE( cursor.writeI32( (int32_t)-305450464 ) );
        REQUIRE( cursor.writeI64( (int64_t)-811183134362 ) );
        REQUIRE( cursor.setPosition( 0 ) );
        int8_t  sbyte;
        int16_t sword;
        int32_t sdword;
        int64_t sqword;

        REQUIRE( cursor.readI8( sbyte ) );
        REQUIRE( cursor.readI16( sword ) );
        REQUIRE( cursor.readI32( sdword ) );
        REQUIRE( cursor.readI64( sqword ) );
        REQUIRE( sbyte == -42 );
        REQUIRE( sword == -10234 );
        REQUIRE( sdword == -305450464 );
        REQUIRE( sqword == -811183134362 );
    }

    SECTION( "skip" )
    {
        CursorLE cursor( bufferMemory_, sizeof( bufferMemory_ ) );
        size_t   remaining = cursor.getRemainingCount();
        REQUIRE( cursor.skip( remaining / 2 ) );
        REQUIRE( cursor.getRemainingCount() == remaining / 2 );
        REQUIRE( cursor.skip( remaining / 2 + 1 ) == false );
        REQUIRE( cursor.skip( 1 ) == false );
    }

    SECTION( "error-cases" )
    {
        CursorLE cursor( bufferMemory_, sizeof( bufferMemory_ ) );
        REQUIRE( cursor.errorOccurred() == false);

        REQUIRE( cursor.setPosition( 0 ) );
        REQUIRE( cursor.errorOccurred() == false );
        int8_t dummy;
        REQUIRE( cursor.write( &dummy, BUFFER_SIZE + 1 ) == false );
        REQUIRE( cursor.errorOccurred() );
        REQUIRE( cursor.writeI8( dummy ) == false );

        REQUIRE( cursor.setPosition( 0 ) );
        REQUIRE( cursor.errorOccurred() == false );
        REQUIRE( cursor.read( &dummy, BUFFER_SIZE + 1 ) == false );
        REQUIRE( cursor.errorOccurred() );
        REQUIRE( cursor.readI8( dummy ) == false );

        REQUIRE( cursor.setPosition( 0 ) );
        REQUIRE( cursor.errorOccurred() == false );
        REQUIRE( cursor.writeI8( dummy ) == true );
        REQUIRE( cursor.setPosition( BUFFER_SIZE + 1 ) == false );
        REQUIRE( cursor.errorOccurred() == true );

        REQUIRE( cursor.setPosition( 0 ) );
        REQUIRE( cursor.errorOccurred() == false );
        REQUIRE( cursor.read( nullptr, 0 ) == false );
        REQUIRE( cursor.errorOccurred() );

        REQUIRE( cursor.setPosition( 0 ) );
        REQUIRE( cursor.errorOccurred() == false );
        REQUIRE( cursor.write( nullptr, 0 ) == false);
        REQUIRE( cursor.errorOccurred() );

        REQUIRE( cursor.setPosition( 0 ) );
        REQUIRE( cursor.errorOccurred() == false );
        REQUIRE( cursor.fill( BUFFER_SIZE + 1 ) == false );
        REQUIRE( cursor.errorOccurred() );
        REQUIRE( cursor.fill( 1 ) == false );
        REQUIRE( cursor.errorOccurred() );
    }

    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}
