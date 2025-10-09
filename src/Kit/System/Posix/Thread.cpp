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
#include "Kit/System/Api.h"
#include "Kit/System/PrivateStartup.h"
#include "Kit/System/Shutdown.h"
#include "Kit/System/Thread.h"
#include "Kit/System/Trace.h"
#include "Kit/System/FatalError.h"
#include "Kit/System/SimTick.h"
#include <limits.h>
#include <sched.h>
#include <unistd.h>
#include <stdio.h>
#include <algorithm>


// Initialize static class variables
pthread_key_t tsdKey_;
bool          keyCreated_ = false;

#define SECT_ "Kit::System::Posix::Thread"

////------------------------------------------------------------------------------
namespace Kit {
namespace System {
namespace Posix {


////////////////////////////////////
namespace {  // Anonymous namespace

/// This class is used to turn the entry/native/main thread into a Kit::System::Thread (i.e. add the thread semaphore)
class RegisterInitHandler_ : public Kit::System::IStartupHook,
                             public Kit::System::IRunnable
{
protected:
    // Empty entry function -- it is never called!
    void entry() noexcept override {}

public:
    ///
    RegisterInitHandler_()
        : IStartupHook( SYSTEM ) {}


protected:
    ///
    void notify( InitLevel initLevel ) noexcept override
    {
        // Create a thread object for the native thread
        m_parentThreadPtr_ = new Thread( *this );
    }
};
}  // end namespace

///
static RegisterInitHandler_ autoRegister_systemInit_hook_;


////////////////////////////////////
Thread::Thread( Kit::System::IRunnable& dummyRunnable )
    : Kit::System::Thread( dummyRunnable )
    , m_name( "PosixMain" )
{
    // Create TSD key that holds a pointer to "me" (only do this once)
    // NOTE: By definition/design this constructor will be CALLED before
    //       the 'normal' thread constructor and will ONLY CALLED ONCE.
    if ( pthread_key_create( &tsdKey_, NULL ) != 0 )
    {
        Kit::System::FatalError::logRaw( Shutdown::eOSAL, "Posix::Thread::Thread().  pthread_key_create() failed when creating index to store the current Thread Pointer instance." );
    }

    // Set my Tls index/key 'created' status to good
    keyCreated_ = true;

    // Store a reference to the Kit thread object for the native thread
    pthread_setspecific( tsdKey_, this );

    // Add the native thread to the list of active threads
    addThreadToActiveList( *this );

    // Mark the NATIVE/Main thread as 'real time' thread for the SimTick engine
    KIT_SYSTEM_THREAD_SET_SIM_TICK_FLAG( false );
    KIT_SYSTEM_SIM_TICK_THREAD_INIT_( false );  
}


Thread::Thread( Kit::System::IRunnable& runnable,
                const char*             name,
                int                     priority,
                unsigned                stackSize,
                int                     schedType,
                bool                    allowSimTicks )
    : Kit::System::Thread( runnable )
    , m_name( name )
{
    KIT_SYSTEM_THREAD_SET_SIM_TICK_FLAG( allowSimTicks );
    
    KIT_SYSTEM_TRACE_FUNC( SECT_ );
    KIT_SYSTEM_TRACE_MSG( SECT_, "Name=%s, pri=%d, schedType=%d", name, priority, schedType );

    // Calculate run time priority
    struct sched_param threadPriority;
    int                platMinPriority  = sched_get_priority_min( schedType );
    int                platMaxPriority  = sched_get_priority_max( schedType );
    priority                           += platMinPriority;
    priority                            = std::min( priority, platMaxPriority );
    priority                            = std::max( priority, platMinPriority );
    threadPriority.sched_priority       = priority;
    KIT_SYSTEM_TRACE_MSG( SECT_,
                          "platMinPri=%d platMaxPri=%d, runTimePri=%d",
                          platMinPriority,
                          platMaxPriority,
                          priority );

    // Setup to access the Thread Attributes
    pthread_attr_t thread_attr;
    int            rc_init = pthread_attr_init( &thread_attr );

    // Get the default Stack size
    size_t defaultStackSize = 0;
    pthread_attr_getstacksize( &thread_attr, &defaultStackSize );
    if ( stackSize == 0 )
    {
        stackSize = defaultStackSize;
    }

    // Initialize Thread Attributes
    int rc_setdetach   = pthread_attr_setdetachstate( &thread_attr, PTHREAD_CREATE_DETACHED );  // This is to prevent memory leaks
    int rc_setstack    = pthread_attr_setstacksize( &thread_attr, std::max( PTHREAD_STACK_MIN, static_cast<long>( stackSize ) ) );
    int rc_schedpolicy = pthread_attr_setschedpolicy( &thread_attr, schedType );
    int rc_schedparam  = pthread_attr_setschedparam( &thread_attr, &threadPriority );
    KIT_SYSTEM_TRACE_MSG( SECT_,
                          "pthread_attr_* Results (0==good): _init()=%d, _setdetachedstate=%d, _setstacksize=%d, _setschedpolicy=%d, _setschedparam=%d, stacksize=%ld",
                          rc_init,
                          rc_setdetach,
                          rc_setstack,
                          rc_schedpolicy,
                          rc_schedparam,
                          std::max( PTHREAD_STACK_MIN, static_cast<long>( stackSize ) ) );
    if ( rc_init || rc_setdetach || rc_setstack || rc_schedpolicy || rc_schedparam )
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "Kit::System::Posix::Thread. unexpected error when creating thread %s. Returns codes (all should be zero): attr_init=%d, setdetach=%d, setstack=%d, schedpolicy=%d, schedparam=%d", name, rc_init, rc_setdetach, rc_setstack, rc_schedpolicy, rc_schedparam );
    }


    // Create the thread
    pthread_create( &m_nativeThreadHdl, &thread_attr, &entryPoint, this );
    pthread_attr_destroy( &thread_attr );
}

Thread::~Thread()
{
    // NOTE: In general it is not a good thing to "kill" threads - but to
    //       let the thread "run-to-completion", i.e. have the run() method
    //       of the associated IRunnable object complete.  If you do
    //       need to kill a thread - be dang sure that it is state such
    //       that it is ok to die - i.e. it has released all of its acquired
    //       resources: mutexes, semaphores, file handles, etc.
    if ( isActive() )
    {
        // Ask the runnable object nicely to stop
        m_runnable.pleaseStop();
        Kit::System::sleep( KIT_SYSTEM_THREAD_POSIX_DESTROY_WAIT_MS );  // Yield execution and allow time for the thread to actually exit.

        // Just to make sure: Brute the force the thread to end - IF it is still running
        // NOTE: This will NOT free any resources associated with the thread including the stack!
        if ( isActive() )
        {
            pthread_cancel( m_nativeThreadHdl );
        }
    }
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
    return m_name;
}


//////////////////////////////
void* Thread::entryPoint( void* data )
{
    // Convert data arg to a pointer to a Thread Object
    auto* myThreadPtr = static_cast<Thread*>( data );

    // Set the my TSD key for this thread to point to "my thread"
    pthread_setspecific( tsdKey_, myThreadPtr );

    // Set my "cancel" type so that thread can always be killed (i.e. the thread
    // is terminated immediately without waiting to reach a "cancellation point")
    int oldCancelType;
    pthread_setcanceltype( PTHREAD_CANCEL_ASYNCHRONOUS, &oldCancelType );

    // Go Execute the "IRunnable" object
    launchRunnable( *myThreadPtr );
    return 0;
}


}  // end namespace
}
}
//------------------------------------------------------------------------------
//////////////////////////////
Kit::System::Thread* Kit::System::Thread::create( IRunnable&  runnable,
                                                  const char* name,
                                                  int         priority,
                                                  int         stackSize,
                                                  void*       stackPtr,
                                                  bool        allowSimTicks ) noexcept
{
    return new Kit::System::Posix::Thread( runnable, name, priority, stackSize, SCHED_OTHER, allowSimTicks );
}


void Kit::System::Thread::destroy( Thread& threadToDestroy, uint32_t delayTimeMsToWaitIfActive ) noexcept
{
    // Wait for the thread to stop (if it is still running)
    if ( delayTimeMsToWaitIfActive > 0 && threadToDestroy.isActive() )
    {
        threadToDestroy.m_runnable.pleaseStop();
        threadToDestroy.timedWait( delayTimeMsToWaitIfActive );
    }

    delete &threadToDestroy;
}

//////////////////////////////
Kit::System::Thread* Kit::System::Thread::tryGetCurrent() noexcept
{
    // Trap potential error
    if ( !keyCreated_ )
    {
        Kit::System::FatalError::logRaw( Shutdown::eOSAL, "Thread::tryGetCurrent().  Have not yet created 'Tls Index'." );
    }

    auto* ptr = static_cast<Kit::System::Posix::Thread*>( pthread_getspecific( tsdKey_ ) );
    return isActiveThread( ptr ) ? ptr : nullptr;
}


void Kit::System::Thread::wait() noexcept
{
    ( static_cast<Kit::System::Posix::Thread*>( &getCurrent() ) )->m_syncSema.wait();
}

bool Kit::System::Thread::tryWait() noexcept
{
    return ( static_cast<Kit::System::Posix::Thread*>( &getCurrent() ) )->m_syncSema.tryWait();
}

bool Kit::System::Thread::timedWait( uint32_t msecs ) noexcept
{
    return ( static_cast<Kit::System::Posix::Thread*>( &getCurrent() ) )->m_syncSema.timedWait( msecs );
}
