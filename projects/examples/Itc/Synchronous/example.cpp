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
#include "Server.h"
#include "Kit/EventQueue/Server.h"
#include "Kit/System/Api.h"
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
namespace Itc {
namespace Synchronous {

static int                     exitCode_;
static Kit::EventQueue::Server mbox_;  // Note: The client and the server CAN NOT execute in the same thread
static Server                  myServer_( mbox_ );

static uint32_t flashRatesMs[] = { OPTION_ITC_SYNCHRONOUS_SERVER_MIN_FLASH_RATE_MS,
                                   OPTION_ITC_SYNCHRONOUS_SERVER_MIN_FLASH_RATE_MS * 2,
                                   OPTION_ITC_SYNCHRONOUS_SERVER_MIN_FLASH_RATE_MS * 4,
                                   OPTION_ITC_SYNCHRONOUS_SERVER_MIN_FLASH_RATE_MS * 8 };

//
int runExample( unsigned numCycles ) noexcept
{
    // Enable tracing
    KIT_SYSTEM_TRACE_ENABLE();
    // KIT_SYSTEM_TRACE_SET_INFO_LEVEL( Kit::System::Trace::eVERBOSE );
    KIT_SYSTEM_TRACE_SET_INFO_LEVEL( Kit::System::Trace::eINFO );
    KIT_SYSTEM_TRACE_ENABLE_SECTION( SECT_ );

    KIT_SYSTEM_TRACE_MSG( SECT_, "**** Starting ITC Synchronous Example... ****" );

    // Create the threads
    auto* t1 = Kit::System::Thread::create( mbox_, "APP" );
    KIT_SYSTEM_ASSERT( t1 != nullptr );

    // Open/start the server and client
    myServer_.open();

    // Outer loop
    while ( numCycles-- )
    {
        // cycle through a set of flash rates
        for ( unsigned idx = 0; idx < sizeof( flashRatesMs ) / sizeof( flashRatesMs[0] ); ++idx )
        {
            uint32_t flashRate = flashRatesMs[idx];
            KIT_SYSTEM_TRACE_MSG( SECT_, "Setting flash rate to %" PRIu32 " ms", flashRate );
            bool success = myServer_.set( flashRate );
            if ( success )
            {
                // Let the new flash rate run for a while
                Kit::System::sleep( flashRate * 2 * 10 );  // Run each flash rate for 10 full on/off cycles
            }
            else
            {
                KIT_SYSTEM_TRACE_MSG( SECT_, "Failed to set flash rate to %" PRIu32 " ms (idx=%u)", flashRate, idx );
            }
        }

        KIT_SYSTEM_TRACE_MSG( SECT_, "Finished flash rate cycling" );
    }

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
    ::Itc::Synchronous::exitCode_ = Kit::System::Shutdown::eSUCCESS;
    return ::Itc::Synchronous::exitCode_;
}

int Kit::System::Shutdown::failure( int exitCode ) noexcept
{
    ::Itc::Synchronous::exitCode_ = exitCode;
    return ::Itc::Synchronous::exitCode_;
}

//------------------------------------------------------------------------------