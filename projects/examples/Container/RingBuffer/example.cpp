/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "kit_config.h"
#include "Container/RingBuffer/FlashCode.h"
#include "Kit/Container/RingBufferMPAllocate.h"
#include "Kit/System/Semaphore.h"
#include "example.h"
#include "LedServer.h"
#include "Producer.h"
#include "ModelPoints.h"
#include "Kit/EventQueue/Server.h"
#include "Kit/System/Thread.h"
#include "Kit/System/Trace.h"
#include "Kit/System/Assert.h"
#include "Kit/System/Shutdown.h"

/// Time, in milliseconds, to wait for runnable object to stop.
//  NOTE: the `destroy()` method calls `pleaseStop()` on the runnable
//        object and then waits for the runnable to stop.  If the runnable does
//        not stop within the specified time, then the thread is terminated
//        regardless of the runnable's state.
#ifndef OPTION_WAIT_FOR_MBOX_STOPPED_TIMEOUT_MS
#define OPTION_WAIT_FOR_MBOX_STOPPED_TIMEOUT_MS 1000  // 1sec
#endif

/// Number of 'fill cycles' to perform
#ifndef OPTION_NUM_FILL_CYCLES
#define OPTION_NUM_FILL_CYCLES UINT_MAX // No limit on the number of fill cycles
#endif

#define SECT_ "main"

#ifndef OPTION_FLASHCODE_QUEUE_MAX_ITEMS
#define OPTION_FLASHCODE_QUEUE_MAX_ITEMS 10
#endif
//------------------------------------------------------------------------------
namespace Container {
namespace RingBuffer {

static Kit::EventQueue::Server mboxLedServer_;
static Kit::EventQueue::Server mboxProducer_;

// Semaphore used to wait for the shutdown request
static Kit::System::Semaphore waitForShutdown_;
static int                    exitCode_;

// Ring buffer (add '1' to the raw element count to account for the Ring Buffer 'empty' vs 'full' state ambiguity)
static Kit::Container::RingBufferMPAllocate<FlashCode_T, OPTION_FLASHCODE_QUEUE_MAX_ITEMS+1> flashCodeQueue( mp::flashCodeQueueElementCount );

//
static LedServer ledServer_( mboxLedServer_, flashCodeQueue );
static Producer  flashCodesProducer_( mboxProducer_, flashCodeQueue, OPTION_NUM_FILL_CYCLES );

//
int runExample() noexcept
{
    // Enable tracing
    KIT_SYSTEM_TRACE_ENABLE();
    KIT_SYSTEM_TRACE_ENABLE_SECTION( SECT_ );

    KIT_SYSTEM_TRACE_MSG( SECT_, "**** Starting Container Ring Buffer Example... ****" );

    // Create the threads
    auto* t1 = Kit::System::Thread::create( mboxLedServer_, "LED" );
    KIT_SYSTEM_ASSERT( t1 != nullptr );
    auto* t2 = Kit::System::Thread::create( mboxProducer_, "PRODUCER" );
    KIT_SYSTEM_ASSERT( t2 != nullptr );

    // Open/start the led server and flash codes producer
    ledServer_.open();
    flashCodesProducer_.open();

    // Wait for shutdown request
    waitForShutdown_.wait();

    flashCodesProducer_.close();
    ledServer_.close();

    // Shutdown the KIT library
    Kit::System::Shutdown::notifyShutdownHandlers( exitCode_ );

    // terminate the application threads
    Kit::System::Thread::destroy( *t1, OPTION_WAIT_FOR_MBOX_STOPPED_TIMEOUT_MS );
    Kit::System::Thread::destroy( *t2, OPTION_WAIT_FOR_MBOX_STOPPED_TIMEOUT_MS );
    return exitCode_;
}


}  // end namespace
}


/*----------------------------------------------------------------------------*/
// Application specific shutdown handling.  See Kit::System::Shutdown for more details.

int Kit::System::Shutdown::success() noexcept
{
    ::Container::RingBuffer::exitCode_ = Kit::System::Shutdown::eSUCCESS;
    ::Container::RingBuffer::waitForShutdown_.signal();
    return ::Container::RingBuffer::exitCode_;
}

int Kit::System::Shutdown::failure( int exitCode ) noexcept
{
    ::Container::RingBuffer::exitCode_ = exitCode;
    ::Container::RingBuffer::waitForShutdown_.signal();
    return ::Container::RingBuffer::exitCode_;
}
//------------------------------------------------------------------------------