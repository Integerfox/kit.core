/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Job/Manager.h"
#include "Kit/Job/JobBase.h"
#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include "catch2/catch_test_macros.hpp"
#include "Kit/EventQueue/Server.h"
#include "Kit/System/Thread.h"
#include "Kit/Container/SList.h"

///
using namespace Kit::Job;

////////////////////////////////////////////////////////////////////////////////
// Anonymous namespace
namespace {

// Test Job that tracks how/when its package-scoped methods are invoked
class TestJob : public JobBase
{
public:
    TestJob( Kit::Container::SList<IJob>& jobList, const char* name ) noexcept
        : JobBase( jobList, name, "test job description", "test job usage" )
    {
    }

public:
    unsigned  initializeCount = 0;
    unsigned  shutdownCount   = 0;
    unsigned  startCount      = 0;
    unsigned  stopCount       = 0;
    bool      startResult     = true;
    IContext* lastContext     = nullptr;
    char*     lastArgs        = nullptr;

public:
    /// See Kit::Job::IJob
    void initialize_() noexcept override { initializeCount++; }

    /// See Kit::Job::IJob
    void shutdown_() noexcept override { shutdownCount++; }

    /// See Kit::Job::IJob
    bool start_( IContext& context, char* optionalArgs ) noexcept override
    {
        startCount++;
        lastContext = &context;
        lastArgs    = optionalArgs;
        return startResult;
    }

    /// See Kit::Job::IJob
    void stop_() noexcept override { stopCount++; }
};

// Test Job that self-terminates (i.e. calls IContext::completed()) from within its start_() method
class SelfCompletingJob : public JobBase
{
public:
    SelfCompletingJob( Kit::Container::SList<IJob>& jobList, const char* name ) noexcept
        : JobBase( jobList, name )
    {
    }

public:
    bool completedResultFirstCall  = false;
    bool completedResultSecondCall = true;

public:
    /// See Kit::Job::IJob
    void initialize_() noexcept override {}

    /// See Kit::Job::IJob
    void shutdown_() noexcept override {}

    /// See Kit::Job::IJob
    bool start_( IContext& context, char* optionalArgs ) noexcept override
    {
        ( void )optionalArgs;

        // First call: I am still in the Manager's 'started' list -> succeeds
        completedResultFirstCall = context.completed( *this );

        // Second call: I have already been removed from the 'started' list -> fails
        completedResultSecondCall = context.completed( *this );
        return true;
    }

    /// See Kit::Job::IJob
    void stop_() noexcept override {}
};

}  // end anonymous namespace


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Manager" )
{
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();

    Kit::Container::SList<IJob> jobList;
    TestJob                     job1( jobList, "job1" );
    TestJob                     job2( jobList, "job2" );

    Kit::EventQueue::Server uutEventLoop;
    Manager                 uut( uutEventLoop, jobList );

    auto* testThread = Kit::System::Thread::create( uutEventLoop, "TestThread" );
    REQUIRE( testThread != nullptr );

    SECTION( "open/close - happy path (and redundant open/close calls)" )
    {
        uut.open();
        REQUIRE( job1.initializeCount == 1 );
        REQUIRE( job2.initializeCount == 1 );

        // Already-opened -> no-op
        uut.open();
        REQUIRE( job1.initializeCount == 1 );
        REQUIRE( job2.initializeCount == 1 );

        uut.close();
        REQUIRE( job1.shutdownCount == 1 );
        REQUIRE( job2.shutdownCount == 1 );

        // Already-closed -> no-op
        uut.close();
        REQUIRE( job1.shutdownCount == 1 );
        REQUIRE( job2.shutdownCount == 1 );
    }

    SECTION( "close - stops and shuts down started jobs, restoring them to the inactive list" )
    {
        uut.open();

        char args[] = "";
        REQUIRE( uut.startJob( "job1", args ) == true );

        uut.close();
        REQUIRE( job1.stopCount == 1 );
        REQUIRE( job1.shutdownCount == 1 );
        REQUIRE( job2.shutdownCount == 1 );

        // Job1 was restored to the inactive list -> re-opening re-initializes it
        uut.open();
        REQUIRE( job1.initializeCount == 2 );
        REQUIRE( job2.initializeCount == 2 );

        uut.close();
    }

    SECTION( "startJob() - success" )
    {
        uut.open();

        char args[] = "some-args";
        bool result = uut.startJob( "job1", args );
        REQUIRE( result == true );
        REQUIRE( job1.startCount == 1 );
        REQUIRE( job1.lastArgs == args );
        REQUIRE( job1.lastContext != nullptr );

        unsigned        numFound = 0;
        Kit::Job::IJob* running[4];
        REQUIRE( uut.getRunningJobs( running, 4, numFound ) == true );
        REQUIRE( numFound == 1 );
        REQUIRE( running[0] == &job1 );

        uut.close();
    }

    SECTION( "startJob() - job name not found" )
    {
        uut.open();

        char args[] = "";
        bool result = uut.startJob( "no-such-job", args );
        REQUIRE( result == false );

        uut.close();
    }

    SECTION( "startJob() - nullptr args fails and restores the job to the inactive list" )
    {
        uut.open();

        bool result = uut.startJob( "job1", nullptr );
        REQUIRE( result == false );
        REQUIRE( job1.startCount == 0 );
        REQUIRE( uut.lookUpJob( "job1" ) == &job1 );

        uut.close();
    }

    SECTION( "startJob() - Job's start_() fails and restores the job to the inactive list" )
    {
        uut.open();
        job1.startResult = false;

        char args[] = "";
        bool result = uut.startJob( "job1", args );
        REQUIRE( result == false );
        REQUIRE( job1.startCount == 1 );

        unsigned        numFound = 0;
        Kit::Job::IJob* avail[4];
        REQUIRE( uut.getAvailableJobs( avail, 4, numFound ) == true );
        REQUIRE( numFound == 2 );

        uut.close();
    }

    SECTION( "stopJob() - success, then fails when stopped a second time" )
    {
        uut.open();

        char args[] = "";
        REQUIRE( uut.startJob( "job1", args ) == true );

        bool result = uut.stopJob( "job1" );
        REQUIRE( result == true );
        REQUIRE( job1.stopCount == 1 );

        result = uut.stopJob( "job1" );
        REQUIRE( result == false );
        REQUIRE( job1.stopCount == 1 );

        uut.close();
    }

    SECTION( "stopJob() - job name not found/not started" )
    {
        uut.open();

        bool result = uut.stopJob( "job1" );
        REQUIRE( result == false );

        result = uut.stopJob( "no-such-job" );
        REQUIRE( result == false );

        uut.close();
    }

    SECTION( "stopAllJobs() - no active jobs" )
    {
        uut.open();

        uut.stopAllJobs();
        REQUIRE( job1.stopCount == 0 );
        REQUIRE( job2.stopCount == 0 );

        uut.close();
    }

    SECTION( "stopAllJobs() - stops all started jobs" )
    {
        uut.open();

        char args[] = "";
        REQUIRE( uut.startJob( "job1", args ) == true );
        REQUIRE( uut.startJob( "job2", args ) == true );

        uut.stopAllJobs();
        REQUIRE( job1.stopCount == 1 );
        REQUIRE( job2.stopCount == 1 );

        unsigned        numFound = 0;
        Kit::Job::IJob* running[4];
        REQUIRE( uut.getRunningJobs( running, 4, numFound ) == true );
        REQUIRE( numFound == 0 );

        uut.close();
    }

    SECTION( "getAvailableJobs() - truncated (inactive list alone overflows the client's buffer)" )
    {
        uut.open();

        unsigned        numFound = 0;
        Kit::Job::IJob* avail[1];
        bool            result = uut.getAvailableJobs( avail, 1, numFound );
        REQUIRE( result == false );
        REQUIRE( numFound == 1 );

        uut.close();
    }

    SECTION( "getAvailableJobs() - truncated while walking the started list" )
    {
        uut.open();

        char args[] = "";
        REQUIRE( uut.startJob( "job1", args ) == true );

        unsigned        numFound = 0;
        Kit::Job::IJob* avail[1];
        bool            result = uut.getAvailableJobs( avail, 1, numFound );
        REQUIRE( result == false );
        REQUIRE( numFound == 1 );

        uut.close();
    }

    SECTION( "getAvailableJobs() - success across both the inactive and started lists" )
    {
        uut.open();

        char args[] = "";
        REQUIRE( uut.startJob( "job1", args ) == true );

        unsigned        numFound = 0;
        Kit::Job::IJob* avail[4];
        bool            result = uut.getAvailableJobs( avail, 4, numFound );
        REQUIRE( result == true );
        REQUIRE( numFound == 2 );

        uut.close();
    }

    SECTION( "getRunningJobs() - truncated" )
    {
        uut.open();

        char args[] = "";
        REQUIRE( uut.startJob( "job1", args ) == true );
        REQUIRE( uut.startJob( "job2", args ) == true );

        unsigned        numFound = 0;
        Kit::Job::IJob* running[1];
        bool            result = uut.getRunningJobs( running, 1, numFound );
        REQUIRE( result == false );
        REQUIRE( numFound == 1 );

        uut.close();
    }

    SECTION( "getRunningJobs() - success" )
    {
        uut.open();

        char args[] = "";
        REQUIRE( uut.startJob( "job1", args ) == true );

        unsigned        numFound = 0;
        Kit::Job::IJob* running[4];
        bool            result = uut.getRunningJobs( running, 4, numFound );
        REQUIRE( result == true );
        REQUIRE( numFound == 1 );
        REQUIRE( running[0] == &job1 );

        uut.close();
    }

    SECTION( "lookUpJob() - found in the inactive list, found in the started list, and not found" )
    {
        uut.open();

        REQUIRE( uut.lookUpJob( "job1" ) == &job1 );

        char args[] = "";
        REQUIRE( uut.startJob( "job1", args ) == true );
        REQUIRE( uut.lookUpJob( "job1" ) == &job1 );

        REQUIRE( uut.lookUpJob( "does-not-exist" ) == nullptr );

        uut.close();
    }

    SECTION( "IContext - getWorkBuffer0()/getWorkBuffer1() return distinct, usable buffers" )
    {
        uut.open();

        Kit::Text::IString& buf0 = uut.getWorkBuffer0();
        Kit::Text::IString& buf1 = uut.getWorkBuffer1();
        buf0                     = "hello";
        buf1                     = "world";
        REQUIRE( buf0 == "hello" );
        REQUIRE( buf1 == "world" );

        uut.close();
    }

    SECTION( "IContext - completed() self-terminates a running job" )
    {
        SelfCompletingJob selfJob( jobList, "self-job" );
        uut.open();

        char args[] = "";
        REQUIRE( uut.startJob( "self-job", args ) == true );
        REQUIRE( selfJob.completedResultFirstCall == true );
        REQUIRE( selfJob.completedResultSecondCall == false );

        unsigned        numFound = 0;
        Kit::Job::IJob* running[4];
        REQUIRE( uut.getRunningJobs( running, 4, numFound ) == true );
        REQUIRE( numFound == 0 );
        REQUIRE( uut.lookUpJob( "self-job" ) == &selfJob );

        uut.close();
    }

    // Shutdown threads
    uutEventLoop.pleaseStop();
    Kit::System::Thread::destroy( *testThread );

    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}
