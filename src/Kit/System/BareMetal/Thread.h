#ifndef KIT_SYSTEM_BAREMETAL_THREAD_H_
#define KIT_SYSTEM_BAREMETAL_THREAD_H_
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
#include "Kit/System/IRunnable.h"
#include "Kit/System/Semaphore.h"


///
namespace Kit {
namespace System {
namespace BareMetal {

/** This concrete class implements a Thread object on a bare-metal platform, i.e
    NO operating system, i.e. a 'single thread' with a super or event loop
 */
class Thread : public Kit::System::Thread
{
private:
    /// The application can not directly create thread objects.
    Thread() = delete;

public:
    /// Destructor
    ~Thread() noexcept;

public:
    /// See Kit::System::Thread
    const char* getName() const noexcept override;

public:
    /// See Kit::System::ISignable
    int signal() noexcept override;

    /// See Kit::System::ISignable
    int su_signal() noexcept override;


protected:
    /** Converts the application's native thread to a Kit Thread. The Application
        must use the Kit::System::Thread::create() to create its one-and-only-one
        thread.
     */
    Thread( Kit::System::IRunnable& applicationRunnable, const char* name ) noexcept;

public:
    /** This method has COMPONENT SCOPE, i.e the Application should NEVER call
        this method directly

        Entry point for the one and only one thread
     */
    static void entryPoint_() noexcept;


protected:
    /// Synchronization semaphore
    Kit::System::Semaphore m_syncSema;

    /// Thread name
    const char* m_name;

public:
    /// Housekeeping
    friend class Kit::System::Thread;
};


}  // end namespaces
}
}
#endif  // end header latch
