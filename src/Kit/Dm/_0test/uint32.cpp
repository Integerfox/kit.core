/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include "catch2/catch_test_macros.hpp"
#include "Kit/System/Trace.h"
#include "Kit/Text/DString.h"
#include "Kit/Dm/ModelDatabase.h"
#include "Kit/Dm/Mp/Uint32.h"
#include "common.h"
#include <cstdint>
#include <string.h>


////////////////////////////////////////////////////////////////////////////////
#define STREAM_BUFFER_SIZE 100
#define MAX_STR_LENG       1024

static constexpr const char* EXPECTED_PRETTY =
    "{\r\n"
    "  \"name\": \"APPLE\",\r\n"
    "  \"valid\": true,\r\n"
    "  \"type\": \"Kit::Dm::Mp::Uint32\",\r\n"
    "  \"seqnum\": 18,\r\n"
    "  \"locked\": false,\r\n"
    "  \"val\": {\r\n"
    "    \"dec\": 127,\r\n"
    "    \"hex\": \"0x7F\"\r\n"
    "  }\r\n"
    "}";

#define STRCMP( s1, s2 ) ( strcmp( s1, s2 ) == 0 )

// Allocate/create my Model Database
static ModelDatabase modelDb_( "ignoreThisParameter_usedToInvokeTheStaticConstructor" );

// Allocate my Model Points
static Mp::Uint32 mp_apple_( modelDb_, "APPLE" );
static Mp::Uint32 mp_orange_( modelDb_, "ORANGE", 32 );
static Mp::Uint32 mp_cherry_( modelDb_, "CHERRY", 666U );

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Uint32" )
{
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();

    Kit::Text::DString errorMsg( "noerror", 1024 );
    char               string[MAX_STR_LENG + 1];
    bool               truncated;
    uint16_t           seqNum;
    uint16_t           seqNum2;
    IModelPoint*       mp;
    mp_apple_.removeLock();
    mp_orange_.removeLock();

    SECTION( "gets" )
    {
        // Gets...
        const char* name = mp_apple_.getName();
        REQUIRE( strcmp( name, "APPLE" ) == 0 );
        name = mp_orange_.getName();
        REQUIRE( strcmp( name, "ORANGE" ) == 0 );

        size_t s = mp_apple_.getSize();
        REQUIRE( s == sizeof( uint32_t ) );
        s = mp_orange_.getSize();
        REQUIRE( s == sizeof( uint32_t ) );

        s = mp_apple_.getExternalSize();
        REQUIRE( s == sizeof( uint32_t ) + sizeof( bool ) );
        s = mp_orange_.getExternalSize();
        REQUIRE( s == sizeof( uint32_t ) + sizeof( bool ) );

        const char* mpType = mp_apple_.getTypeAsText();
        KIT_SYSTEM_TRACE_MSG( SECT_, "typeText: [%s]", mpType );
        REQUIRE( strcmp( mpType, "Kit::Dm::Mp::Uint32" ) == 0 );
    }

    SECTION( "copy" )
    {
        mp_apple_.write( 12 );
        mp_orange_.copyFrom( mp_apple_ );
        uint32_t value;
        bool     valid = mp_orange_.read( value );
        REQUIRE( valid );
        REQUIRE( value == 12 );

        mp_apple_.setInvalid();
        mp_orange_.copyFrom( mp_apple_ );
        REQUIRE( mp_orange_.isNotValid() );
    }

    SECTION( "export" )
    {
        //  Export/Import Buffer
        uint8_t streamBuffer[STREAM_BUFFER_SIZE];
        REQUIRE( mp_apple_.getExternalSize() <= STREAM_BUFFER_SIZE );

        // Export...
        REQUIRE( mp_apple_.isNotValid() == true );
        seqNum   = mp_apple_.getSequenceNumber();
        seqNum2  = mp_apple_.getSequenceNumber();
        size_t b = mp_apple_.exportData( streamBuffer, sizeof( streamBuffer ), &seqNum2 );
        REQUIRE( b != 0 );
        REQUIRE( b == mp_apple_.getExternalSize() );
        REQUIRE( seqNum == seqNum2 );

        // Update the MP
        seqNum = mp_apple_.write( 42 );
        REQUIRE( seqNum == seqNum2 + 1 );
        uint32_t value;
        bool     valid;
        valid = mp_apple_.read( value );
        REQUIRE( valid == true );
        REQUIRE( mp_apple_.isNotValid() == false );
        REQUIRE( value == 42 );

        // Import...
        b = mp_apple_.importData( streamBuffer, sizeof( streamBuffer ), &seqNum2 );
        REQUIRE( b != 0 );
        REQUIRE( b == mp_apple_.getExternalSize() );
        REQUIRE( seqNum + 1 == seqNum2 );

        // Read import value/state
        valid = mp_apple_.read( value );
        REQUIRE( mp_apple_.isNotValid() == true );
        REQUIRE( valid == false );

        // Update the MP
        seqNum = mp_apple_.write( 13 );
        REQUIRE( seqNum == seqNum2 + 1 );
        valid = mp_apple_.read( value );
        REQUIRE( valid == true );
        REQUIRE( mp_apple_.isNotValid() == false );
        REQUIRE( value == 13 );

        // Export...
        REQUIRE( mp_apple_.isNotValid() == false );
        b = mp_apple_.exportData( streamBuffer, sizeof( streamBuffer ), &seqNum2 );
        REQUIRE( b != 0 );
        REQUIRE( b == mp_apple_.getExternalSize() );
        REQUIRE( seqNum == seqNum2 );

        // set a new value AND invalidate the MP
        mp_apple_.write( 666 );
        seqNum = mp_apple_.setInvalid();
        REQUIRE( seqNum == seqNum2 + 2 );
        REQUIRE( mp_apple_.isNotValid() == true );

        // Import...
        b = mp_apple_.importData( streamBuffer, sizeof( streamBuffer ), &seqNum2 );
        REQUIRE( b != 0 );
        REQUIRE( b == mp_apple_.getExternalSize() );
        REQUIRE( seqNum + 1 == seqNum2 );

        // Read import value/state
        valid = mp_apple_.read( value );
        REQUIRE( mp_apple_.isNotValid() == false );
        REQUIRE( valid == true );
        REQUIRE( value == 13 );
    }

    SECTION( "export" )
    {
        //  Export/Import Buffer
        uint8_t streamBuffer[STREAM_BUFFER_SIZE];
        REQUIRE( mp_apple_.getExternalSize( true ) <= STREAM_BUFFER_SIZE );

        // Export...
        mp_apple_.setInvalid();
        REQUIRE( mp_apple_.isNotValid() == true );
        REQUIRE( mp_apple_.isLocked() == false );
        uint16_t seqNum  = mp_apple_.getSequenceNumber();
        uint16_t seqNum2 = mp_apple_.getSequenceNumber();
        size_t   b       = mp_apple_.exportData( streamBuffer, sizeof( streamBuffer ), &seqNum2, true );
        REQUIRE( b != 0 );
        REQUIRE( b == mp_apple_.getExternalSize( true ) );
        REQUIRE( seqNum == seqNum2 );

        // Update the MP
        seqNum = mp_apple_.write( 42, false, IModelPoint::eLOCK );
        REQUIRE( seqNum == seqNum2 + 1 );
        uint32_t value;
        bool     valid;
        valid = mp_apple_.read( value );
        REQUIRE( valid == true );
        REQUIRE( mp_apple_.isLocked() == true );
        REQUIRE( mp_apple_.isNotValid() == false );
        REQUIRE( value == 42 );

        // Import...
        b = mp_apple_.importData( streamBuffer, sizeof( streamBuffer ), &seqNum2, true );
        REQUIRE( b != 0 );
        REQUIRE( b == mp_apple_.getExternalSize( true ) );
        REQUIRE( seqNum + 1 == seqNum2 );

        // Read import value/state
        valid = mp_apple_.read( value );
        REQUIRE( mp_apple_.isNotValid() == true );
        REQUIRE( mp_apple_.isLocked() == false );
        REQUIRE( valid == false );

        // Update the MP
        seqNum = mp_apple_.write( 13, false, IModelPoint::eLOCK );
        REQUIRE( seqNum == seqNum2 + 1 );
        valid = mp_apple_.read( value );
        REQUIRE( valid == true );
        REQUIRE( mp_apple_.isLocked() == true );
        REQUIRE( mp_apple_.isNotValid() == false );
        REQUIRE( value == 13 );

        // Export...
        REQUIRE( mp_apple_.isNotValid() == false );
        b = mp_apple_.exportData( streamBuffer, sizeof( streamBuffer ), &seqNum2, true );
        REQUIRE( b != 0 );
        REQUIRE( b == mp_apple_.getExternalSize( true ) );
        REQUIRE( seqNum == seqNum2 );

        // set a new value AND invalidate the MP
        mp_apple_.write( 666, false, IModelPoint::eLOCK );
        seqNum = mp_apple_.setInvalid( false, IModelPoint::eUNLOCK );
        REQUIRE( seqNum == seqNum2 + 2 );
        REQUIRE( mp_apple_.isNotValid() == true );
        REQUIRE( mp_apple_.isLocked() == false );

        // Import...
        b = mp_apple_.importData( streamBuffer, sizeof( streamBuffer ), &seqNum2, true );
        REQUIRE( b != 0 );
        REQUIRE( b == mp_apple_.getExternalSize( true ) );
        REQUIRE( seqNum + 1 == seqNum2 );

        // Read import value/state
        valid = mp_apple_.read( value );
        REQUIRE( mp_apple_.isNotValid() == false );
        REQUIRE( valid == true );
        REQUIRE( mp_apple_.isLocked() == true );
        REQUIRE( value == 13 );
    }


    SECTION( "toJSON-Invalid/terse" )
    {
        // Invalid (Default value)
        mp_apple_.setInvalid();
        mp_apple_.toJSON( string, MAX_STR_LENG, truncated, false );
        KIT_SYSTEM_TRACE_MSG( SECT_, "toJSON: terse [%s])", string );
        REQUIRE( truncated == false );

        StaticJsonDocument<1024> doc;
        DeserializationError     err = deserializeJson( doc, string );
        REQUIRE( err == DeserializationError::Ok );
        REQUIRE( STRCMP( doc["name"], "APPLE" ) );
        REQUIRE( doc["type"].as<const char*>() == 0 );
        REQUIRE( doc["seqnum"].as<const char*>() == 0 );
        REQUIRE( doc["locked"].as<const char*>() == 0 );
        REQUIRE( doc["valid"] == false );
    }

    SECTION( "toJSON-Invalid" )
    {
        // Invalid (Default value)
        uint16_t seqnum = mp_apple_.setInvalid();
        mp_apple_.toJSON( string, MAX_STR_LENG, truncated );
        KIT_SYSTEM_TRACE_MSG( SECT_, "toJSON: [%s]", string );
        REQUIRE( truncated == false );

        StaticJsonDocument<1024> doc;
        DeserializationError     err = deserializeJson( doc, string );
        REQUIRE( err == DeserializationError::Ok );
        REQUIRE( STRCMP( doc["name"], "APPLE" ) );
        REQUIRE( STRCMP( doc["type"], mp_apple_.getTypeAsText() ) );
        REQUIRE( doc["seqnum"] == seqnum );
        REQUIRE( doc["locked"] == false );
        REQUIRE( doc["valid"] == false );
    }

    SECTION( "toJSON-Invalid + Locked" )
    {
        mp_apple_.applyLock();
        mp_apple_.toJSON( string, MAX_STR_LENG, truncated );
        KIT_SYSTEM_TRACE_MSG( SECT_, "toJSON: [%s]", string );

        StaticJsonDocument<1024> doc;
        DeserializationError     err = deserializeJson( doc, string );
        REQUIRE( err == DeserializationError::Ok );
        REQUIRE( doc["locked"] == true );
        REQUIRE( doc["valid"] == false );
    }

    SECTION( "toJSON-Value" )
    {
        uint16_t seqnum = mp_apple_.write( 127, false, IModelPoint::eUNLOCK );
        mp_apple_.toJSON( string, MAX_STR_LENG, truncated );
        KIT_SYSTEM_TRACE_MSG( SECT_, "toJSON: [%s]", string );

        StaticJsonDocument<1024> doc;
        DeserializationError     err = deserializeJson( doc, string );
        REQUIRE( err == DeserializationError::Ok );
        REQUIRE( doc["seqnum"] == seqnum );
        REQUIRE( doc["locked"] == false );
        REQUIRE( doc["valid"] == true );
        REQUIRE( doc["val"]["dec"] == 127 );
        REQUIRE( STRCMP( doc["val"]["hex"], "0x7F" ) );
    }


    SECTION( "toJSON-pretty" )
    {
        uint16_t seqnum = mp_apple_.write( 127, false, IModelPoint::eUNLOCK );
        mp_apple_.toJSON( string, MAX_STR_LENG, truncated, true, true );
        KIT_SYSTEM_TRACE_MSG( SECT_, "toJSON: [%s]", string );
        REQUIRE( strcmp( string, EXPECTED_PRETTY ) == 0 );

        StaticJsonDocument<1024> doc;
        DeserializationError     err = deserializeJson( doc, string );
        REQUIRE( err == DeserializationError::Ok );
        REQUIRE( doc["seqnum"] == seqnum );
        REQUIRE( doc["locked"] == false );
        REQUIRE( doc["valid"] == true );
        REQUIRE( doc["val"]["dec"] == 127 );
        REQUIRE( STRCMP( doc["val"]["hex"], "0x7F" ) );
    }

    SECTION( "toJSON-Value + Lock" )
    {
        mp_apple_.applyLock();
        mp_apple_.toJSON( string, MAX_STR_LENG, truncated );
        KIT_SYSTEM_TRACE_MSG( SECT_, "toJSON: [%s]", string );

        StaticJsonDocument<1024> doc;
        DeserializationError     err = deserializeJson( doc, string );
        REQUIRE( err == DeserializationError::Ok );
        REQUIRE( doc["locked"] == true );
        REQUIRE( doc["valid"] == true );
        REQUIRE( doc["val"]["dec"] == 127 );
        REQUIRE( STRCMP( doc["val"]["hex"], "0x7F" ) );
    }


    SECTION( "fromJSON" )
    {
        // Start with MP in the invalid state
        mp_orange_.setInvalid();
        uint16_t seqNum = mp_apple_.setInvalid();

        const char* json   = "{name:\"APPLE\", val:1234}";
        bool        result = modelDb_.fromJSON( json, &errorMsg, &mp, &seqNum2 );
        KIT_SYSTEM_TRACE_MSG( SECT_, "fromJSON errorMsg=[%s]", errorMsg.getString() );
        REQUIRE( result == true );
        REQUIRE( seqNum2 == seqNum + 1 );
        uint32_t value;
        bool     valid = mp_apple_.read( value, &seqNum );
        REQUIRE( seqNum == seqNum2 );
        REQUIRE( valid );
        REQUIRE( value == 1234 );
        REQUIRE( errorMsg == "noerror" );
        REQUIRE( mp == &mp_apple_ );
    }

    SECTION( "fromJSON2" )
    {
        // Start with MP in the invalid state
        mp_orange_.setInvalid();
        uint16_t seqNum = mp_apple_.setInvalid();

        const char* json   = R"({name:"APPLE", val:"0x1234"})";
        bool        result = modelDb_.fromJSON( json, &errorMsg, &mp, &seqNum2 );
        KIT_SYSTEM_TRACE_MSG( SECT_, "fromJSON errorMsg=[%s]", errorMsg.getString() );
        REQUIRE( result == true );
        REQUIRE( seqNum2 == seqNum + 1 );
        uint32_t value;
        bool     valid = mp_apple_.read( value, &seqNum );
        REQUIRE( seqNum == seqNum2 );
        REQUIRE( valid );
        REQUIRE( value == 0x1234 );
        REQUIRE( errorMsg == "noerror" );
        REQUIRE( mp == &mp_apple_ );
    }

    SECTION( "Write value - error cases" )
    {
        // Start with MP in the invalid state
        mp_orange_.setInvalid();
        seqNum = mp_apple_.setInvalid();

        const char* json   = R"({name:"APPLE", val:"abc"})";
        bool        result = modelDb_.fromJSON( json, &errorMsg );
        KIT_SYSTEM_TRACE_MSG( SECT_, "fromJSON errorMsg=[%s]", errorMsg.getString() );
        REQUIRE( result == false );
        REQUIRE( mp_apple_.isNotValid() );
        REQUIRE( errorMsg != "noerror" );

        errorMsg = "noerror";
        json     = R"({name:"APPLE"})";
        result   = modelDb_.fromJSON( json, &errorMsg );
        KIT_SYSTEM_TRACE_MSG( SECT_, "fromJSON errorMsg=[%s]", errorMsg.getString() );
        REQUIRE( result == false );
        REQUIRE( mp_apple_.isNotValid() );
        REQUIRE( errorMsg != "noerror" );

        errorMsg = "noerror";
        json     = R"({namex:"APPLE"})";
        result   = modelDb_.fromJSON( json, &errorMsg );
        KIT_SYSTEM_TRACE_MSG( SECT_, "fromJSON errorMsg=[%s]", errorMsg.getString() );
        REQUIRE( result == false );
        REQUIRE( errorMsg != "noerror" );
        REQUIRE( mp_apple_.isNotValid() );

        errorMsg = "noerror";
        json     = R"({name:"APPLE", val:a123})";
        result   = modelDb_.fromJSON( json, &errorMsg );
        KIT_SYSTEM_TRACE_MSG( SECT_, "fromJSON errorMsg=[%s]", errorMsg.getString() );
        REQUIRE( result == false );
        REQUIRE( mp_apple_.isNotValid() );
        REQUIRE( errorMsg != "noerror" );

        errorMsg = "noerror";
        json     = R"({name:"APPLE", val:123a})";
        result   = modelDb_.fromJSON( json, &errorMsg );
        KIT_SYSTEM_TRACE_MSG( SECT_, "fromJSON errorMsg=[%s]", errorMsg.getString() );
        REQUIRE( result == false );
        REQUIRE( mp_apple_.isNotValid() );
        REQUIRE( errorMsg != "noerror" );

        errorMsg = "noerror";
        json     = R"({name:"APPLE", val:2.2})";
        result   = modelDb_.fromJSON( json, &errorMsg );
        KIT_SYSTEM_TRACE_MSG( SECT_, "fromJSON errorMsg=[%s]", errorMsg.getString() );
        REQUIRE( result == false );
        REQUIRE( mp_apple_.isNotValid() );
        REQUIRE( errorMsg != "noerror" );

        errorMsg = "noerror";
        json     = R"({name:"APPLE", val:8589934591})";  // Exceed range of the uint32_t
        result   = modelDb_.fromJSON( json, &errorMsg );
        KIT_SYSTEM_TRACE_MSG( SECT_, "fromJSON errorMsg=[%s]", errorMsg.getString() );
        REQUIRE( result == false );
        REQUIRE( mp_apple_.isNotValid() );
        REQUIRE( errorMsg != "noerror" );

        json   = R"({name:"APPLE", val:"hello"})";  // Exceed range of the uint32_t
        result = modelDb_.fromJSON( json );
        KIT_SYSTEM_TRACE_MSG( SECT_, "fromJSON errorMsg=[%s]", errorMsg.getString() );
        REQUIRE( result == false );
        REQUIRE( mp_apple_.isNotValid() );

        errorMsg = "noerror";
        json     = R"({name:"BOB", valid:false})";
        result   = modelDb_.fromJSON( json, &errorMsg );
        KIT_SYSTEM_TRACE_MSG( SECT_, "fromJSON errorMsg=[%s]", errorMsg.getString() );
        REQUIRE( result == false );
        REQUIRE( mp_apple_.isNotValid() );
        REQUIRE( errorMsg != "noerror" );

        json   = R"(name:"BOB", valid:false})";
        result = modelDb_.fromJSON( json );
        KIT_SYSTEM_TRACE_MSG( SECT_, "fromJSON errorMsg=[%s]", errorMsg.getString() );
        REQUIRE( result == false );
        REQUIRE( mp_apple_.isNotValid() );
    }

    SECTION( "Set Invalid" )
    {
        // Start with MP in the invalid state
        mp_orange_.setInvalid();
        seqNum = mp_apple_.setInvalid();

        seqNum             = mp_apple_.write( 12 );
        const char* json   = R"({name:"APPLE", val:4321, valid:false})";
        bool        result = modelDb_.fromJSON( json, &errorMsg, &mp, &seqNum2 );
        KIT_SYSTEM_TRACE_MSG( SECT_, "fromJSON errorMsg=[%s]", errorMsg.getString() );
        REQUIRE( result == true );
        REQUIRE( seqNum2 == seqNum + 1 );
        uint32_t value;
        bool     valid = mp_apple_.read( value, &seqNum );
        REQUIRE( seqNum == seqNum2 );
        REQUIRE( valid == false );
        REQUIRE( errorMsg == "noerror" );
        REQUIRE( mp == &mp_apple_ );
    }

    SECTION( "lock..." )
    {
        // Start with MP in the invalid state
        mp_orange_.setInvalid();
        seqNum = mp_apple_.setInvalid();

        const char* json   = R"({name:"APPLE", val:4321, locked:true})";
        bool        result = modelDb_.fromJSON( json, &errorMsg );
        KIT_SYSTEM_TRACE_MSG( SECT_, "fromJSON errorMsg=[%s]", errorMsg.getString() );
        REQUIRE( result == true );
        uint32_t value;
        bool     valid = mp_apple_.read( value );
        REQUIRE( valid == true );
        REQUIRE( errorMsg == "noerror" );
        REQUIRE( mp_apple_.isLocked() == true );
        REQUIRE( value == 4321 );

        json   = R"({name:"APPLE", valid:false, locked:false})";
        result = modelDb_.fromJSON( json, &errorMsg );
        KIT_SYSTEM_TRACE_MSG( SECT_, "fromJSON errorMsg=[%s]", errorMsg.getString() );
        REQUIRE( result == true );
        REQUIRE( mp_apple_.isNotValid() == true );
        REQUIRE( mp_apple_.isLocked() == false );

        json   = R"({name:"APPLE", val:21, locked:true})";
        result = modelDb_.fromJSON( json );
        KIT_SYSTEM_TRACE_MSG( SECT_, "fromJSON errorMsg=[%s]", errorMsg.getString() );
        REQUIRE( result == true );
        REQUIRE( mp_apple_.isLocked() == true );
        valid = mp_apple_.read( value );
        REQUIRE( valid == true );
        REQUIRE( value == 21 );

        json   = R"({name:"APPLE", val:42 })";
        result = modelDb_.fromJSON( json, &errorMsg );
        KIT_SYSTEM_TRACE_MSG( SECT_, "fromJSON errorMsg=[%s]", errorMsg.getString() );
        REQUIRE( result == true );
        REQUIRE( mp_apple_.isLocked() == true );
        valid = mp_apple_.read( value );
        REQUIRE( valid == true );
        REQUIRE( value == 21 );

        json   = R"({name:"APPLE", locked:false})";
        result = modelDb_.fromJSON( json, &errorMsg );
        KIT_SYSTEM_TRACE_MSG( SECT_, "fromJSON errorMsg=[%s]", errorMsg.getString() );
        REQUIRE( result == true );
        valid = mp_apple_.read( value );
        REQUIRE( valid == true );
        REQUIRE( value == 21 );
        REQUIRE( mp_apple_.isLocked() == false );
    }

    SECTION( "Increment/Decrement" )
    {
        // Start with MP in the invalid state
        mp_orange_.setInvalid();
        seqNum = mp_apple_.setInvalid();

        seqNum2 = mp_apple_.increment( 2 );
        REQUIRE( seqNum2 == seqNum + 1 );
        uint32_t value;
        bool     valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( value == 2 );  // By design the invalid MP has a 'data value' of zero
        seqNum = mp_apple_.decrement( 1 );
        REQUIRE( seqNum == seqNum2 + 1 );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( value == 1 );

        mp_apple_.increment( 5, false, IModelPoint::eLOCK );
        mp_apple_.write( 10 );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( value == 1 + 5 );

        mp_apple_.decrement( 2 );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( value == 1 + 5 );

        mp_apple_.decrement( 2, false, IModelPoint::eUNLOCK );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( value == 1 + 5 - 2 );
    }

    SECTION( "forceNotification" )
    {
        uint16_t seqNum  = mp_apple_.write( 12 );
        uint16_t seqNum2 = mp_apple_.write( 12 );
        REQUIRE( seqNum2 == seqNum );
        seqNum2 = mp_apple_.write( 12, true );
        REQUIRE( seqNum2 == ( seqNum + 1 ) );
    }

    SECTION( "bit operations" )
    {
        // Start with MP in the invalid state (which has a 'data value' of zero)
        seqNum = mp_apple_.setInvalid();
        uint32_t value;

        seqNum = mp_apple_.bitwiseOR( 0x0F0F0F0F );
        bool valid  = mp_apple_.read( value );
        REQUIRE( valid == true );
        REQUIRE( value == 0x0F0F0F0F );

        seqNum = mp_apple_.bitwiseXOR( 0xFFFF0000 );
        valid  = mp_apple_.read( value );
        REQUIRE( valid == true );
        REQUIRE( value == 0xF0F00F0F  );

        seqNum = mp_apple_.bitwiseAND( 0xFF00FF00 );
        valid  = mp_apple_.read( value );
        REQUIRE( valid == true );
        REQUIRE( value == 0xF0000F00);

        seqNum = mp_apple_.bitwiseClearAndSet( 0xE0000700, 0x00001000 );
        valid  = mp_apple_.read( value );
        REQUIRE( valid == true );
        REQUIRE( value == 0x10001800 );

        mp_apple_.write( 0x00000F00 );
        seqNum = mp_apple_.setBit( 31 );
        valid  = mp_apple_.read( value );
        REQUIRE( valid == true );
        REQUIRE( value == (( 1U << 31 ) | 0x00000F00) );

        seqNum = mp_apple_.clearBit( 31 );
        valid  = mp_apple_.read( value );
        REQUIRE( valid == true );
        REQUIRE( value == 0x00000F00 );

        seqNum = mp_apple_.flipBit( 8 );  // Flip bit that is currently set
        valid  = mp_apple_.read( value );
        REQUIRE( valid == true );
        REQUIRE( value == 0x00000E00 );

        seqNum = mp_apple_.flipBit( 8 );  // Flip bit that is currently clear
        valid  = mp_apple_.read( value );
        REQUIRE( valid == true );
        REQUIRE( value == 0x00000F00 );

        // Test Read Then Clear Bits
        mp_apple_.write( 0xFFFF0000 );
        uint32_t readValue;
        seqNum = mp_apple_.readThenClearBits( readValue, 0x00FF0000 );
        valid  = mp_apple_.read( value );
        REQUIRE( valid == true );
        REQUIRE( readValue == 0xFFFF0000 );
        REQUIRE( value == 0xFF000000 );

        // Test Read Then Clear
        mp_apple_.write( 0x12345678 );
        seqNum = mp_apple_.readThenClear( readValue );
        valid  = mp_apple_.read( value );
        REQUIRE( valid == true );
        REQUIRE( readValue == 0x12345678 );
        REQUIRE( value == 0x00000000 );
    }

    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}
