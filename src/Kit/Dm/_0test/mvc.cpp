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
/* The test app consists of 4 threads:
   - Two client threads, one contains viewers, the other contain writers
   - One master thread (which the main thread)
*/

// Create my Data Model mailboxes
static Kit::EventQueue::Server t1Mbox_;
static Kit::EventQueue::Server t2Mbox_;

// Allocate/create my Model Database
static ModelDatabase modelDb_( "ignoreThisParameter_usedToInvokeTheStaticConstructor" );

// Allocate my Model Points
static Mp::Uint32 mp_apple_( modelDb_, "APPLE" );
static Mp::Uint32 mp_orange_( modelDb_, "ORANGE" );
static Mp::Uint32 mp_cherry_( modelDb_, "CHERRY" );
static Mp::Uint32 mp_plum_( modelDb_, "PLUM" );


#define VIEWER_APPLE1_END_VALUE  ( (uint32_t)10 )
#define VIEWER_ORANGE1_END_VALUE ( (uint32_t)7 )
#define VIEWER_CHERRY1_END_VALUE ( (uint32_t)11 )
#define VIEWER_PLUM1_END_VALUE   ( (uint32_t)9 )
#define GENERIC_VIEWER_END_COUNT ( VIEWER_APPLE1_END_VALUE + VIEWER_ORANGE1_END_VALUE + VIEWER_CHERRY1_END_VALUE )


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "mvc" )
{
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();

    Kit::System::Thread* t1 = Kit::System::Thread::create( t1Mbox_, "T1" );
    Kit::System::Thread* t2 = Kit::System::Thread::create( t2Mbox_, "T2" );

    // Create my viewers, writers
#define NUM_INSTANCES 9
    Viewer viewer_apple1( t1Mbox_, Kit::System::Thread::getCurrent(), mp_apple_, VIEWER_APPLE1_END_VALUE );
    Writer writer_apple1( t2Mbox_, Kit::System::Thread::getCurrent(), mp_apple_, 10, 1, VIEWER_APPLE1_END_VALUE, 1 );

    Viewer viewer_orange1( t1Mbox_, Kit::System::Thread::getCurrent(), mp_orange_, VIEWER_ORANGE1_END_VALUE );
    Writer writer_orange1( t1Mbox_, Kit::System::Thread::getCurrent(), mp_orange_, 0, 1, VIEWER_ORANGE1_END_VALUE, 1 );

    Viewer viewer_cherry1( t2Mbox_, Kit::System::Thread::getCurrent(), mp_cherry_, VIEWER_CHERRY1_END_VALUE );
    Writer writer_cherry1( t1Mbox_, Kit::System::Thread::getCurrent(), mp_cherry_, 100, 1, VIEWER_CHERRY1_END_VALUE, 1 );

    Viewer viewer_plum1( t1Mbox_, Kit::System::Thread::getCurrent(), mp_plum_, VIEWER_PLUM1_END_VALUE );
    Writer writer_plum1( t1Mbox_, Kit::System::Thread::getCurrent(), mp_plum_, 100, 1, VIEWER_PLUM1_END_VALUE, 1 );

    GenericViewer generic_viewer( t1Mbox_, Kit::System::Thread::getCurrent(), mp_apple_, mp_orange_, mp_cherry_, GENERIC_VIEWER_END_COUNT );


    // Open my viewers, writers
    viewer_apple1.open();
    viewer_orange1.open();
    viewer_cherry1.open();
    viewer_plum1.open();
    generic_viewer.open();
    writer_apple1.open();
    writer_orange1.open();
    writer_cherry1.open();
    writer_plum1.open();

    // Wait for everything to finish
    for ( int i = 0; i < NUM_INSTANCES; i++ )
    {
        Kit::System::Thread::wait();
    }

    REQUIRE( viewer_apple1.m_lastValue == VIEWER_APPLE1_END_VALUE );
    REQUIRE( viewer_orange1.m_lastValue == VIEWER_ORANGE1_END_VALUE );
    REQUIRE( viewer_cherry1.m_lastValue == VIEWER_CHERRY1_END_VALUE );
    REQUIRE( viewer_plum1.m_lastValue >= VIEWER_PLUM1_END_VALUE );
    REQUIRE( generic_viewer.m_mpNotificationCount >= ( GENERIC_VIEWER_END_COUNT / 2 ) );

    KIT_SYSTEM_TRACE_MSG( SECT_, "viewer_apple1(%p). m_lastValue=%" PRIu32 " (expected == %" PRIu32 ")", &viewer_apple1, viewer_apple1.m_lastValue, VIEWER_APPLE1_END_VALUE );
    KIT_SYSTEM_TRACE_MSG( SECT_, "viewer_orange1(%p). m_lastValue=%" PRIu32 " (expected == %" PRIu32 ")", &viewer_orange1, viewer_orange1.m_lastValue, VIEWER_ORANGE1_END_VALUE );
    KIT_SYSTEM_TRACE_MSG( SECT_, "viewer_cherry1(%p). m_lastValue=%" PRIu32 " (expected == %" PRIu32 ")", &viewer_cherry1, viewer_cherry1.m_lastValue, VIEWER_CHERRY1_END_VALUE );
    KIT_SYSTEM_TRACE_MSG( SECT_, "viewer_plum1(%p). m_lastValue=%" PRIu32 " (expected >= %" PRIu32 ")", &viewer_plum1, viewer_plum1.m_lastValue, VIEWER_PLUM1_END_VALUE );
    KIT_SYSTEM_TRACE_MSG( SECT_, "generic_viewer(%p). m_notifCount=%" PRIu32 " (expected >= %" PRIu32 ")", &generic_viewer, generic_viewer.m_mpNotificationCount, ( GENERIC_VIEWER_END_COUNT / 2 ) );

    // Close my viewers, writers,
    generic_viewer.close();
    viewer_apple1.close();
    viewer_orange1.close();
    viewer_cherry1.close();
    viewer_plum1.close();
    writer_apple1.close();
    writer_orange1.close();
    writer_cherry1.close();
    writer_plum1.close();

    // Shutdown threads
    t1Mbox_.pleaseStop();
    t2Mbox_.pleaseStop();
    Kit::System::sleep( 100 );  // allow time for threads to stop
    REQUIRE( t1->isActive() == false );
    REQUIRE( t2->isActive() == false );

    Kit::System::Thread::destroy( *t1 );
    Kit::System::Thread::destroy( *t2 );
    Kit::System::sleep( 100 );  // allow time for threads to stop BEFORE the runnable object goes out of scope
    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}
