/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "StdioThread.h"
#include "Kit/System/Thread.h"
#include <atomic>

/// Anonymous namespace
namespace {

class ShellRunnable : public Kit::System::IRunnable
{
public:
    ///
    Kit::TShell::IProcessor& m_shell;
    ///
    Kit::Io::IInput& m_infd;
    ///
    Kit::Io::IOutput& m_outfd;
    ///
    std::atomic<bool> m_startEnabled;
    ///
    std::atomic<bool> m_endThread;
    ///
    std::atomic<bool> m_shellRunning;

public:
    ShellRunnable( Kit::TShell::IProcessor& shell,
                   Kit::Io::IInput&         infd,
                   Kit::Io::IOutput&        outfd,
                   bool                     enabledOnLaunch )
        : m_shell( shell )
        , m_infd( infd )
        , m_outfd( outfd )
        , m_startEnabled( enabledOnLaunch )
        , m_endThread( false )
        , m_shellRunning( false )
    {
    }

public:
    ///
    void pleaseStopTShell( bool stopThread = false ) noexcept
    {
        m_endThread = stopThread;
        m_shell.requestStop();
        if ( stopThread )
        {
            // Signal the Runnable instance in case it self-terminated and is waiting for a signal to restart the shell
            auto* t = getThread();
            if ( t )
            {
                t->signal();
            }
        }
    }

    ///
    bool isTShellRunning() noexcept
    {
        return m_shellRunning;
    }

    ///
    bool restartTShell()
    {
        auto* t = getThread();
        if ( t )
        {
            t->signal();
            return true;
        }
        return false;
    }

    ///
    void entry() noexcept override
    {
        for ( ;; )
        {
            // Support starting in the disabled/wait state
            if ( m_startEnabled )
            {
                m_shellRunning = true;
                m_shell.start( m_infd, m_outfd );
                m_shellRunning = false;
            }
            m_startEnabled = true;
            if ( !m_endThread )
            {
                Kit::System::Thread::wait();
            }
            else
            {
                break;  // Exit the loop and self-terminate the thread
            }
        }
    }

    ///
    void pleaseStop() noexcept override
    {
        pleaseStopTShell( true );
        IRunnable::pleaseStop();
    }
};

}  // end anonymous namespace

//------------------------------------------------------------------------------
namespace Kit {
namespace TShell {

StdioThread::~StdioThread()
{
    if ( !m_staticInstance )
    {
        if ( m_threadPtr )
        {
            Kit::System::Thread::destroy( *m_threadPtr );
        }
        delete m_runnablePtr;
    }
}

bool StdioThread::launchTShell( Kit::Io::IInput& infd, Kit::Io::IOutput& outfd, bool enabledOnLaunch ) noexcept
{
    // Already launched?
    if ( m_threadPtr )
    {
        return false;
    }

    m_runnablePtr = new ( std::nothrow ) ShellRunnable( m_shell, infd, outfd, enabledOnLaunch );
    if ( m_runnablePtr != nullptr )
    {
        m_threadPtr = Kit::System::Thread::create( *m_runnablePtr, m_name, m_priority );
        if ( m_threadPtr == nullptr )
        {
            delete m_runnablePtr;
            m_runnablePtr = nullptr;
            return false;
        }
    }

    return m_runnablePtr != nullptr;
}

bool StdioThread::restartTShell() noexcept
{
    // Must have been successfully launched first
    if ( m_threadPtr == nullptr )
    {
        return false;
    }

    // Signal the TShell thread to restart the shell
    auto* myShellPtr = static_cast<ShellRunnable*>( m_runnablePtr );
    return myShellPtr->restartTShell();
}

void StdioThread::pleaseStopTShell( bool stopThread ) noexcept
{
    if ( m_threadPtr && m_runnablePtr )
    {
        ShellRunnable* myShellPtr = static_cast<ShellRunnable*>( m_runnablePtr );
        myShellPtr->pleaseStopTShell( stopThread );
    }
}

bool StdioThread::isTShellRunning() noexcept
{
    if ( m_threadPtr && m_runnablePtr )
    {
        ShellRunnable* myShellPtr = static_cast<ShellRunnable*>( m_runnablePtr );
        return myShellPtr->isTShellRunning();
    }
    return false;
}


}  // end namespace
}
//------------------------------------------------------------------------------