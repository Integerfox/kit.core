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
#include "Kit/System/Semaphore.h"
#include "example.h"
#include "Client.h"
#include "Server.h"
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

#define SECT_ "main"

//------------------------------------------------------------------------------
namespace Dm {
namespace Transaction {

static Kit::EventQueue::Server mbox_;  // Note: The client and the server are NOT required to execute in same thread

// Semaphore used to wait for the shutdown request
static Kit::System::Semaphore waitForShutdown_;
static int                    exitCode_;

static Server myServer_( mbox_, mp::trigger );
static Client myClient_( mbox_, mp::trigger );

//
int runExample() noexcept
{
    // Enable tracing
    KIT_SYSTEM_TRACE_ENABLE();
    KIT_SYSTEM_TRACE_ENABLE_SECTION( SECT_ );

    KIT_SYSTEM_TRACE_MSG( SECT_, "**** Starting Dm Transaction Example... ****" );
    
    // Create the threads
    auto* t1 = Kit::System::Thread::create( mbox_, "APP" );
    KIT_SYSTEM_ASSERT( t1 != nullptr );

    // Open/start the server and client
    myServer_.open();
    myClient_.open();

    // Wait for shutdown request
    waitForShutdown_.wait();

    myClient_.close();
    myServer_.close();

    // Shutdown the KIT library
    Kit::System::Shutdown::notifyShutdownHandlers( exitCode_ );

    // terminate the application threads
    Kit::System::Thread::destroy( *t1, OPTION_WAIT_FOR_MBOX_STOPPED_TIMEOUT_MS );
    return exitCode_;
}


}  // end namespace
}


/*----------------------------------------------------------------------------*/
// Application specific shutdown handling.  See Kit::System::Shutdown for more details.

int Kit::System::Shutdown::success() noexcept
{
    ::Dm::Transaction::exitCode_ = Kit::System::Shutdown::eSUCCESS;
    ::Dm::Transaction::waitForShutdown_.signal();
    return ::Dm::Transaction::exitCode_;
}

int Kit::System::Shutdown::failure( int exitCode ) noexcept
{
    ::Dm::Transaction::exitCode_ = exitCode;
    ::Dm::Transaction::waitForShutdown_.signal();
    return ::Dm::Transaction::exitCode_;
}
//------------------------------------------------------------------------------