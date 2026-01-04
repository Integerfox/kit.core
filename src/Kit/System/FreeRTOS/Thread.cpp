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
#include "Kit/System/FatalError.h"
#include "Kit/System/PrivateStartup.h"
#include "Kit/System/Shutdown.h"
#include <new>

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
    void entry() noexcept override {}

public:
    ///
    RegisterInitHandler_()
        : IStartupHook( IStartupHook::InitLevel::SYSTEM ) {}


protected:
    ///
    void notify( IStartupHook::InitLevel initLevel ) noexcept override
    {
        // Create a thread object for the native thread
        m_parentThreadPtr_ = new ( std::nothrow ) Kit::System::FreeRTOS::Thread( "main", *this );
    }
};

static RegisterInitHandler_ autoRegister_systemInit_hook_;

}  // end anonymous namespace
// #endif


//------------------------------------------------------------------------------
namespace Kit {
namespace System {
namespace FreeRTOS {


////////////////////////////////////
Thread::Thread( const char* threadName, Kit::System::IRunnable& dummyRunnable ) noexcept
    : Kit::System::Thread( dummyRunnable )
    , m_name( threadName )
{
    // Plant the address of the thread object into FreeRTOS's TCB
    m_nativeThreadHdl = xTaskGetCurrentTaskHandle();
    vTaskSetApplicationTaskTag( m_nativeThreadHdl, (TaskHookFunction_t)this );

    // Add the native thread to the list of active threads
    addThreadToActiveList( *this );
}


Thread::Thread( Kit::System::IRunnable& runnable,
                const char*             name,
                int                     priority,
                unsigned                stackSize ) noexcept
    : Kit::System::Thread( runnable )
    , m_name( name )
{
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
    xTaskCreate( &entryPoint, name, stackSize, this, priority, &m_nativeThreadHdl );
    if ( m_nativeThreadHdl == NULL )
    {
        Kit::System::FatalError::logf( Kit::System::Shutdown::eOSAL, "FreeRTOS::Thread::Thread().  Failed to create thread %s.", name );
    }
}

void Thread::entryPoint( void* data ) noexcept
{
    // Convert data arg to a pointer to a Thread Object
    auto* myThreadPtr = static_cast<Thread*>( data );

    // Plant the address the thread object into FreeRTOS's TCB
    vTaskSetApplicationTaskTag( myThreadPtr->m_nativeThreadHdl, (TaskHookFunction_t)myThreadPtr );

    // Go Execute the "IRunnable" object
    launchRunnable( *myThreadPtr );

    // Terminate myself
    vTaskDelete( NULL );

    // Per the FreeRTOS API -->this function can NEVER return
    for ( ;; )
        ;
}

Thread::~Thread() noexcept
{
    // NOTE: In general it is not a good thing to "kill" threads - but to
    //       let the thread "run-to-completion", i.e. have the entry() method
    //       of the associated IRunnable object complete.  If you do
    //       need to kill a thread - be dang sure that it is state such
    //       that it is ok to die - i.e. it has released all of its acquired
    //       resources: mutexes, semaphores, file handles, etc.
    if ( isActive() )
    {
        // Ask the runnable object nicely to stop
        m_runnable.pleaseStop();
        Kit::System::sleep( KIT_SYSTEM_THREAD_FREERTOS_DESTROY_WAIT_MS );  // Yield execution and allow time for the thread to actually exit.

        // Just to make sure: Brute the force the thread to end - IF it is still running
        // NOTE: This will NOT free any resources associated with the thread including the stack!
        if ( isActive() )
        {
            vTaskDelete( m_nativeThreadHdl );
        }
    }
}

/////////////////////////////
int Thread::signal() noexcept
{
    xTaskNotifyGive( m_nativeThreadHdl );
    return 0;  // Always return success
}

/** NOTE: This method returns the 'higherPriorityTaskWoken' instead of the
          defined 'return zero on success' semantics.  This is to overcome
          the oddities of FreeRTOS.
 */
int Thread::su_signal() noexcept
{
    BaseType_t higherPriorityTaskWoken = pdFALSE;
    vTaskNotifyGiveFromISR( m_nativeThreadHdl, &higherPriorityTaskWoken );
    return higherPriorityTaskWoken;
}

const char* Thread::getName() const noexcept
{
    return m_name;
}

}  // end namespace
}
}

//------------------------------------------------------------------------------
namespace Kit {
namespace System {


//////////////////////////////
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

bool Kit::System::Thread::timedWait( uint32_t msecs ) noexcept
{
    return ulTaskNotifyTake( pdFALSE, msecs * portTICK_PERIOD_MS ) > 0;
}


//////////////////////////////
Kit::System::Thread* Kit::System::Thread::create( IRunnable&  runnable,
                                                  const char* name,
                                                  int         priority,
                                                  int         stackSize,
                                                  void*       stackPtr,
                                                  bool        allowSimTicks ) noexcept
{
    return new ( std::nothrow ) Kit::System::FreeRTOS::Thread( runnable, name, priority, stackSize );
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


}  // end namespace
}
//------------------------------------------------------------------------------