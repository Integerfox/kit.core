#ifndef KIT_SYSTEM_RPPICO_THREAD_H_
#define KIT_SYSTEM_RPPICO_THREAD_H_
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
#include "Kit/System/Thread.h"
#include "Kit/System/Semaphore.h"
#include "Kit/Text/FString.h"



/// Size, in bytes (not including the null terminator), of a thread name
#ifndef OPTION_CPL_SYSTEM_RPPICOTHREAD_NAME_LEN        
#define OPTION_CPL_SYSTEM_RPPICOTHREAD_NAME_LEN        16
#endif


///
namespace Kit {
///
namespace System {
///
namespace RPPico {

/** This concrete class implements a Thread object that maps 1 to 1 with
    a processor core on the Raspberry PI RP2xxx micro-controller.

    At least one thread must be created and at most only two threads can be
    created. The first thread created executes on core0.  The second thread
    created executes on core1.

    The first thread MUST be created before enableScheduling() is called. The
    second thread that executes on core1 can be created or after enableScheduling()
    is called. The core1 thread can also be destroyed and recreated after
    enableScheduling() is called.

    The name of the threads are hard coded to 'CORE0' and 'CORE1'.

    The 'native' thread handle is the core number (i.e. 0 or 1).

    Threads can ONLY be created using the Kit::System::Thread::create() method.
    When created threads:
        - Only the 'runnable' and 'name' should be specified.
        - The stack sizes are defined by the build symbols: PICO_STACK_SIZE
          and PICO_CORE1_STACK_SIZE, e.g. -DPICO_STACK_SIZE=4096 -DPICO_CORE1_STACK_SIZE=2048
        - Thread priority has NO meaning since both threads/cores execute concurrently.
        - The thread(s) begin executing after enableScheduling() is called.

    NOTES: 
        1. The threads do NOT 'execute' (i.e. call their Runnable object's run() 
           method till enableScheduling() is called.  In addition the enableScheduling()
           method NEVER returns.
        2. The suspendScheduling() semantics suspends ALL processing on the 'other'
           core.  This includes interrupt processing on the other core.

  */
class Thread : public Kit::System::Thread
{
protected:
    /// Private Constructor. Application can only create thread using the Kit::System::Thread::create() method.
    Thread( IRunnable& runnable, unsigned coreId ) noexcept;

public:
    /// Destructor
    ~Thread();

public:
    /// See Kit::System::Thread
    const char* getName() const noexcept override;

    /// Override to use m_runnablePtr pointer when it's been set by create()
    Kit::System::IRunnable& getRunnable() const noexcept override;

    /// See Kit::System::Signable
    int signal( void ) noexcept;

    /// See Kit::System::Signable
    int su_signal( void ) noexcept;



public:
    /** COMPONENT SCOPED METHOD.  The application should NEVER call this
        this method.

        Helper method to internally allocate a thread. 
     */
    static void createThreadInstance( unsigned coreId, Kit::System::IRunnable& runnable ) noexcept;


protected:
    /// The thread synchronized message semaphore.
    Kit::System::Semaphore  m_syncSema;

    /// Pointer to the actual runnable object (overrides base class reference when set)
    Kit::System::IRunnable*  m_runnablePtr;

    /// internal handle
    unsigned                m_coreId;

public:
    /// Housekeeping
    friend class Kit::System::Thread;
};


};      // end namespaces
};
};
#endif  // end header latch
