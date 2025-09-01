#ifndef KIT_SYSTEM_POSIX_THREAD_H_
#define KIT_SYSTEM_POSIX_THREAD_H_
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
#include <pthread.h>

///
namespace Kit {
///
namespace System {
///
namespace Posix {

/** This concrete class implements a Thread object using Posix threads
 */
class Thread : public Kit::System::Thread
{
public:
    /** Constructor.

        o POSIX does not define/require specific Priority values, however
          it does define that the priority range must be at 32 and the
          a higher numerical value has higher priority... So the priority
          values support ONLY a range of 32, with N+1 having a higher
          priority than N.  At run time, the HIGHEST/LOWEST bounds will be
          mapped the actual range defined by the functions:
              sched_get_priority_max(), sched_get_priority_min()

        o The preferred schedType is SCHED_RR or SCHED_FIFO - BUT these
          type require superuser privileges to work.  All of us mere
          mortals must use SCHED_OTHER.  SCHED_OTHER is defined as
          "normal scheduling" - what ever that means.  The biggest problem
          with SCHED_OTHER is that only ONE priority is supported -->BUMMER!
          You can still pass different priority values with SCHED_OTHER -
          but they will have no effect. The priority values WILL work if
          SCHED_RR or SCHED_FIFO is specified.

        o Does NOT support the application supplying the stack memory.
     */
    Thread( Runnable&      runnable,
            const char*    name,
            int            priority      = KIT_SYSTEM_THREAD_PRIORITY_NORMAL,
            unsigned       stackSize     = 0,
            int            schedType     = SCHED_OTHER,
            bool           allowSimTicks = true
    );

    /// Destructor
    ~Thread();

public:
    /// See Kit::System::Thread
    const char* getName() const noexcept override;

    /// See Kit::System::Thread
    KitSystemThreadID_T getId() const noexcept override;

    /// See Kit::System::Thread
    bool isRunning( void ) const noexcept override;

    /// See Kit::System::Thread
    Runnable& getRunnable( void ) const noexcept override;

public:
    /// See Kit::System::Signable
    int signal( void ) noexcept override;

    /// See Kit::System::Signable
    int su_signal( void ) noexcept override;


protected:
    /// Entry point for all newly created threads
    static void* entryPoint( void* data );


public:
    /** Private constructor to convert the native Posix thread to a Kit Thread.
        THIS CONSTRUCTOR SHOULD NEVER BE USED BY THE APPLICATION!
     */
    Thread( Kit::System::Runnable& dummyRunnable );

protected:
    /// Reference to the runnable object for the thread
    Kit::System::Runnable&  m_runnable;

    /// ASCII name of the task
    Kit::Text::FString<64>  m_name;

    /// internal handle
    pthread_t               m_threadHandle;

    /// The thread synchronized message semaphore.
    Kit::System::Semaphore  m_syncSema;

    /// Option to allow simulated ticks
    bool                    m_allowSimTicks;
};


}       // end namespaces
}
}
#endif  // end header latch
