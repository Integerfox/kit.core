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
#include "Kit/_support/testing/ManualEventQueue.h"
#include "Kit/Dm/ModelDatabase.h"
#include "Kit/Dm/Mp/Uint32.h"
#include "Kit/Logging/Framework/LogSink.h"
#include "Kit/Container/RingBufferMPAllocate.h"
#include <string.h>

using namespace Kit::Logging::Framework;

#define SECT_           "_0test"

#define NUM_LOG_ENTRIES 10

namespace {  // Anonymous namespace for test support code

class MyUtt : public LogSink
{
public:
    unsigned numProcessed;
    uint64_t timestamps[NUM_LOG_ENTRIES];

public:
    MyUtt( Kit::EventQueue::IQueue& myEventQueue, Kit::Container::RingBufferMPAllocate<EntryData_T, NUM_LOG_ENTRIES + 1>& logFifo )
        : LogSink( myEventQueue, logFifo )
        , numProcessed( 0 )
        , timestamps{}
    {
    }

public:
    void dispatchLogEntry( Kit::Logging::Framework::EntryData_T& src ) noexcept override
    {
        if ( numProcessed < NUM_LOG_ENTRIES )
        {
            timestamps[numProcessed] = src.m_timestamp;
        }
        numProcessed++;
    }
};

}  // end anonymous namespace
////////////////////////////////////////////////////////////////////////////////

static void addEntries( Kit::Container::RingBufferMP<EntryData_T>& logFifo, unsigned numEntries, unsigned firstTimestamp = 0 )
{
    EntryData_T entry;
    for ( unsigned i = 0; i < numEntries; i++ )
    {
        entry.m_timestamp        = firstTimestamp + i;
        entry.m_classificationId = i;
        entry.m_packageId        = i;
        entry.m_subSystemId      = i;
        entry.m_messageId        = i;
        strcpy( entry.m_infoText, "Test log message" );
        REQUIRE( logFifo.add( entry ) == true );
    }
}

////////////////////////////////////////////////////////////////////////////////

//
// Note: The event queue is run by the test thread (no event-queue thread), i.e.
//       once runUntilIdle() returns true, the sink has processed everything it
//       is going to process.
//
TEST_CASE( "LogSink" )
{
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();

    // NOTE: The Model Point is local (and constructed before the sink) so that
    //       a failed REQUIRE can not leave the sink attached to a (static) MP
    Kit::Dm::ModelDatabase                                                     modelDb;
    Kit::Dm::Mp::Uint32                                                        mp_logFifoCount_( modelDb, "logFIFO" );
    Kit::Container::RingBufferMPAllocate<EntryData_T, ( NUM_LOG_ENTRIES + 1 )> logFifo( mp_logFifoCount_, false );  // IMPORTANT: Must set the initialize-memory flag to FALSE since the EntryData_T type has a non-trivial constructor
    ManualEventQueue                                                           eventQueue;
    MyUtt                                                                      uut( eventQueue, logFifo );
    mp_logFifoCount_.setInvalid();

    SECTION( "open/close - redundant calls succeed" )
    {
        REQUIRE( eventQueue.requestOpen( uut ) );
        REQUIRE( eventQueue.requestOpen( uut ) );
        REQUIRE( eventQueue.requestClose( uut ) );
        REQUIRE( eventQueue.requestClose( uut ) );
        REQUIRE( uut.numProcessed == 0 );
    }

    SECTION( "nothing is dispatched before open" )
    {
        addEntries( logFifo, 3 );
        REQUIRE( eventQueue.runUntilIdle() );
        REQUIRE( uut.numProcessed == 0 );
        REQUIRE( logFifo.getNumItems() == 3 );
    }

    SECTION( "open drains the existing entries" )
    {
        addEntries( logFifo, 3 );
        REQUIRE( eventQueue.requestOpen( uut ) );
        REQUIRE( uut.numProcessed == 3 );
        REQUIRE( logFifo.isEmpty() );
    }

    SECTION( "entries are dispatched in order, at most MAX_BATCH_WRITE per change notification" )
    {
        REQUIRE( eventQueue.requestOpen( uut ) );
        addEntries( logFifo, NUM_LOG_ENTRIES );

        // One change notification is processed per event-queue iteration
        REQUIRE( eventQueue.runUntilIdle( 1 ) == false );
        REQUIRE( uut.numProcessed == OPTION_KIT_LOGGING_FRAMEWORK_MAX_BATCH_WRITE );

        unsigned iterations = 0;
        REQUIRE( eventQueue.runUntilIdle( OPTION_KIT_TESTING_MANUAL_EVENT_QUEUE_MAX_ITERATIONS, &iterations ) );
        REQUIRE( iterations == ( NUM_LOG_ENTRIES - 1 ) / OPTION_KIT_LOGGING_FRAMEWORK_MAX_BATCH_WRITE );
        REQUIRE( uut.numProcessed == NUM_LOG_ENTRIES );
        REQUIRE( logFifo.isEmpty() );
        for ( unsigned i = 0; i < NUM_LOG_ENTRIES; i++ )
        {
            CAPTURE( i );
            REQUIRE( uut.timestamps[i] == i );
        }
    }

    SECTION( "less than a full batch is dispatched by a single change notification" )
    {
        REQUIRE( eventQueue.requestOpen( uut ) );
        addEntries( logFifo, OPTION_KIT_LOGGING_FRAMEWORK_MAX_BATCH_WRITE - 1 );

        unsigned iterations = 0;
        REQUIRE( eventQueue.runUntilIdle( OPTION_KIT_TESTING_MANUAL_EVENT_QUEUE_MAX_ITERATIONS, &iterations ) );
        REQUIRE( iterations == 1 );
        REQUIRE( uut.numProcessed == OPTION_KIT_LOGGING_FRAMEWORK_MAX_BATCH_WRITE - 1 );
        REQUIRE( logFifo.isEmpty() );
    }

    SECTION( "nothing is dispatched after close, and the backlog is drained on re-open" )
    {
        REQUIRE( eventQueue.requestOpen( uut ) );
        addEntries( logFifo, 2 );
        REQUIRE( eventQueue.runUntilIdle() );
        REQUIRE( uut.numProcessed == 2 );

        REQUIRE( eventQueue.requestClose( uut ) );
        addEntries( logFifo, 3, 2 );
        REQUIRE( eventQueue.runUntilIdle() );
        REQUIRE( uut.numProcessed == 2 );
        REQUIRE( logFifo.getNumItems() == 3 );

        REQUIRE( eventQueue.requestOpen( uut ) );
        REQUIRE( uut.numProcessed == 5 );
        REQUIRE( logFifo.isEmpty() );
        for ( unsigned i = 0; i < 5; i++ )
        {
            CAPTURE( i );
            REQUIRE( uut.timestamps[i] == i );
        }
    }

    SECTION( "draining the buffer does not trigger additional dispatches" )
    {
        REQUIRE( eventQueue.requestOpen( uut ) );
        addEntries( logFifo, 1 );

        // NOTE: Removing the entry changes the element count (to zero), i.e.
        //       the sink is notified again - and must dispatch nothing
        REQUIRE( eventQueue.runUntilIdle() );
        REQUIRE( eventQueue.hasPendingWork() == false );
        REQUIRE( uut.numProcessed == 1 );
        REQUIRE( logFifo.isEmpty() );
    }

    REQUIRE( eventQueue.requestClose( uut ) );
    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}
