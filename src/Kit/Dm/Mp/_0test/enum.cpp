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
#include "Kit/EventQueue/Server.h"
#include "common.h"
#include "Kit/Type/BetterEnum.h"
#include "Kit/Text/FString.h"
#include "Kit/Dm/ModelDatabase.h"
#include "Kit/Dm/Mp/EnumBase.h"
#include <string.h>

#define STRCMP( s1, s2 ) ( strcmp( s1, s2 ) == 0 )
#define MAX_STR_LENG     1024
#define SECT_            "_0test"


#define INITIAL_VALUE    MyEnum::eBLUE

using namespace Kit::Dm;
using namespace Kit::Dm::Mp;

////////////////////////////////////////////////////////////////////////////////
BETTER_ENUM( MyEnum, int, eRED, eBLUE, eGREEN );


namespace {
class MyUut : public EnumBase<MyEnum, MyUut>
{
public:
    MyUut( Kit::Dm::IModelDatabase& myModelBase, const char* symbolicName )
        : EnumBase<MyEnum, MyUut>( myModelBase, symbolicName )
    {
    }

    /// Constructor. Valid Point.  Requires an initial value
    MyUut( Kit::Dm::IModelDatabase& myModelBase, const char* symbolicName, MyEnum initialValue )
        : EnumBase<MyEnum, MyUut>( myModelBase, symbolicName, initialValue )
    {
    }

    ///  See Kit::Dm::ModelPoint.
    const char* getTypeAsText() const noexcept
    {
        return "Kit::Dm::MyUut";
    }
};

}  // end anonymous namespace

////////////////////////////////////////////////////////////////////////////////

// Allocate/create my Model Database
static ModelDatabase modelDb_( "ignoreThisParameter_usedToInvokeTheStaticConstructor" );

// Allocate my Model Points
static MyUut mp_apple_( modelDb_, "APPLE" );
static MyUut mp_orange_( modelDb_, "ORANGE", INITIAL_VALUE );


// Don't let the Runnable object go out of scope before its thread has actually terminated!
static Kit::EventQueue::Server t1Mbox_;


////////////////////////////////////////////////////////////////////////////////

// Use template specialization to proper display the MP value for the Observer test
template <>
void Viewer<MyUut, MyEnum>::displayElement( const char* label, MyEnum& elem )
{
    KIT_SYSTEM_TRACE_MSG( SECT_, "%s:%s", label, elem._to_string() );
}


//
// Note: The bare minimum I need to test code that is 'new' to concrete MP type
//
TEST_CASE( "Enum" )
{
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();

    Kit::Text::FString<MAX_STR_LENG> errorMsg = "noerror";
    char                             string[MAX_STR_LENG + 1];
    bool                             truncated;
    bool                             valid;
    MyEnum                           value = MyEnum::eRED;
    mp_apple_.setInvalid();

    SECTION( "gets" )
    {
        // Gets...
        const char* name = mp_apple_.getName();
        REQUIRE( strcmp( name, "APPLE" ) == 0 );

        size_t s = mp_apple_.getSize();
        REQUIRE( s == sizeof( value ) );

        s = mp_apple_.getExternalSize();
        REQUIRE( s == sizeof( value ) + sizeof( bool ) );

        const char* mpType = mp_apple_.getTypeAsText();
        KIT_SYSTEM_TRACE_MSG( SECT_, "typeText: [%s]", mpType );
        REQUIRE( strcmp( mpType, "Kit::Dm::MyUut" ) == 0 );
    }


    SECTION( "read/writes" )
    {
        mp_apple_.write( MyEnum::eRED );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( value == +MyEnum::eRED );
        mp_apple_.write( MyEnum::eGREEN );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( value == +MyEnum::eGREEN );

        valid = mp_orange_.read( value );
        REQUIRE( valid );
        REQUIRE( value == +INITIAL_VALUE );
    }

    SECTION( "copy" )
    {
        mp_apple_.write( MyEnum::eRED );
        mp_orange_.copyFrom( mp_apple_ );
        valid = mp_apple_.read( value );
        REQUIRE( value == +MyEnum::eRED );
        REQUIRE( valid );

        mp_apple_.setInvalid();
        mp_orange_.copyFrom( mp_apple_ );
        REQUIRE( mp_orange_.isNotValid() );
    }


    SECTION( "toJSON-pretty" )
    {
        mp_apple_.write( MyEnum::eBLUE );
        mp_apple_.toJSON( string, MAX_STR_LENG, truncated, true, true );
        KIT_SYSTEM_TRACE_MSG( SECT_, "toJSON: [%s]", string ) ;

        StaticJsonDocument<1024> doc;
        DeserializationError     err = deserializeJson( doc, string );
        REQUIRE( err == DeserializationError::Ok );
        REQUIRE( doc["locked"] == false );
        REQUIRE( doc["valid"] == true );
        REQUIRE( STRCMP( doc["val"], ( +MyEnum::eBLUE )._to_string() ) );
    }

    SECTION( "fromJSON" )
    {
        const char* json   = "{name:\"APPLE\", val:\"eGREEN\"}";
        bool        result = modelDb_.fromJSON( json );
        REQUIRE( result == true );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( value == +MyEnum::eGREEN );

        json   = "{name:\"APPLE\", val:\"true\"}";
        result = modelDb_.fromJSON( json, &errorMsg );
        REQUIRE( result == false );
        REQUIRE( errorMsg != "noerror" );

        result = modelDb_.fromJSON( json );
        REQUIRE( result == false );

        json   = "{name:\"APPLE\", val:true}";
        result = modelDb_.fromJSON( json, &errorMsg );
        REQUIRE( result == false );
        REQUIRE( errorMsg != "noerror" );

        result = modelDb_.fromJSON( json );
        REQUIRE( result == false );
    }

    SECTION( "observer" )
    {
        KIT_SYSTEM_TRACE_SCOPE( SECT_, "observer test" );
        MyEnum expectedVal = MyEnum::eRED;
        Viewer<MyUut, MyEnum> viewer_apple1( t1Mbox_, Kit::System::Thread::getCurrent(), mp_apple_, expectedVal );
        Kit::System::Thread*  t1 = Kit::System::Thread::create( t1Mbox_, "T1" );

        // NOTE: The MP MUST be in the INVALID state at the start of this test
        viewer_apple1.open();
        mp_apple_.write( MyEnum::eRED );
        Kit::System::Thread::wait();
        viewer_apple1.close();

        // Shutdown threads
        t1Mbox_.pleaseStop();
        WAIT_FOR_THREAD_TO_STOP( t1 );
        Kit::System::Thread::destroy( *t1 );
    }

    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}
