/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "test.h"
#include "Kit/TShell/Processor.h"
#include "Kit/TShell/NoSecurity.h"
#include "Kit/System/Api.h"
#include "Kit/System/Private.h"
#include "Kit/TShell/Command/Bye.h"
#include "Kit/TShell/Command/Help.h"
#include "Kit/TShell/Command/Echo.h"
#include "Kit/TShell/Command/Trace.h"
#include "Kit/TShell/Command/Wait.h"
#include "Kit/TShell/StdioThread.h"
#include "Kit/Job/Manager.h"
#include "Kit/Job/JobBase.h"
#include "Kit/Job/TShell/Cmd.h"
#include "Kit/EventQueue/Server.h"
#include "Kit/System/Thread.h"

#define SECT_ "_0test"

///
using namespace Kit::System;
using namespace Kit::TShell;
Kit::Container::OrderedList<Kit::TShell::ICommand> g_commandList( "ignore_static_constructor" );
Kit::Container::OrderedList<Kit::Job::IJob>         g_jobList( "ignore_static_constructor" );

namespace {

class TestJob : public Kit::Job::JobBase
{
public:
    TestJob( Kit::Container::OrderedList<Kit::Job::IJob>& jobList,
             const char*                                 name,
             const char*                                 description,
             const char*                                 usage ) noexcept
        : JobBase( jobList, name, description, usage )
    {
    }

protected:
    void initialize_() noexcept override {}

    bool start_( Kit::Job::IContext&, char* ) noexcept override
    {
        return setRunningState( true );
    }

    void stop_() noexcept override
    {
        setStoppedState();
    }

    void shutdown_() noexcept override {}
};

}  // end anonymous namespace

static Kit::EventQueue::Server jobEventQueue_;
static Kit::Job::Manager       jobManager_( jobEventQueue_, g_jobList );
static TestJob                 alphaJob_( g_jobList, "alpha", "Alpha test job", "alpha [args]" );
static TestJob                 betaJob_( g_jobList, "beta", "Beta test job", "beta [args]" );

static NoSecurity                                securityPolicy_;
static Kit::Framing::StreamSource                streamSrc_;
static Kit::Framing::StreamDestination           streamDst_;
static Processor                                 tshell_( g_commandList,
                                                         streamSrc_,
                                                         streamDst_,
                                                         securityPolicy_,
                                                         Kit::System::PrivateLocks::tracingOutput() );
static StdioThread                               stdioThread_( tshell_ );
static Command::Bye                              byeCmd_( g_commandList );
static Command::Help                             helpCmd_( g_commandList );
static Command::Echo                             echoCmd_( g_commandList );
static Command::Trace                            traceCmd_( g_commandList );
static Command::Wait                             waitCmd_( g_commandList );
static Kit::Job::TShell::Cmd                      jobsCmd_( g_commandList, jobManager_ );

void shell_test( Kit::Io::IInput& infd, Kit::Io::IOutput& outfd )
{
    auto* jobThread = Kit::System::Thread::create( jobEventQueue_, "JOBS" );
    KIT_SYSTEM_ASSERT( jobThread != nullptr );
    KIT_SYSTEM_ASSERT( jobManager_.open() );

    stdioThread_.launchTShell( infd, outfd );

    // Wait forever - the 'bye' command is responsible for exiting
    Kit::System::sleep( 0xFFFFFFFF );

    jobManager_.close();
    jobEventQueue_.pleaseStop();
    Kit::System::Thread::destroy( *jobThread );
}