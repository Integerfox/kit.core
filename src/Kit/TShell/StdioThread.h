#ifndef KIT_TSHELL_STDIO_THREAD_H_
#define KIT_TSHELL_STDIO_THREAD_H_
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
#include "Kit/TShell/IProcessor.h"
#include "Kit/Io/IInput.h"
#include "Kit/Io/IOutput.h"
#include "Kit/System/Thread.h"


///
namespace Kit {
///
namespace TShell {

/** This concrete class provides the 'threading wrapper' for running a
    TShell Command Processor.  This requires that the Input/Output streams
    be provided when the TShell is launched.  The TShell runs in a dedicated
    thread with blocking waiting semantics with respect to its input.

    The TShell can be started, stopped, and then restarted multiple times.

    NOTE: This class dynamically allocates memory and dynamically creates a
          Thread.  However, the memory and thread are allocated/created ONCE, 
          i.e. stopping and restarting the TShell does NOT free/re-allocate the
          dynamic memory or destroy/re-create the thread.
*/
class StdioThread
{
public:
    /** Constructor.  The 'thisIsAStaticInstance' argument is to inform the
        instance being create that it is being created statically (i.e. before
        main() is entered) - which is the intended typically behavior.  This
        knowledge is used to inhibit delete/destroy-thread actions in the
        instance's destructor.  This is necessary because there is no
        guaranteed order to when static destructor fire and as such the state
        of the KIT Libraries static resources (e.g. mutexes) are unknown
        which your application will crash/behavior poorly on exit.
     */
    StdioThread( IProcessor& shell,
                 const char* threadName            = "TShell",
                 int         threadPriority        = KIT_SYSTEM_THREAD_PRIORITY_NORMAL + KIT_SYSTEM_THREAD_PRIORITY_LOWER,
                 bool        thisIsAStaticInstance = true ) noexcept
        : m_shell( shell )
        , m_threadPtr( nullptr )
        , m_name( threadName )
        , m_runnablePtr( nullptr )
        , m_priority( threadPriority )
        , m_staticInstance( thisIsAStaticInstance )
    {
    }

    /// Destructor
    ~StdioThread();

public:
    /** This method starts the Processor.  It is thread safe in that it
        runs in the context of the calling thread - and spawns a new
        thread for Shell/Command Processor to executing in.

        When the 'enabledOnLaunch' parameter is set to true, the tshell will
        immediately start processing input.  When 'enabledOnLaunch' is set to
        false, the thread will be created, but the tshell will not be started
        until restartTShell() is called.

        The stream parameters (infd, outfd) are required to be valid for the
        lifetime of the StdioThread instance.
        
        The method returns true if successful, else false is returned.
        NOTE: If launchTShell is called more than once it will always return
              false.
     */
    bool launchTShell( Kit::Io::IInput&  infd,
                       Kit::Io::IOutput& outfd,
                       bool              enabledOnLaunch = true ) noexcept;

    /** This method is used to restart the tshell. This method can ONLY be
        called AFTER launch has been called.

        The method returns true if the tshell was successfully restarted, 
        else false is returned (e.g. launchTShell() was never called)
     */
    bool restartTShell() noexcept;

    /** This method is used to request that the tshell be stopped AND optionally
        that its thread be terminated.

        Note: The tshell self terminates when a 'bye tshell' command was issued
              by the user or a IO error on the tshell's IO Streams are encounter.
              When the tshell self terminates - the thread is placed into a
              blocking state, i.e. waiting for a call to restartTShell().
     */
    void pleaseStopTShell( bool stopThread = false ) noexcept;

    /// This method returns true when the tshell is running
    bool isTShellRunning() noexcept;

protected:
    /// Command Processor to run
    IProcessor& m_shell;

    /// Thread that the shell runs in
    Kit::System::Thread* m_threadPtr;

    /// Thread name for the shell
    const char* m_name;

    /// Runnable instance
    Kit::System::IRunnable* m_runnablePtr;

    /// Thread priority to run the shell
    int m_priority;

    /// How I was created
    bool m_staticInstance;
};


}  // end namespaces
}
#endif  // end header latch
