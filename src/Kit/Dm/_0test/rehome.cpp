/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/EventQueue/Server.h"
#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include "catch2/catch_test_macros.hpp"
#include "common.h"
#include "Kit/Dm/ModelDatabase.h"


////////////////////////////////////////////////////////////////////////////////
namespace {

/// Observer that (illegally) attempts to re-home itself to a DIFFERENT Model
/// Point from within its own change-notification callback -- this must trip
/// the ModelPointBase re-homing FatalError trap.
class RehomeObserver : public ObserverCallback<Mp::Uint32>
{
public:
    RehomeObserver( Kit::EventQueue::IChangeNotification& myEventQueue,
                    Mp::Uint32&                           rehomeTargetMp ) noexcept
        : ObserverCallback<Mp::Uint32>( myEventQueue )
        , m_rehomeTargetMp( rehomeTargetMp )
    {
        setCallback<RehomeObserver, &RehomeObserver::changed>( this );
    }

    void changed( Mp::Uint32& modelPointThatChanged, IObserver& clientObserver ) noexcept
    {
        // Re-home to a different MP while still inside the callback -- NOT allowed.
        // NOTE: The FatalError trap fires AFTER this method returns (in eNOTIFY_COMPLETE
        //       processing), so do NOT signal the master thread from here.
        m_rehomeTargetMp.attach( *this );
    }

    Mp::Uint32& m_rehomeTargetMp;
};

}  // end anonymous namespace

TEST_CASE( "rehome_trap" )
{
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();

    ModelDatabase           myDb;
    Mp::Uint32              mpA( myDb, "REHOME_A" );
    Mp::Uint32              mpB( myDb, "REHOME_B" );
    Kit::EventQueue::Server myMbox;
    Kit::System::Thread*    t1 = Kit::System::Thread::create( myMbox, "REHOME_T1" );

    RehomeObserver observer( myMbox, mpB );

    // Attaching with the (default) 'unknown' sequence number triggers an immediate
    // change notification callback -- which is where the illegal re-home is attempted
    mpA.attach( observer );
    Kit::System::sleep( 200 );  // allow the notification (and re-home attempt) to be fully processed

    // The re-home attempt is a FatalError (the unit test build counts fatal errors instead of exiting)
    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 1u );

    // NOTE: 'observer' is intentionally left un-detached: re-homing during a callback is a
    //       documented fatal/unrecoverable misuse, so there is no well-defined state to clean up.
    myMbox.pleaseStop();
    Kit::System::sleep( 100 );  // allow time for the thread to stop
    REQUIRE( t1->isActive() == false );
    Kit::System::Thread::destroy( *t1 );
    Kit::System::sleep( 100 );  // allow time for the thread to stop BEFORE the runnable object goes out of scope
}
