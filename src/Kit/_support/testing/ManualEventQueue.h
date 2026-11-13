#ifndef KIT_TESTING_SUPPORT_MANUAL_EVENT_QUEUE_H_
#define KIT_TESTING_SUPPORT_MANUAL_EVENT_QUEUE_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file

    This file defines an Event Queue that is run 'manually' by the unit test's
    thread, i.e. without an event-queue thread.  See ManualEventQueue.
 */

#include "Kit/EventQueue/Server.h"
#include "Kit/Itc/IOpenRequest.h"
#include "Kit/Itc/ICloseRequest.h"
#include "Kit/Itc/NullReturnHandler.h"


/** The default upper bound on the number of event-queue iterations that
    ManualEventQueue::runUntilIdle() performs before it gives up (i.e. it
    detects a busy-looping server)
 */
#ifndef OPTION_KIT_TESTING_MANUAL_EVENT_QUEUE_MAX_ITERATIONS
#define OPTION_KIT_TESTING_MANUAL_EVENT_QUEUE_MAX_ITERATIONS 100
#endif

/** Internal symbol (do NOT use directly): the 'pending work' predicate of the
    Event Queue's compile time variant (USE_KIT_EVENT_QUEUE_ITC_MSG_ONLY
    removes the Data Model change notifications from the Event Queue).  It is
    evaluated inside a ManualEventQueue member function.
 */
#ifndef USE_KIT_EVENT_QUEUE_ITC_MSG_ONLY
#define KIT_TESTING_MANUAL_EVENT_QUEUE_HAS_PENDING_WORK_() ( isPendingMessage() || m_changeHandler.isPendingChangingNotifications() )
#else
#define KIT_TESTING_MANUAL_EVENT_QUEUE_HAS_PENDING_WORK_() ( isPendingMessage() )
#endif


/** This class is an Event Queue (Kit::EventQueue::Server) that is NOT run by
    a thread.  Instead, the unit test runs it - on the test's own thread - by
    calling runUntilIdle().  The test therefore controls exactly when the code
    that executes in the Event Queue's context (ITC requests, Model Point
    change notifications, software timer callbacks) runs - and, for software
    timers, how much time has passed (see advanceTime()).  This makes tests
    deterministic: once runUntilIdle() returns true, the queue's servers have
    processed everything they are going to process, i.e. 'nothing happened'
    checks need no sleeps, and 'something happened' checks need no
    polling/timeouts.

    How to use:
        1. Construct a ManualEventQueue instead of a Kit::EventQueue::Server,
           and pass it to the object(s) under test as their Event Queue.  Do
           NOT create a thread for it.
        2. Open/close servers with requestOpen()/requestClose() - NOT with the
           server's synchronous open()/close() methods (see the 'Limitations'
           below).  Other ITC requests are sent with postAndRun().
        3. After every stimulus (e.g. writing a Model Point, adding an entry
           to a RingBufferMP), call runUntilIdle() and then check the results.
           Wrap the call in REQUIRE() - a false return value means the
           server never became idle (i.e. it is busy-looping).

    Example (a server that implements Kit::Itc::OpenCloseSync, and processes
    Model Point change notifications):
        \code

        #include "Kit/_support/testing/ManualEventQueue.h"

        TEST_CASE( "MyServer" )
        {
            ManualEventQueue eventQueue;                // No thread!
            MyServer         uut( eventQueue, mp_input_ );

            REQUIRE( eventQueue.requestOpen( uut ) );   // Instead of uut.open()

            mp_input_.write( 42 );                      // Stimulus...
            REQUIRE( eventQueue.runUntilIdle() );       // ...let the server react...
            REQUIRE( uut.m_count == 1 );                // ...and check the exact result

            mp_input_.write( 42 );                      // No change -->no notification
            REQUIRE( eventQueue.runUntilIdle() );
            REQUIRE( uut.m_count == 1 );                // Deterministic 'nothing happened' check

            REQUIRE( eventQueue.requestClose( uut ) );  // Instead of uut.close()
        }

        \endcode

    Example (any other ITC request - the result is read from the payload after
    the queue has processed the message):
        \code

        MyServer::MyPayload_T payload( ... );
        MyServer::MyMsg       msg( uut, payload, eventQueue.nullReturnHandler() );
        REQUIRE( eventQueue.postAndRun( msg ) );
        REQUIRE( payload.m_success );

        \endcode

    Example (software timers - time only advances when the test says so):
        \code

        ManualEventQueue eventQueue;
        MyWidget         uut( eventQueue );                     // Starts a 100 ms timer (Kit::System::Timer) on the queue

        REQUIRE( eventQueue.advanceTime( 99 ) );                // 99 ms later...
        REQUIRE( uut.m_timeoutCount == 0 );                     // ...not expired
        REQUIRE( eventQueue.advanceTime( 1 ) );                 // 100 ms...
        REQUIRE( uut.m_timeoutCount == 1 );                     // ...expired, exactly once
        REQUIRE( eventQueue.getElapsedTime() == 100 );

        \endcode

    Limitations:
        - Synchronous ITC calls (e.g. Kit::Itc::OpenCloseSync::open()/close(),
          or any other xxxSync API) block until a server thread processes the
          message - there is none, i.e. the call never returns.  Use
          requestOpen(), requestClose(), or postAndRun() instead.
        - Messages are sent with a 'null' return handler, i.e. the server's
          returnToSender() does nothing.  Read the results from the payload
          after the call.
        - Software timers (Kit::System::Timer and friends that use the queue
          as their Kit::System::ITimingSource) run on the queue's own virtual
          clock, which starts at zero and ONLY advances with advanceTime().
          Code that reads the system clock directly (e.g.
          Kit::System::ElapsedTime) still sees real time.
        - Event flags of the underlying Kit::System::EventLoop are NOT
          serviced.
        - The queue must only be run by ONE thread (the test thread).  Code
          under test that creates its own threads is not made deterministic.

    NOTE: The class is intended for unit tests only.
 */
class ManualEventQueue : public Kit::EventQueue::Server
{
public:
    /// Constructor.  See Kit::EventQueue::Server
    ManualEventQueue() noexcept
        : Kit::EventQueue::Server()
        , m_elapsedMs( 0 )
    {
    }

public:
    /** Processes the pending ITC messages and change notifications until there
        is no pending work, or 'maxIterations' iterations have been performed.
        Returns true when the queue is idle, i.e. false means the server(s)
        never stopped generating work (e.g. busy-looping).  The number of
        iterations performed is optionally returned via 'retIterations'.
     */
    bool runUntilIdle( unsigned  maxIterations = OPTION_KIT_TESTING_MANUAL_EVENT_QUEUE_MAX_ITERATIONS,
                       unsigned* retIterations = nullptr ) noexcept
    {
        unsigned iterations = 0;
        while ( iterations < maxIterations && hasPendingWork() )
        {
            serverProcessEvents();
            iterations++;
        }
        if ( retIterations )
        {
            *retIterations = iterations;
        }
        return !hasPendingWork();
    }

    /// Returns true if there are ITC messages and/or change notifications waiting to be processed
    bool hasPendingWork() noexcept
    {
        return KIT_TESTING_MANUAL_EVENT_QUEUE_HAS_PENDING_WORK_();
    }

public:
    /** Posts 'msg' (asynchronously) and runs the queue until idle.  Returns
        the result of runUntilIdle().  The message must be constructed with
        nullReturnHandler() (or another non-blocking return handler).
     */
    bool postAndRun( Kit::Itc::IMessage& msg ) noexcept
    {
        post( msg );
        return runUntilIdle();
    }

    /** Opens 'server' - the asynchronous replacement for the server's
        synchronous open() method.  Returns the server's open result, i.e.
        false when the server fails to open, or the queue does not become idle.
     */
    bool requestOpen( Kit::Itc::IOpenRequest& server, void* args = nullptr ) noexcept
    {
        Kit::Itc::IOpenRequest::OpenPayload_T payload( args );
        Kit::Itc::IOpenRequest::OpenMsg       msg( server, payload, m_nullReturnHandler );
        return postAndRun( msg ) && payload.success;
    }

    /** Closes 'server' - the asynchronous replacement for the server's
        synchronous close() method.  Returns the server's close result, i.e.
        false when the server fails to close, or the queue does not become
        idle.
     */
    bool requestClose( Kit::Itc::ICloseRequest& server, void* args = nullptr ) noexcept
    {
        Kit::Itc::ICloseRequest::ClosePayload_T payload( args );
        Kit::Itc::ICloseRequest::CloseMsg       msg( server, payload, m_nullReturnHandler );
        return postAndRun( msg ) && payload.success;
    }

    /// Returns a return handler for messages that are sent with postAndRun()
    Kit::Itc::IReturnHandler& nullReturnHandler() noexcept
    {
        return m_nullReturnHandler;
    }

public:
    /** Advances the queue's virtual clock by 'milliseconds', one millisecond
        at a time.  For each millisecond, the expired software timers' callbacks
        are called, and then the queue is run until idle (i.e. the work that
        the callbacks generate is processed before the clock advances again -
        the same order as a running Event Queue).  A periodic timer, i.e. one
        that restarts itself from its expired callback, therefore expires
        once per period.  Pending work is processed before the clock is
        advanced.  Returns false (immediately) if the queue does not become
        idle.
     */
    bool advanceTime( uint32_t milliseconds ) noexcept
    {
        if ( !runUntilIdle() )
        {
            return false;
        }
        for ( uint32_t i = 0; i < milliseconds; i++ )
        {
            // Advance the clock first, i.e. the expired callbacks see the time at which they expire
            m_elapsedMs++;
            tick( 1 );
            tickComplete();
            if ( !runUntilIdle() )
            {
                return false;
            }
        }
        return true;
    }

    /// Returns the queue's virtual clock, i.e. the total time advanced with advanceTime(), in milliseconds
    uint64_t getElapsedTime() const noexcept
    {
        return m_elapsedMs;
    }

    /** See Kit::System::ITimingSource.  One count is one millisecond of the
        virtual clock, i.e. the timer duration does NOT include the (real) time
        since the timers were last processed (the base class behavior).
     */
    uint32_t msecToCounts( uint32_t durationInMsecs ) const noexcept override
    {
        return durationInMsecs;
    }

protected:
    /// Return handler for the messages sent by the test (they are never returned)
    Kit::Itc::NullReturnHandler m_nullReturnHandler;

    /// Virtual clock (milliseconds)
    uint64_t m_elapsedMs;
};

#endif  // end header latch
