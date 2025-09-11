#ifndef Cpl_System_Win32_Thread_h_
#define Cpl_System_Win32_Thread_h_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */


#include "Kit/System/Thread.h"
#include "Kit/System/Semaphore.h"
#include "Kit/Text/FString.h"

/** The time (in milliseconds) to wait when the Thread instance is being destroyed
    and the associated IRunnable object is still running - BEFORE forcibly terminating
    the thread.
 */
#ifndef KIT_SYSTEM_THREAD_WIN32_DESTROY_WAIT_MS
#define KIT_SYSTEM_THREAD_WIN32_DESTROY_WAIT_MS 100
#endif


///
namespace Kit {
///
namespace System {
///
namespace Win32 {

/** This concrete class implements a Thread object using Win32 threads

    NOTE: The class/implementation uses _beginthread() instead of
          CreateThread() ->this is per Microsoft's documentation
          that says the _beginthread() properly initializes/cleans-up
          the C-Runtime library as where CreateThread() does NOT.
 */
class Thread : public Kit::System::Thread
{
public:
    /** Constructor.  Priority is 0=highest, 30=lowest, normal=15.
        NOTE: Does NOT support the application supplying the stack
              memory.
     */
    Thread( IRunnable&   runnable,
            const char* name,
            int         priority      = KIT_SYSTEM_THREAD_PRIORITY_NORMAL,
            unsigned    stackSize     = 0,
            bool        allowSimTicks = true
    );

    /// Destructor
    ~Thread();

public:
    /// See Kit::System::Thread
    const char* getName() const noexcept override;

public:
    /// See Kit::System::ISignable
    int signal( void ) noexcept override;

    /// See Kit::System::ISignable
    int su_signal( void ) noexcept override;



private:
    /// Entry point for all newly created threads
    static void __cdecl entryPoint( void* data );


public:
    /** COMPONENT Scoped constructor to convert the native Win32 thread to a 
        Kit Thread. THIS CONSTRUCTOR SHOULD NEVER BE USED BY THE APPLICATION!
     */
    Thread( Kit::System::IRunnable& dummyRunnable );


public:
    /// Housekeeping
    friend class Kit::System::Thread;

protected:
    /// The thread synchronized message semaphore.
    Kit::System::Semaphore  m_syncSema;

    /// ASCII name of the task
    Kit::Text::FString<64>  m_name;

    /// Priority
    int                     m_priority;
};


};      // end namespaces
};
};
#endif  // end header latch
