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
#include "Kit/Dm/ModelDatabase.h"
#include "Kit/Dm/ObserverCallback.h"
#include "Kit/EventQueue/Server.h"
#include "Kit/Dm/Mp/Uint32.h"
#include "Kit/Dm/Mp/PseudoMpBase.h"
#include <string.h>

#define STRCMP( s1, s2 ) ( strcmp( s1, s2 ) == 0 )
#define MAX_STR_LENG     1024

using namespace Kit::Dm;
using namespace Kit::Dm::Mp;

////////////////////////////////////////////////////////////////////////////////

namespace {

/// Concrete pseudo MP (to show that the template is not specific to the Run Model Points)
class MyPseudo : public PseudoMpBase<uint32_t, Uint32>
{
public:
    MyPseudo( Kit::Dm::IModelDatabase& myModelBase, const char* symbolicName, Uint32& dataMp )
        : PseudoMpBase<uint32_t, Uint32>( myModelBase, symbolicName, dataMp )
    {
    }

    /// The only 'writer' method
    uint16_t increment() noexcept
    {
        uint32_t value = 0;
        m_dataMp.read( value );
        return m_dataMp.write( value + 1 );
    }

    const char* getTypeAsText() const noexcept override
    {
        return "MyPseudo";
    }

    bool fromJSON_( JsonVariant& src, LockRequest_T lockRequest, uint16_t& retSequenceNumber, Kit::Text::IString* errorMsg ) noexcept override
    {
        retSequenceNumber = increment();
        return true;
    }

    /// Expose the protected method
    bool callSetJSONVal( JsonDocument& doc ) noexcept
    {
        return setJSONVal( doc );
    }
};

}  // end anonymous namespace

////////////////////////////////////////////////////////////////////////////////

static ModelDatabase modelDb_( "ignoreThisParameter_usedToInvokeTheStaticConstructor" );
static Uint32        mp_data_( modelDb_, "DATA" );
static MyPseudo      mp_pseudo_( modelDb_, "PSEUDO", mp_data_ );

// Event queue for the observer (note: the queue is never run since the test does not generate change notifications)
static Kit::EventQueue::Server eventQ_;

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "PseudoMpBase" )
{
    uint32_t value  = 0;
    uint16_t seqNum = 0;

    // Access via the generic interface (the compiler cannot devirtualize/inline the calls)
    IModelPoint* mp = modelDb_.lookupModelPoint( "PSEUDO" );
    REQUIRE( mp == &mp_pseudo_ );
    mp_data_.setInvalid();
    mp_data_.setLockState( LockRequest_T::eUNLOCK );

    SECTION( "has its own name in the database" )
    {
        REQUIRE( modelDb_.lookupModelPoint( "PSEUDO" ) == &mp_pseudo_ );
        REQUIRE( modelDb_.lookupModelPoint( "DATA" ) == &mp_data_ );
        REQUIRE( STRCMP( mp_pseudo_.getName(), "PSEUDO" ) );
        REQUIRE( STRCMP( mp_pseudo_.getTypeAsText(), "MyPseudo" ) );
    }

    SECTION( "state is that of the data MP" )
    {
        REQUIRE( mp->isNotValid( &seqNum ) );
        REQUIRE( seqNum == mp_data_.getSequenceNumber() );
        REQUIRE( mp->isNotValid() );
        REQUIRE( mp->getSequenceNumber() == mp_data_.getSequenceNumber() );
        REQUIRE( mp->isLocked() == false );

        mp_data_.write( 42 );
        REQUIRE( mp->isNotValid( &seqNum ) == false );
        REQUIRE( seqNum == mp_data_.getSequenceNumber() );
        REQUIRE( mp->isNotValid() == false );
        REQUIRE( mp->getSequenceNumber() == mp_data_.getSequenceNumber() );
        mp_data_.setLockState( LockRequest_T::eLOCK );
        REQUIRE( mp->isLocked() );

        // The state follows the data MP's changes
        mp_data_.write( 43 );
        REQUIRE( mp->getSequenceNumber() == mp_data_.getSequenceNumber() );
        mp_data_.setLockState( LockRequest_T::eUNLOCK );
        REQUIRE( mp->isLocked() == false );
        mp_data_.setInvalid();
        REQUIRE( mp->isNotValid() );
    }

    SECTION( "read" )
    {
        REQUIRE( mp_pseudo_.read( value, &seqNum ) == false );
        mp_data_.write( 42 );
        REQUIRE( mp_pseudo_.read( value, &seqNum ) );
        REQUIRE( value == 42 );
        REQUIRE( seqNum == mp_data_.getSequenceNumber() );
    }

    SECTION( "getSize is delegated to the data MP" )
    {
        IModelPoint* mp = &mp_pseudo_;
        REQUIRE( mp->getSize() == sizeof( uint32_t ) );
        REQUIRE( mp->getSize() == mp_data_.getSize() );
    }

    SECTION( "toJSON is delegated to the data MP" )
    {
        char buffer1[MAX_STR_LENG];
        char buffer2[MAX_STR_LENG];
        bool truncated;

        mp_data_.write( 42 );
        mp_data_.toJSON( buffer1, sizeof( buffer1 ), truncated );
        REQUIRE( mp->toJSON( buffer2, sizeof( buffer2 ), truncated ) );
        REQUIRE( STRCMP( buffer1, buffer2 ) );

        // Non-verbose and pretty formats
        mp_data_.toJSON( buffer1, sizeof( buffer1 ), truncated, false );
        REQUIRE( mp->toJSON( buffer2, sizeof( buffer2 ), truncated, false ) );
        REQUIRE( STRCMP( buffer1, buffer2 ) );
        mp_data_.toJSON( buffer1, sizeof( buffer1 ), truncated, true, true );
        REQUIRE( mp->toJSON( buffer2, sizeof( buffer2 ), truncated, true, true ) );
        REQUIRE( STRCMP( buffer1, buffer2 ) );

        // Truncation is reported
        REQUIRE( mp->toJSON( buffer2, 10, truncated ) );
        REQUIRE( truncated );

        // Invalid
        mp_data_.setInvalid();
        mp_data_.toJSON( buffer1, sizeof( buffer1 ), truncated );
        mp->toJSON( buffer2, sizeof( buffer2 ), truncated );
        REQUIRE( STRCMP( buffer1, buffer2 ) );
    }

    SECTION( "setJSONVal is not supported" )
    {
        StaticJsonDocument<256> doc;
        mp_data_.write( 42 );
        REQUIRE( mp_pseudo_.callSetJSONVal( doc ) == false );
    }

    SECTION( "unsupported generic operations do nothing" )
    {
        uint8_t buffer[64];
        mp_data_.write( 42 );
        seqNum = mp_data_.getSequenceNumber();

        REQUIRE( mp->setInvalid() == seqNum );
        REQUIRE( mp->setInvalid( ChangeNotice_T::eFORCE, LockRequest_T::eLOCK ) == seqNum );
        REQUIRE( mp->touch() == seqNum );
        REQUIRE( mp->setLockState( LockRequest_T::eLOCK ) == seqNum );
        REQUIRE( mp->setLockState( LockRequest_T::eUNLOCK ) == seqNum );
        REQUIRE( mp->getExternalSize() == 0 );
        REQUIRE( mp->getExternalSize( true ) == 0 );
        REQUIRE( mp->exportData( buffer, sizeof( buffer ) ) == 0 );
        REQUIRE( mp->exportData( buffer, sizeof( buffer ), &seqNum, true ) == 0 );
        REQUIRE( mp->importData( buffer, sizeof( buffer ) ) == 0 );
        REQUIRE( mp->importData( buffer, sizeof( buffer ), &seqNum, true ) == 0 );

        REQUIRE( mp_data_.isNotValid() == false );
        REQUIRE( mp_data_.isLocked() == false );
        REQUIRE( mp_data_.getSequenceNumber() == seqNum );
        REQUIRE( mp_data_.read( value ) );
        REQUIRE( value == 42 );
    }

    SECTION( "JSON invalidate/lock requests are ignored" )
    {
        mp_data_.write( 42 );
        REQUIRE( modelDb_.fromJSON( "{name:\"PSEUDO\", valid:false}" ) );
        REQUIRE( modelDb_.fromJSON( "{name:\"PSEUDO\", locked:true}" ) );
        REQUIRE( mp_data_.isNotValid() == false );
        REQUIRE( mp_data_.isLocked() == false );
    }

    SECTION( "the derived class's fromJSON_ is used" )
    {
        mp_data_.write( 42 );
        REQUIRE( modelDb_.fromJSON( "{name:\"PSEUDO\", val:1}", nullptr, nullptr, &seqNum ) );
        REQUIRE( seqNum == mp_data_.getSequenceNumber() );
        REQUIRE( mp_data_.read( value ) );
        REQUIRE( value == 43 );
    }

    SECTION( "observers attach to the data MP" )
    {
        ObserverCallback<Uint32> observer( eventQ_ );
        mp_data_.write( 42 );

        mp_pseudo_.attach( observer, mp_data_.getSequenceNumber() );
        REQUIRE( observer.getModelPoint_() == &mp_data_ );
        mp_pseudo_.detach( observer );
        REQUIRE( observer.getModelPoint_() == nullptr );

        mp->genericAttach( observer, mp_data_.getSequenceNumber() );
        REQUIRE( observer.getModelPoint_() == &mp_data_ );
        mp->genericDetach( observer );
        REQUIRE( observer.getModelPoint_() == nullptr );
    }

    // Cleanup for a (potential) subsequent test
    mp_data_.setLockState( LockRequest_T::eUNLOCK );
    mp_data_.setInvalid();
}
