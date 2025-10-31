#ifndef KIT_SYSTEM_FREERTOS_THREAD_H_
#define KIT_SYSTEM_FREERTOS_THREAD_H_
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
#include "Kit/Text/FString.h"
#include "task.h"


/** Provides the default stack size (since FreeRTOS does not provide one)
 */
#ifndef OPTION_KIT_SYSTEM_FREERTOS_DEFAULT_STACK_SIZE
#define OPTION_KIT_SYSTEM_FREERTOS_DEFAULT_STACK_SIZE ( 1024 * 3 )
#endif

/** The time (in milliseconds) to wait when the Thread instance is being destroyed
    and the associated IRunnable object is still running - BEFORE forcibly terminating
    the thread.
 */
#ifndef KIT_SYSTEM_THREAD_FREERTOS_DESTROY_WAIT_MS
#define KIT_SYSTEM_THREAD_FREERTOS_DESTROY_WAIT_MS 100
#endif


///
namespace Kit {
namespace System {
namespace FreeRTOS {

/** This concrete class implements a Thread object using FreeRTOS threads
 */
class Thread : public Kit::System::Thread
{
protected:
    /// ASCII name of the task
    Kit::Text::FString<configMAX_TASK_NAME_LEN> m_name;

public:
    /** Constructor.
            o Does NOT support the application supplying the stack
              memory. Stack memory is allocated from the HEAP

            o If zero is passed as the stack size, then the default stack size
              is set based on the OPTION_KIT_SYSTEM_FREERTOS_DEFAULT_STACK_SIZE
              parameter.
     */
    Thread( IRunnable&  runnable,
            const char* name,
            int         priority  = KIT_SYSTEM_THREAD_PRIORITY_NORMAL,
            unsigned    stackSize = 0 ) noexcept;

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
    /// Entry point for all newly created threads
    static void entryPoint( void* data ) noexcept;


public:
    /** COMPONENT Scoped constructor to convert the native FreeRTOS thread to a
        Kit Thread. THIS CONSTRUCTOR SHOULD NEVER BE USED BY THE APPLICATION!
     */
    Thread( const char* threadName, Kit::System::IRunnable& dummyRunnable ) noexcept;

    /** This is helper method to 'convert' the first/main FreeRTOS thread
        to a CPL thread.  The method can be called many times - but it
        does the 'conversation' once.  The motivation for this method was
        working with the Arduino platform/framework where it creates
        the first/main FreeRTOS thread.
     */
    static void makeNativeMainThreadAKitThread() noexcept;

public:
    /// Housekeeping
    friend class Kit::System::Thread;
};

/** This is a helper class that can be used to make the current thread
    a CPL thread.  This class should only be used when the 'application'
    contains active threads there were not created through the CPL
    libraries APIs.  For example: On the Arduino Feather52 platform,
    the Arduino framework creates the 'main' thread.

    ** ONLY USE THIS CLASS IF YOU KNOW WHAT YOU ARE DOING **
 */
class MakeCurrentThreadAKitThread : public Kit::System::IRunnable
{
protected:
    // Empty run function
    void entry() noexcept override {}

public:
    /// Converts the native thread to a Kit thread
    MakeCurrentThreadAKitThread( const char* threadName = "main" ) noexcept
    {
        // Create a thread object for the native thread
        m_parentThreadPtr_ = new Kit::System::FreeRTOS::Thread( threadName, *this );
    }
};


}  // end namespaces
}
}
#endif  // end header latch
