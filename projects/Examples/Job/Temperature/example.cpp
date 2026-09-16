/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Job/IJob.h"
#include "kit_config.h"
#include "Kit/System/Semaphore.h"
#include "example.h"
#include "Temperature.h"
#include "ModelPoints.h"
#include "Kit/EventQueue/Server.h"
#include "Kit/System/Thread.h"
#include "Kit/System/Trace.h"
#include "Kit/System/Assert.h"
#include "Kit/System/Shutdown.h"
#include "Kit/System/Private.h"
#include "Kit/TShell/Processor.h"
#include "Kit/TShell/StdioThread.h"
#include "Kit/TShell/NoSecurity.h"
#include "Kit/TShell/Command/Bye.h"
#include "Kit/TShell/Command/Help.h"
#include "Kit/TShell/Command/Echo.h"
#include "Kit/TShell/Command/Trace.h"
#include "Kit/TShell/Command/Wait.h"
#include "Kit/Dm/TShell/Read.h"
#include "Kit/Dm/TShell/Write.h"
#include "Kit/Job/Manager.h"
#include "Kit/Job/TShell/Cmd.h"


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
namespace Job {
namespace Temperature {

// Semaphore used to wait for the shutdown request
static Kit::System::Semaphore waitForShutdown_;
static int                    exitCode_;

/// Event queue/loop for executing Jobs
static Kit::EventQueue::Server jobEventQueue_;

/// Jobs
static Kit::Container::OrderedList<Kit::Job::IJob> jobList_;
static Kit::Job::Manager                           jobManager_( jobEventQueue_, jobList_ );
static Temperature                                 temperature_( jobList_, mp::tempSensor1, "inletTemp" );
static Temperature                                 temperature2_( jobList_, mp::tempSensor2, "outletTemp" );
static Kit::Job::TShell::Cmd                       jobCmd_( g_commandList, jobManager_ );

/// TShell command infrastructure
Kit::Container::OrderedList<Kit::TShell::ICommand> g_commandList( "ignore_static_constructor" );
static Kit::TShell::NoSecurity                     securityPolicy_;
static Kit::Framing::StreamSource                  streamSrc_;
static Kit::Framing::StreamDestination             streamDst_;
//
static Kit::TShell::Processor tshell_( g_commandList,
                                       streamSrc_,
                                       streamDst_,
                                       securityPolicy_,
                                       Kit::System::PrivateLocks::tracingOutput() );
//
static Kit::TShell::StdioThread    stdioThread_( tshell_ );
static Kit::TShell::Command::Bye   byeCmd_( g_commandList );
static Kit::TShell::Command::Help  helpCmd_( g_commandList );
static Kit::TShell::Command::Echo  echoCmd_( g_commandList );
static Kit::TShell::Command::Trace traceCmd_( g_commandList );
static Kit::TShell::Command::Wait  waitCmd_( g_commandList );
static Kit::Dm::TShell::Write      dmWriteCmd_( g_commandList, mp::g_modelDatabase );
static Kit::Dm::TShell::Read       dmReadCmd_( g_commandList, mp::g_modelDatabase );

//
int runExample( Kit::Io::IInput& infd, Kit::Io::IOutput& outfd ) noexcept
{
    // Enable tracing
    KIT_SYSTEM_TRACE_ENABLE();
    KIT_SYSTEM_TRACE_ENABLE_SECTION( SECT_ );
    KIT_SYSTEM_TRACE_ENABLE_SECTION( OPTION_KIT_JOB_TRACE_SECTION );
    KIT_SYSTEM_TRACE_MSG( SECT_, "**** Starting Job Temperature Example... ****" );

    // Create the threads
    auto* t1 = Kit::System::Thread::create( jobEventQueue_, "JOBS" );
    KIT_SYSTEM_ASSERT( t1 != nullptr );

    // Some initial values for the temperature
    mp::tempSensor1.write( 25.0 );
    mp::tempSensor2.write( 30.0 );

    // Start the Job Manager
    jobManager_.open();

    // Start the Command console
    stdioThread_.launchTShell( infd, outfd );

    // Wait for shutdown request
    waitForShutdown_.wait();

    // Stop the Job Manager
    jobManager_.close();

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
    Examples::Job::Temperature::exitCode_ = Kit::System::Shutdown::eSUCCESS;
    Examples::Job::Temperature::waitForShutdown_.signal();
    return Examples::Job::Temperature::exitCode_;
}

int Kit::System::Shutdown::failure( int exitCode ) noexcept
{
    Examples::Job::Temperature::exitCode_ = exitCode;
    Examples::Job::Temperature::waitForShutdown_.signal();
    return Examples::Job::Temperature::exitCode_;
}
//------------------------------------------------------------------------------