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
#include "Kit/Dm/Mp/_0test/common.h"
#include "Kit/EventQueue/Server.h"
#include "Kit/Dm/ModelDatabase.h"
#include "Kit/Text/FString.h"
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

public:
    MyUtt( Kit::EventQueue::IQueue& myMbox, Kit::Container::RingBufferMPAllocate<EntryData_T, NUM_LOG_ENTRIES+1>& logFifo )
        : LogSink( myMbox, logFifo )
        , numProcessed( 0 )
    {
    }
public:
    void dispatchLogEntry( Kit::Logging::Framework::EntryData_T& src ) noexcept override
    {
        numProcessed++;
    }
};

}  // end anonymous namespace
////////////////////////////////////////////////////////////////////////////////

// Allocate/create my Model Database
static Kit::Dm::ModelDatabase modelDb_( "ignoreThisParameter_usedToInvokeTheStaticConstructor" );

// Allocate my Model Points
static Kit::Dm::Mp::Uint32 mp_logFifoCount_( modelDb_, "logFIFO" );


// Don't let the Runnable object go out of scope before its thread has actually terminated!
static Kit::EventQueue::Server t1Mbox_;

////////////////////////////////////////////////////////////////////////////////

//
// Note: The bare minimum I need to test code that is 'new' to concrete MP type
//
TEST_CASE( "LogSink" )
{
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();
    Kit::Container::RingBufferMPAllocate<EntryData_T, (NUM_LOG_ENTRIES + 1)> logFifo( mp_logFifoCount_ );
    MyUtt                                                                  uut( t1Mbox_, logFifo );
    mp_logFifoCount_.setInvalid();

    Kit::System::Thread* t1 = Kit::System::Thread::create( t1Mbox_, "UUT" );
    REQUIRE( t1 != nullptr );

    SECTION( "open/close" )
    {
        uut.open();
        uut.open();
        uut.close();
        uut.close();
    }

    SECTION( "nominal" )
    {
        uut.open();

        EntryData_T entry;
        for ( unsigned i = 0; i < NUM_LOG_ENTRIES; i++ )
        {
            entry.m_timestamp        = i;
            entry.m_classificationId = i;
            entry.m_packageId        = i;
            entry.m_subSystemId      = i;
            entry.m_messageId        = i;
            strcpy( entry.m_infoText, "Test log message" );
            REQUIRE( logFifo.add( entry ) == true );
        }

        // Allow time for the LogSink to process the entries
        Kit::System::sleep( 300 );
        REQUIRE( uut.numProcessed == NUM_LOG_ENTRIES );

        uut.close();
    }

        SECTION( "nominal" )
    {
        uut.open();

        EntryData_T entry;
        for ( unsigned i = 0; i < OPTION_KIT_LOGGING_FRAMEWORK_MAX_BATCH_WRITE-1; i++ )
        {
            entry.m_timestamp        = i;
            entry.m_classificationId = i;
            entry.m_packageId        = i;
            entry.m_subSystemId      = i;
            entry.m_messageId        = i;
            strcpy( entry.m_infoText, "Test log message" );
            REQUIRE( logFifo.add( entry ) == true );
        }

        // Allow time for the LogSink to process the entries
        Kit::System::sleep( 300 );
        REQUIRE( uut.numProcessed == OPTION_KIT_LOGGING_FRAMEWORK_MAX_BATCH_WRITE-1 );

        uut.close();
    }

    // Shutdown thread(s)
    t1Mbox_.pleaseStop();
    WAIT_FOR_THREAD_TO_STOP( t1 );
    Kit::System::Thread::destroy( *t1 );

    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}