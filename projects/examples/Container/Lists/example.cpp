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
#include "Flasher.h"
#include "Kit/System/Trace.h"
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
namespace Container {
namespace Lists {

static int exitCode_;

static OnPeriod  on1SecondA_( 1000 );
static OnPeriod  on1SecondB_( 1000 );
static OnPeriod  on500ms_( 500 );
static OnPeriod  on50ms_( 50 );
static OffPeriod off1Second_( 1000 );
static OffPeriod off500msA_( 500 );
static OffPeriod off500msB_( 500 );
static OffPeriod off50ms_( 50 );

static Flasher myFlasher_;        

int runExample( unsigned numCycles ) noexcept
{
    // Enable tracing
    KIT_SYSTEM_TRACE_ENABLE();
    KIT_SYSTEM_TRACE_SET_INFO_LEVEL( Kit::System::Trace::eINFO );
    KIT_SYSTEM_TRACE_ENABLE_SECTION( SECT_ );

    KIT_SYSTEM_TRACE_MSG( SECT_, "**** Starting Container Linked Lists Example... ****" );


    // Outer loop
    unsigned originalNumCycles = numCycles;
    while ( numCycles-- )
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "Starting Cycle (%u of %u)", originalNumCycles - numCycles, originalNumCycles );

        // Create 1st flash pattern: 1 sec on, 1 sec off
        Kit::Container::DList<Period> pattern;
        pattern.put( on1SecondA_ ); // Insert at the tail of the list
        pattern.put( off1Second_ );
        myFlasher_.runPattern( pattern ); // Note: the list is empty after this call


        // Create 2nd flash pattern: 2 sec on, 1.5 sec off, 50 ms on, 50 ms off
        pattern.put( on1SecondA_ );
        pattern.put( on1SecondB_ );
        pattern.put( off1Second_ );
        pattern.put( off500msA_ );
        pattern.put( on50ms_ );
        pattern.put( off50ms_ );
        myFlasher_.runPattern( pattern ); 

        // Create 3rd flash pattern: 50 ms on, 50 ms off
        pattern.put( on50ms_ );
        pattern.put( off50ms_ );
        myFlasher_.runPattern( pattern );

        KIT_SYSTEM_TRACE_MSG( SECT_, "Finished Cycle." );
    }


    // Shutdown the KIT library
    Kit::System::Shutdown::success();
    return exitCode_;
}


}  // end namespace
}


/*----------------------------------------------------------------------------*/
// Application specific shutdown handling.  See Kit::System::Shutdown for more details.

int Kit::System::Shutdown::success() noexcept
{
    ::Container::Lists::exitCode_ = Kit::System::Shutdown::eSUCCESS;
    return ::Container::Lists::exitCode_;
}

int Kit::System::Shutdown::failure( int exitCode ) noexcept
{
    ::Container::Lists::exitCode_ = exitCode;
    return ::Container::Lists::exitCode_;
}

//------------------------------------------------------------------------------