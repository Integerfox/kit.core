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
#include "Kit/System/api.h"
#include "Kit/System/FatalError.h"
// #include "Kit/System/SimTick.h"
#include "Kit/System/PrivateStartup.h"
#include <process.h>

// Initialize static class variables
DWORD dwTlsIndex_ = 0xFFFFFFFF;
bool  keyCreated_ = false;


////////////////////////////////////
namespace {  // anonymous namespace

/// This class is used to turn the entry/native/main thread into a Kit::System::Thread (i.e. add the thread semaphore)
class RegisterInitHandler_ : public Kit::System::StartupHook,
                             public Kit::System::Runnable
{
protected:
    // Empty run function
    // Note: Leave my 'running state' set to false -->this is so I don't
    // terminate the native thread prematurely when/if the Thread instance
    // is deleted.  In theory this can't happen since the Thread and Runnable
    // instance pointers for the native thread are never exposed to the
    // application and/or explicitly deleted.
    void entry() noexcept override {}

public:
    ///
    RegisterInitHandler_()
        : StartupHook( SYSTEM ) {}


protected:
    ///
    void notify( InitLevel_e initLevel ) noexcept override
    {
        // Create a thread object for the native thread
        m_parentThreadPtr_ = new Kit::System::Win32::Thread( *this );
    }
};
};  // end namespace

///
static RegisterInitHandler_ autoRegisterSystemInitHook_;

//------------------------------------------------------------------------------
namespace Kit {
namespace System {
namespace Win32 {


////////////////////////////////////
Thread::Thread( Kit::System::Runnable& dummyRunnable )
    : Kit::System::Thread( dummyRunnable )
    , m_name( "Win32Main" )
    , m_priority( GetThreadPriority( GetCurrentThread() ) )
{
    // Create TSD key that holds a pointer to "me" (only do this once)
    // NOTE: By definition/design this constructor will be CALLED before
    //       the 'normal' thread constructor and will ONLY CALLED ONCE.
    dwTlsIndex_ = TlsAlloc();
    if ( dwTlsIndex_ == TLS_OUT_OF_INDEXES )
    {
        Kit::System::FatalError::logRaw( Shutdown::eOSAL, "Thread().  TlsAlloc() failed when creating index to store the current Thread Pointer instance." );
    }

    // Set my Tls index/key 'created' status to good
    keyCreated_ = true;

    // Store a reference to the Kit thread object for the native thread
    TlsSetValue( dwTlsIndex_, this );

    // Add the native thread to the list of active threads
    addThreadToActiveList( *this );

    // Mark the NATIVE/Main thread as 'real time' thread for the SimTick engine
    // TODO: SIMTICK
    // CPL_SYSTEM_SIM_TICK_THREAD_INIT_( false );

    // Get a 'Real Handle' to this thread
    DuplicateHandle( GetCurrentProcess(), GetCurrentThread(), GetCurrentProcess(), &m_nativeThreadHdl, 0, FALSE, DUPLICATE_SAME_ACCESS );
}


Thread::Thread( Kit::System::Runnable& runnable,
                const char*            name,
                int                    priority,
                unsigned               stackSize,
                bool                   allowSimTicks )
    : Kit::System::Thread( runnable )
    , m_name( name )
    , m_priority( priority )
    , m_allowSimTicks( allowSimTicks )
{
    // Create a thread using _beginthreadex for HANDLE compatibility
    // Per the MSVC Documentation is okay to cast the returned uintptr_t to a HANDLE
    unsigned threadID;
    m_nativeThreadHdl = reinterpret_cast<HANDLE>( _beginthreadex( nullptr,                                      // default security attributes
                                                                  stackSize,                                    // r stack sizee
                                                                  (unsigned( __stdcall* )( void* ))entryPoint,  // thread function
                                                                  this,                                         // argument to thread function
                                                                  0,                                            // creation flags
                                                                  &threadID                                     // thread identifier
                                                                  ) );
}

Thread::~Thread()
{
    // NOTE: In general it is not a good thing to "kill" threads - but to
    //       let the thread "run-to-completion", i.e. have the run() method
    //       of the associated Runnable object complete.  If you do
    //       need to kill a thread - be dang sure that it is state such
    //       that it is ok to die - i.e. it has released all of its acquired
    //       resources: mutexes, semaphores, file handles, etc.
    if ( isActive() )
    {
        // Ask the runnable object nicely to stop
        m_runnable.pleaseStop();
        Kit::System::sleep( KIT_SYSTEM_THREAD_WIN32_DESTROY_WAIT_MS );  // Yield execution and allow time for the thread to actually exit.

        // Just to make sure: Brute the force the thread to end - IF it is still running
        // NOTE: This will NOT free any resources associated with the thread including the stack!
        if ( isActive() )
        {
            TerminateThread( m_nativeThreadHdl, 0 );
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
// DWORD WINAPI Thread::entryPoint(void* data)
void __cdecl Thread::entryPoint( void* data )
{
    // Initialize the TLS Value for this thread.
    TlsSetValue( dwTlsIndex_, data );

    // Go Execute the "Runnable" object
    Thread* myThreadPtr = (Thread*)data;
    SetThreadPriority( GetCurrentThread(), myThreadPtr->m_priority );

    launchRunnable( *myThreadPtr );
}

}  // end namespace
}
}
//------------------------------------------------------------------------------
//////////////////////////////

Kit::System::Thread* Kit::System::Thread::tryGetCurrent() noexcept
{
    // Trap potential error
    if ( !keyCreated_ )
    {
        Kit::System::FatalError::logRaw( Shutdown::eOSAL, "Thread::tryGetCurrent().  Have not yet created 'Tls Index'." );
    }

    auto* ptr = static_cast<Kit::System::Win32::Thread*>( TlsGetValue( dwTlsIndex_ ) );
    return isActiveThread( ptr ) ? ptr : nullptr;
}

void Kit::System::Thread::wait() noexcept
{
    ( static_cast<Kit::System::Win32::Thread*>( &getCurrent() ) )->m_syncSema.wait();
}

bool Kit::System::Thread::tryWait() noexcept
{
    return ( static_cast<Kit::System::Win32::Thread*>( &getCurrent() ) )->m_syncSema.tryWait();
}

bool Kit::System::Thread::timedWait( unsigned long timeout ) noexcept
{
    return ( static_cast<Kit::System::Win32::Thread*>( &getCurrent() ) )->m_syncSema.timedWait( timeout );
}


//////////////////////////////
Kit::System::Thread* Kit::System::Thread::create( Runnable&   runnable,
                                                  const char* name,
                                                  int         priority,
                                                  int         stackSize,
                                                  void*       stackPtr,
                                                  bool        allowSimTicks ) noexcept
{
    return new Kit::System::Win32::Thread( runnable, name, priority, stackSize, allowSimTicks );
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
