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
#include "example.h"
#include "Client.h"
#include "Server.h"
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
namespace Examples {
namespace Itc {
namespace AsyncCancel {

static FlashPattern_T flashRatesMs[] = { { OPTION_EXAMPLES_ITC_ASYNCCANCEL_SERVER_MIN_FLASH_RATE_MS * 1, OPTION_EXAMPLES_ITC_ASYNCCANCEL_SERVER_MIN_FLASH_RATE_MS, 10 },
                                         { OPTION_EXAMPLES_ITC_ASYNCCANCEL_SERVER_MIN_FLASH_RATE_MS * 2, OPTION_EXAMPLES_ITC_ASYNCCANCEL_SERVER_MIN_FLASH_RATE_MS, 10 },
                                         { OPTION_EXAMPLES_ITC_ASYNCCANCEL_SERVER_MIN_FLASH_RATE_MS * 4, OPTION_EXAMPLES_ITC_ASYNCCANCEL_SERVER_MIN_FLASH_RATE_MS * 2, 10 },
                                         { OPTION_EXAMPLES_ITC_ASYNCCANCEL_SERVER_MIN_FLASH_RATE_MS * 8, OPTION_EXAMPLES_ITC_ASYNCCANCEL_SERVER_MIN_FLASH_RATE_MS * 4, 10 },
                                         { OPTION_EXAMPLES_ITC_ASYNCCANCEL_SERVER_MIN_FLASH_RATE_MS * 20, OPTION_EXAMPLES_ITC_ASYNCCANCEL_SERVER_MIN_FLASH_RATE_MS * 10, 10 } };

static int                     exitCode_;
static Kit::EventQueue::Server mbox_;  // Note: The client and the server CAN NOT execute in the same thread
static Server                  myServer_( mbox_ );
static IFlashRequest::SAP      serverSAP_( myServer_, mbox_ );
static Kit::System::Semaphore  shutdownSignal_;
static Client                  client_( mbox_, serverSAP_, flashRatesMs, sizeof( flashRatesMs ) / sizeof( flashRatesMs[0] ), shutdownSignal_ );

//
int runExample( unsigned numCycles ) noexcept
{
    // Enable tracing
    KIT_SYSTEM_TRACE_ENABLE();
    KIT_SYSTEM_TRACE_ENABLE_SECTION( SECT_ );

    KIT_SYSTEM_TRACE_MSG( SECT_, "**** Starting ITC AsyncCancel Example... ****" );

    // Create the thread
    auto* t1 = Kit::System::Thread::create( mbox_, "APP" );
    KIT_SYSTEM_ASSERT( t1 != nullptr );

    // Open/start the server and client
    myServer_.open();
    client_.open( reinterpret_cast<void*>( numCycles ) );

    // Wait for the client to finish its work
    // Kit::System::sleep( 6 * 1000 ); // Use when testing cancel behavior and comment out .wait() call
    shutdownSignal_.wait();
    KIT_SYSTEM_TRACE_MSG( SECT_, "MAIN: Finished flash rate cycling" );

    // Close in the inverse order of the open() calls
    client_.close();
    myServer_.close();

    // Shutdown the KIT library
    Kit::System::Shutdown::notifyShutdownHandlers( exitCode_ );

    // terminate the application threads
    Kit::System::Thread::destroy( *t1, OPTION_WAIT_FOR_MBOX_STOPPED_TIMEOUT_MS );
    return exitCode_;
}


}  // end namespace
}
}

/*----------------------------------------------------------------------------*/
// Application specific shutdown handling.  See Kit::System::Shutdown for more details.

int Kit::System::Shutdown::success() noexcept
{
    Examples::Itc::AsyncCancel::exitCode_ = Kit::System::Shutdown::eSUCCESS;
    return Examples::Itc::AsyncCancel::exitCode_;
}

int Kit::System::Shutdown::failure( int exitCode ) noexcept
{
    Examples::Itc::AsyncCancel::exitCode_ = exitCode;
    return Examples::Itc::AsyncCancel::exitCode_;
}

//------------------------------------------------------------------------------