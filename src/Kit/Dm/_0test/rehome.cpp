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
#include "common.h"
#include "Kit/_support/testing/ManualEventQueue.h"
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
        , m_callbackCount( 0 )
    {
        setCallback<RehomeObserver, &RehomeObserver::changed>( this );
    }

    void changed( Mp::Uint32& modelPointThatChanged, IObserver& clientObserver ) noexcept
    {
        // Re-home to a different MP while still inside the callback -- NOT allowed.
        // NOTE: The FatalError trap fires AFTER this method returns (in eNOTIFY_COMPLETE
        //       processing)
        m_callbackCount++;
        m_rehomeTargetMp.attach( *this );
    }

    Mp::Uint32& m_rehomeTargetMp;
    unsigned    m_callbackCount;
};

}  // end anonymous namespace

TEST_CASE( "rehome_trap" )
{
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();

    ModelDatabase    myDb;
    Mp::Uint32       mpA( myDb, "REHOME_A" );
    Mp::Uint32       mpB( myDb, "REHOME_B" );
    ManualEventQueue myMbox;  // No thread: the test thread runs the queue

    RehomeObserver observer( myMbox, mpB );

    // Attaching with the (default) 'unknown' sequence number triggers an immediate
    // change notification callback -- which is where the illegal re-home is attempted
    mpA.attach( observer );
    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );  // Nothing happens until the queue runs
    REQUIRE( myMbox.runUntilIdle() );
    REQUIRE( observer.m_callbackCount == 1 );

    // The re-home attempt is a FatalError (the unit test build counts fatal errors instead of exiting)
    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 1u );

    // NOTE: 'observer' is intentionally left un-detached: re-homing during a callback is a
    //       documented fatal/unrecoverable misuse, so there is no well-defined state to clean up.
}
