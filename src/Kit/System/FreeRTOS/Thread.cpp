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
#include "Kit/System/Trace.h"
#include "Kit/System/FatalError.h"
#include "Kit/System/PrivateStartup.h"
#include "Kit/Container/SList.h"

#define SECT_ "Kit::System::FreeRTOS::Thread"


////////////////////////////////////
// #ifdef USE_KIT_SYSTEM_FREERTOS_NATIVE_THREAD
namespace {

/// This class is used to turn the entry/native/main thread into a Kit::System::Thread (i.e. add the thread semaphore)
class RegisterInitHandler_ : public Kit::System::IRunnable,
                             public Kit::System::IStartupHook

{
protected:
    // Empty 'run' function -- it is never called!
    void entry( void ) noexcept override {}

public:
    ///
    RegisterInitHandler_()
        : IStartupHook( IStartupHook::InitLevel::SYSTEM ) {}


protected:
    ///
    void notify( IStartupHook::InitLevel initLevel ) noexcept override
    {
        // Create a thread object for the native thread
        m_parentThreadPtr_ = new Kit::System::FreeRTOS::Thread( "main", *this );
    }
};

static RegisterInitHandler_ autoRegister_systemInit_hook_;

} // end anonymous namespace
//#endif

//------------------------------------------------------------------------------
namespace Kit {
namespace System {
namespace FreeRTOS {

// Initialize static class variables
static Kit::Container::SList<Thread> threadList_( "StaticConstructor" );
static const char*                   emptyString_ = "";

// THIS IS BLATANT HACK to work around that xTaskGetSchedulerState() does not work as expected.  This variable is defined in Api.cpp
extern bool g_Kit_System_thread_freertos_schedulerStarted;

static void addThreadToActiveList_( Thread& thread );
static void removeThreadFromActiveList_( Thread& thread );


//------------------------------------------------------------------------------
namespace Kit {
namespace System {
namespace FreeRTOS {


////////////////////////////////////
Thread::Thread( const char* threadName, Kit::System::IRunnable& dummyRunnable )
    : m_runnable( dummyRunnable ), m_name( threadName ), m_threadHandle( xTaskGetCurrentTaskHandle() )
{
    // Initialize by TLS storage for this thread
    for ( unsigned i = 0; i < OPTION_KIT_SYSTEM_TLS_DESIRED_MIN_INDEXES; i++ )
    {
        m_tlsArray[i] = 0;
    }

    // Plant the address of my TLS array into FreeRTOS's TCB
    vTaskSetApplicationTaskTag( m_threadHandle, (TaskHookFunction_t)this );

    // Add the native thread to the list of active threads
    addThreadToActiveList_( *this );
}


Thread::Thread( Kit::System::Runnable& runnable,
                const char*            name,
                int                    priority,
                unsigned               stackSize )
    : m_runnable( runnable ), m_name( name ), m_threadHandle( NULL )
{
    // Initialize by TLS storage for this thread
    for ( unsigned i = 0; i < OPTION_KIT_SYSTEM_TLS_DESIRED_MIN_INDEXES; i++ )
    {
        m_tlsArray[i] = 0;
    }

    // Calculate stack size in terms of 'depth' (not bytes)
    if ( stackSize == 0 )
    {
        stackSize = OPTION_KIT_SYSTEM_FREERTOS_DEFAULT_STACK_SIZE >> ( sizeof( StackType_t ) / 2 );
    }
    else
    {
        stackSize = stackSize >> ( sizeof( StackType_t ) / 2 );
    }


    // Create the thread
    xTaskCreate( &entryPoint, name, stackSize, this, priority, &m_threadHandle );
    if ( m_threadHandle == NULL )
    {
        Kit::System::FatalError::logf( "FreeRTOS::Thread::Thread().  Failed to create thread %s.", name );
    }
}

Thread::~Thread()
{
    // NOTE: In general it is not a good thing to "kill" threads - but to
    //       let the thread "run-to-completion", i.e. have the run() method
    //       of the associated Runnable object complete.  If you do
    //       need to kill a thread - be dang sure that it is state such
    //       that it is ok to die - i.e. it has released all of its acquired
    //       resources: mutexes, semaphores, file handles, etc.
    if ( m_runnable.isRunning() )
    {
        // Ask the runnable object nicely to stop
        m_runnable.pleaseStop();
        Kit::System::Api::sleep( 100 );  // Yield execution and allow time for the thread to actually exit.

        // Just to make sure: Brute the force the thread to end - IF it is still running
        // NOTE: This will NOT free any resources associated with the thread including the stack!
        if ( m_runnable.isRunning() )
        {
            vTaskDelete( m_threadHandle );
        }
    }
}

} // end namespace
}
}
//------------------------------------------------------------------------------

//////////////////////////////
int Thread::signal() noexcept
{
    xTaskNotifyGive( m_threadHandle );
    return 0;  // Always return success
}


/** NOTE: This method returns the 'higherPriorityTaskWoken' instead of the
          defined 'return zero on success' semantics.  This is to overcome
          the oddities of FreeRTOS.
 */
int Thread::su_signal() noexcept
{
    BaseType_t higherPriorityTaskWoken = pdFALSE;
    vTaskNotifyGiveFromISR( m_threadHandle, &higherPriorityTaskWoken );
    return higherPriorityTaskWoken;
}


const char* Thread::getName() noexcept
{
    return m_name;
}

size_t Thread::getId() noexcept
{
    return (size_t)m_threadHandle;
}

bool Thread::isRunning() noexcept
{
    return m_runnable.isRunning();
}

Kit_System_Thread_NativeHdl_T Thread::getNativeHandle( void ) noexcept
{
    return m_threadHandle;
}

Kit::System::Runnable& Thread::getRunnable( void ) noexcept
{
    return m_runnable;
}

}  // end namespace
}
}
//------------------------------------------------------------------------------

//////////////////////////////
void Thread::entryPoint( void* data )
{
    // Convert data arg to a pointer to a Thread Object
    Thread* myThreadPtr = (Thread*)data;

    // Plant the address of my TLS array into FreeRTOS's TCB
    vTaskSetApplicationTaskTag( myThreadPtr->m_threadHandle, (TaskHookFunction_t)myThreadPtr );


    // Go Execute the "Runnable" object
    addThreadToActiveList_( *myThreadPtr );
    myThreadPtr->m_runnable.setThreadOfExecution_( myThreadPtr );
    myThreadPtr->m_runnable.run();

    // Remove the thread from the list of active threads
    removeThreadFromActiveList_( *myThreadPtr );

    // Terminate myself
    vTaskDelete( NULL );

    // Per the FreeRTOS API -->this function can NEVER return
    for ( ;; )
        ;
}


//////////////////////////////
Kit::System::Thread& Kit::System::Thread::getCurrent() noexcept
{
    Thread* curThread = tryGetCurrent();

    // Trap potential error
    if ( !curThread )
    {
        Kit::System::FatalError::logRaw( "FreeRTOS::Thread::getCurrent().  Current thread is NOT a 'Kit::System::Thread'." );
    }

    return *curThread;
}

Kit::System::Thread* Kit::System::Thread::tryGetCurrent() noexcept
{
    TaskHandle_t taskHdl = xTaskGetCurrentTaskHandle();
    if ( taskHdl == 0 )
    {
        return nullptr;
    }
    return (Thread*)xTaskGetApplicationTaskTag( taskHdl );
}

void Kit::System::Thread::wait() noexcept
{
    ulTaskNotifyTake( pdFALSE, portMAX_DELAY );
}

bool Kit::System::Thread::tryWait() noexcept
{
    return ulTaskNotifyTake( pdFALSE, 0 ) > 0;
}

bool Kit::System::Thread::timedWait( unsigned long msecs ) noexcept
{
    return ulTaskNotifyTake( pdFALSE, msecs * portTICK_PERIOD_MS ) > 0;
}

const char* Kit::System::Thread::myName() noexcept
{
    // Provide some protection in case this method is called before the scheduler is running (e.g. called by the trace engine)
    if ( !Kit::System::Api::isSchedulingEnabled() )
    {
        return emptyString_;
    }

    return ( (Kit::System::FreeRTOS::Thread*)xTaskGetApplicationTaskTag( xTaskGetCurrentTaskHandle() ) )->m_name;
}


size_t Kit::System::Thread::myId() noexcept
{
    // Provide some protection in case this method is called before the scheduler is running (e.g. called by the trace engine)
    if ( !Kit::System::Api::isSchedulingEnabled() )
    {
        return 0;
    }

    return (size_t)( ( (Kit::System::FreeRTOS::Thread*)( &getCurrent() ) )->m_threadHandle );
}


void** Thread::getTlsArray() noexcept
{
    return ( (Kit::System::FreeRTOS::Thread*)xTaskGetApplicationTaskTag( xTaskGetCurrentTaskHandle() ) )->m_tlsArray;
}


//////////////////////////////
void Kit::System::Thread::traverse( Kit::System::Thread::Traverser& client ) noexcept
{
    Kit::System::Mutex::ScopeBlock mylock( Kit::System::Locks_::sysLists() );
    Kit::System::FreeRTOS::Thread* t = threadList_.first();
    while ( t )
    {
        if ( client.item( *t ) == Kit::Type::Traverser::eABORT )
        {
            break;
        }
        t = threadList_.next( *t );
    }
}


void addThreadToActiveList_( Thread& thread )
{
    Kit::System::Mutex::ScopeBlock lock( Kit::System::Locks_::sysLists() );
    threadList_.put( thread );
}

void removeThreadFromActiveList_( Thread& thread )
{
    Kit::System::Mutex::ScopeBlock lock( Kit::System::Locks_::sysLists() );
    threadList_.remove( thread );
}


//////////////////////////////
Kit::System::Thread* Kit::System::Thread::create( Runnable&   runnable,
                                                  const char* name,
                                                  int         priority,
                                                  int         stackSize,
                                                  void*       stackPtr,
                                                  bool        allowSimTicks )
{
    return new ( std::nothrow ) Kit::System::FreeRTOS::Thread( runnable, name, priority, stackSize );
}


void Kit::System::Thread::destroy( Thread& threadToDestroy )
{
    delete &threadToDestroy;
}
