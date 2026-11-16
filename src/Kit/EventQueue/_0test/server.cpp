/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file

    Tests for the Kit::EventQueue::Server specific behavior, i.e. the ITC
    contract checks and the forwarding of Event Flags to the underlying
    Kit::System::EventLoop.  Note: ITC messaging is tested by the Kit::Itc
    unit tests.
 */

#include "Kit/EventQueue/Server.h"
#include "Kit/Itc/OpenCloseSync.h"
#include "Kit/Itc/SyncReturnHandler.h"
#include "Kit/System/IEventFlag.h"
#include "Kit/System/Semaphore.h"
#include "Kit/System/Thread.h"
#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include "Kit/_support/testing/ManualEventQueue.h"
#include "Kit/_support/testing/helpers.h"
#include "catch2/catch_test_macros.hpp"
#include <atomic>

///
using namespace Kit::System;

#define MAX_WAIT_MS 5000

////////////////////////////////////////////////////////////////////////////////
namespace {

class NopService : public Kit::Itc::OpenCloseSync
{
public:
    NopService( Kit::EventQueue::IQueue& myEventQueue ) noexcept
        : Kit::Itc::OpenCloseSync( myEventQueue )
    {
    }

    void request( OpenMsg& msg ) noexcept override
    {
        msg.getPayload().success = true;
        msg.returnToSender();
    }

    void request( CloseMsg& msg ) noexcept override
    {
        msg.getPayload().success = true;
        msg.returnToSender();
    }
};

/// Service whose open request (which executes in the server's thread) calls postSync() on its own server
class SelfPostSyncService : public NopService
{
public:
    SelfPostSyncService( Kit::EventQueue::Server& myEventQueue ) noexcept
        : NopService( myEventQueue )
        , m_server( myEventQueue )
        , m_postSyncReturned( false )
    {
    }

    void request( OpenMsg& msg ) noexcept override
    {
        // Contract violation: a synchronous call to my own server
        Kit::Itc::ICloseRequest::ClosePayload_T payload;
        Kit::Itc::SyncReturnHandler             srh;
        CloseMsg                                closeMsg( *this, payload, srh );
        m_server.postSync( closeMsg );
        m_postSyncReturned = true;

        NopService::request( msg );
    }

    using NopService::request;

    Kit::EventQueue::Server& m_server;
    std::atomic<bool>        m_postSyncReturned;
};

/// Records its notifications.  Optionally signals a semaphore when notified
class EventFlag : public EventFlagBase
{
public:
    EventFlag( Kit::Container::SList<IEventFlag>& eventList, uint32_t eventMask, Semaphore* semaphore = nullptr ) noexcept
        : EventFlagBase( eventList, eventMask )
        , m_semaphore( semaphore )
        , m_count( 0 )
        , m_lastEvents( 0 )
    {
    }

    void notified( uint32_t eventMask ) noexcept override
    {
        m_lastEvents = eventMask;
        m_count++;
        if ( m_semaphore )
        {
            m_semaphore->signal();
        }
    }

    Semaphore*            m_semaphore;
    std::atomic<unsigned> m_count;
    std::atomic<uint32_t> m_lastEvents;
};

}  // end anonymous namespace


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Server" )
{
    ShutdownUnitTesting::clearAndUseCounter();

    SECTION( "pleaseStop() with pending ITC messages is a fatal error" )
    {
        ManualEventQueue uut;  // No thread: posted messages stay pending
        NopService       service( uut );

        Kit::Itc::IOpenRequest::OpenPayload_T payload;
        Kit::Itc::IOpenRequest::OpenMsg       msg( service, payload, uut.nullReturnHandler() );
        uut.post( msg );
        REQUIRE( uut.hasPendingWork() );

        uut.pleaseStop();
        REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 1u );

        // The pending message is NOT discarded
        REQUIRE( uut.runUntilIdle() );
        REQUIRE( payload.success );
    }

    SECTION( "pleaseStop() without pending ITC messages is NOT a fatal error" )
    {
        ManualEventQueue uut;
        uut.pleaseStop();
        REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
    }

    SECTION( "postSync() from the server's own thread is a fatal error - and does NOT deadlock" )
    {
        Kit::EventQueue::Server uut;
        SelfPostSyncService     service( uut );
        auto*                   t1 = Thread::create( uut, "SERVER" );
        REQUIRE( t1 != nullptr );

        // Post the open request asynchronously so that a deadlock fails the test (instead of hanging it)
        Semaphore                             openDone;
        Kit::Itc::IOpenRequest::OpenPayload_T payload;
        Kit::Itc::SyncReturnHandler           returnHandler( openDone );
        Kit::Itc::IOpenRequest::OpenMsg       msg( service, payload, returnHandler );
        uut.post( msg );
        REQUIRE( openDone.timedWait( MAX_WAIT_MS ) );
        REQUIRE( payload.success );
        REQUIRE( service.m_postSyncReturned );
        REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 1u );

        // No fatal error for a postSync() from a different thread
        REQUIRE( service.close() );
        REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );

        uut.pleaseStop();
        REQUIRE( waitThreadInactive( t1, MAX_WAIT_MS ) );
        Thread::destroy( *t1 );
    }

    SECTION( "event flags are delivered to the matching event flag handlers" )
    {
        Kit::Container::SList<IEventFlag> eventList;
        EventFlag                         flag3( eventList, 1u << 3 );
        EventFlag                         flags5and6( eventList, ( 1u << 5 ) | ( 1u << 6 ) );
        Semaphore                         sema;
        EventFlag                         sentinel( eventList, 0xFFFFFFFF, &sema );  // Last in the list, i.e. all other handlers have been processed when it is notified

        Kit::EventQueue::Server uut( OPTION_KIT_SYSTEM_EVENT_LOOP_TIMEOUT_PERIOD, &eventList );
        auto*                   t1 = Thread::create( uut, "SERVER" );
        REQUIRE( t1 != nullptr );

        uut.signalEvent( 3 );
        REQUIRE( sema.timedWait( MAX_WAIT_MS ) );
        REQUIRE( flag3.m_count == 1u );
        REQUIRE( flag3.m_lastEvents == ( 1u << 3 ) );
        REQUIRE( flags5and6.m_count == 0u );  // Not its event -->not notified

        // Handlers are only notified of their own event flags
        uut.signalMultipleEvents( ( 1u << 5 ) | ( 1u << 6 ) | ( 1u << 9 ) );
        REQUIRE( sema.timedWait( MAX_WAIT_MS ) );
        REQUIRE( flags5and6.m_count == 1u );
        REQUIRE( flags5and6.m_lastEvents == ( ( 1u << 5 ) | ( 1u << 6 ) ) );
        REQUIRE( flag3.m_count == 1u );
        REQUIRE( sentinel.m_lastEvents == ( ( 1u << 5 ) | ( 1u << 6 ) | ( 1u << 9 ) ) );

        // Supervisor/ISR variants
        uut.su_signalEvent( 6 );
        REQUIRE( sema.timedWait( MAX_WAIT_MS ) );
        REQUIRE( flags5and6.m_count == 2u );
        REQUIRE( flags5and6.m_lastEvents == ( 1u << 6 ) );

        uut.su_signalMultipleEvents( 1u << 3 );
        REQUIRE( sema.timedWait( MAX_WAIT_MS ) );
        REQUIRE( flag3.m_count == 2u );
        REQUIRE( flags5and6.m_count == 2u );

        // Every signal was delivered exactly once
        REQUIRE( sentinel.m_count == 4u );

        uut.pleaseStop();
        REQUIRE( waitThreadInactive( t1, MAX_WAIT_MS ) );
        Thread::destroy( *t1 );
    }

    REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
}
