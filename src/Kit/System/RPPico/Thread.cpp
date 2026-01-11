/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Thread.h"
#include "Kit/Memory/AlignedClass.h"
#include "Kit/System/Private.h"
#include "Kit/System/PrivateStartup.h"
#include "Kit/System/Api.h"
#include "Kit/System/Trace.h"
#include "Kit/System/Mutex.h"
#include "Kit/System/Assert.h"
#include "Kit/System/FatalError.h"
#include "hardware/watchdog.h"
#include "pico/multicore.h"
#include "pico/platform.h"
#include <new>

#define SECT_ "Kit::System::RPPico::Thread"

// Internal states
#define THREAD_STATE_DOES_NOT_EXIST 0
#define THREAD_STATE_ALLOCATED      1
#define THREAD_STATE_CREATED        2
#define THREAD_STATE_RUNNING        3

typedef Kit::Memory::AlignedClass<Kit::System::RPPico::Thread> ThreadMem_T;

// Private variables
static volatile bool                schedulingEnabled_;
static volatile unsigned            states_[KIT_SYSTEM_RPPICO_NUM_CORES];
static ThreadMem_T                  threadMemory_[KIT_SYSTEM_RPPICO_NUM_CORES];
static Kit::System::RPPico::Thread* threads_[KIT_SYSTEM_RPPICO_NUM_CORES];

////////////////////////////////////
static void core1Entry( void )
{
    multicore_lockout_victim_init();  // Enable SDK support on core1 for 'suspending scheduling'
    states_[1] = THREAD_STATE_RUNNING;
    threads_[1]->getRunnable().entry();  // Execute the Runnable object
    multicore_reset_core1();             // Self terminate if/when the Runnable object completes its processing
    states_[1] = THREAD_STATE_ALLOCATED;
}

inline static void launchCore1()
{
    multicore_launch_core1( core1Entry );
}

////////////////////////////////////
namespace {

// This class is used to allocate the actual thread instances.  In addition it
// has the side effect of turn the initial entry/native/main 'thread' into a
// Kit::System::Thread (i.e. adds the thread semaphore)
class RegisterInitHandler_ : public Kit::System::IStartupHook,
                             public Kit::System::IRunnable
{
protected:
    // Empty run function -- it is never called!
    void entry() noexcept override
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "ERROR: RegisterInitHandler_::entry() called - this should never happen!" );
    }

public:
    ///
    RegisterInitHandler_()
        : IStartupHook( SYSTEM )
    {
    }

protected:
    ///
    void notify( InitLevel init_level ) noexcept override
    {
        // Create a thread objects
        Kit::System::RPPico::Thread::createThreadInstance( 0, *this );
        Kit::System::RPPico::Thread::createThreadInstance( 1, *this );
        states_[0] = THREAD_STATE_ALLOCATED;
        states_[1] = THREAD_STATE_ALLOCATED;
    }
};
}  // end namespace

///
static RegisterInitHandler_ autoRegister_systemInit_hook_;


////////////////////////////////////
//------------------------------------------------------------------------------
namespace Kit {
namespace System {
namespace RPPico {

Thread::Thread( Kit::System::IRunnable& runnable, unsigned coreId ) noexcept
    : Kit::System::Thread( runnable )
    , m_coreId( coreId )
{
}

Thread::~Thread()
{
    // Nothing needed
}

void Thread::createThreadInstance( unsigned                coreId,
                                   Kit::System::IRunnable& runnable ) noexcept
{
    threads_[coreId] = new ( threadMemory_[coreId].m_byteMem ) Thread( runnable, coreId );
}

//////////////////////////////
int Thread::signal() noexcept
{
    return m_syncSema.signal();
}

int Thread::su_signal() noexcept
{
    return m_syncSema.su_signal();
}

const char* Thread::getName() const noexcept
{
    return m_coreId == 0 ? "CORE0" : "CORE1";
}


}  // end namespace
}
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
namespace Kit {
namespace System {

void enableScheduling() noexcept
{
    // Do nothing if called twice
    if ( !schedulingEnabled_ )
    {
        // Fail if the application has NOT 'created' core0 thread
        if ( states_[0] != THREAD_STATE_CREATED )
        {
            FatalError::log( Shutdown::eOSAL, "The Application has NOT created any threads" );
        }

        // Housekeeping
        schedulingEnabled_ = true;

        // start core1 if it has been created
        if ( states_[1] == THREAD_STATE_CREATED )
        {
            launchCore1();
        }

        // start core0
        multicore_lockout_victim_init();  // Enable SDK support on core0 for 'suspending scheduling'
        states_[0] = THREAD_STATE_RUNNING;
        threads_[0]->getRunnable().entry();

        // If thread0/core0 runs to completion - force a cold boot
        watchdog_enable( 1, 1 );
        while ( 1 )
        {
            ;
        }
    }
}

bool isSchedulingEnabled( void ) noexcept
{
    return schedulingEnabled_;
}


//////////////////////////////
Kit::System::Thread* Kit::System::Thread::tryGetCurrent() noexcept
{
    unsigned coreIdx = get_core_num();
    auto     state   = states_[coreIdx];
    if ( state == THREAD_STATE_RUNNING )
    {
        return threads_[coreIdx];
    }
    return nullptr;
}

void Kit::System::Thread::wait() noexcept
{
    unsigned coreIdx = get_core_num();
    threads_[coreIdx]->m_syncSema.wait();
}

bool Kit::System::Thread::tryWait() noexcept
{
    unsigned coreIdx = get_core_num();
    return threads_[coreIdx]->m_syncSema.tryWait();
}

bool Kit::System::Thread::timedWait( uint32_t timeout ) noexcept
{
    unsigned coreIdx = get_core_num();
    return threads_[coreIdx]->m_syncSema.timedWait( timeout );
}


//////////////////////////////
volatile bool g_kitCore1IsRunning;  // Needed for suspend/resume scheduling

Kit::System::Thread* Kit::System::Thread::create( IRunnable&  runnable,
                                                  const char* name,
                                                  int         priority,
                                                  int         stackSize,
                                                  void*       stackPtr,
                                                  bool        allowSimTicks ) noexcept
{
    // 'Create' the first thread
    if ( states_[0] == THREAD_STATE_ALLOCATED )
    {
        states_[0]              = THREAD_STATE_CREATED;
        threads_[0]->m_runnable = &runnable;
        return threads_[0];
    }

    // 'Create' the second thread
    else if ( states_[1] == THREAD_STATE_ALLOCATED )
    {
        states_[1]              = THREAD_STATE_CREATED;
        threads_[1]->m_runnable = &runnable;
        if ( schedulingEnabled_ )
        {
            Mutex::ScopeLock criticalSection( PrivateLocks::system() );
            g_kitCore1IsRunning = true;
            launchCore1();
        }
        return threads_[1];
    }

    // If I get here then, ALL threads have already be 'created' -->so the fail
    // the call
    return nullptr;
}

void Kit::System::Thread::destroy( Thread& threadToDestroy, uint32_t delayTimeMsToWaitIfActive ) noexcept
{
    // Ignore request to destroy thread0/core0 thread
    if ( threadToDestroy.getId() == 1 )
    {
        // Ignore if thread1/core1 is not running
        if ( states_[1] == THREAD_STATE_RUNNING )
        {
            // Wait for the thread to stop
            if ( delayTimeMsToWaitIfActive > 0 )
            {
                threadToDestroy.m_runnable->pleaseStop();
                threadToDestroy.timedWait( delayTimeMsToWaitIfActive );
            }

            // NOTE: In general it is not a good thing to "kill" threads - but to
            //       let the thread "run-to-completion", i.e. have the entry() method
            //       of the associated Runnable object complete.  If you do
            //       need to kill a thread - be dang sure that it is state such
            //       that it is ok to die - i.e. it has released all of its acquired
            //       resources: mutexes, semaphores, file handles, etc.
            {
                Mutex::ScopeLock criticalSection( PrivateLocks::system() );
                g_kitCore1IsRunning = false;
                multicore_reset_core1();
            }
            states_[1] = THREAD_STATE_ALLOCATED;
        }
    }
}

}  // end namespace
}
//------------------------------------------------------------------------------