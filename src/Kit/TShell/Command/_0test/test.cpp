/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "test.h"
#include "Kit/Bsp/Api.h"
#include "Kit/Container/OrderedList.h"
#include "Kit/TShell/ISecurity.h"
#include "Kit/TShell/Processor.h"
#include "Kit/TShell/NoSecurity.h"
#include "Kit/System/Trace.h"
#include "Kit/System/Thread.h"
#include "Kit/System/Mutex.h"
#include "Kit/System/IRunnable.h"
#include "Kit/System/Api.h"
#include "Kit/System/Private.h"
#include "Kit/System/ElapsedTime.h"
#include "Kit/TShell/Command/Base.h"
#include "Kit/TShell/Command/Bye.h"
#include "Kit/TShell/Command/Help.h"
#include "Kit/TShell/Command/Echo.h"
#include "Kit/TShell/Command/Trace.h"
#include "Kit/TShell/Command/Wait.h"
#include "Kit/TShell/StdioThread.h"
#include "Kit/Text/Tokenizer/TextBlock.h"
#include "Kit/Text/StringTo.h"

#define SECT_ "_0test"

///
using namespace Kit::System;
using namespace Kit::TShell;
using namespace Kit::Type;


////////////////////////////////////////////////////////////////////////////////
namespace {


// Not exactly thread-safe - but close enough for this test
class Apple : public Kit::System::IRunnable
{
public:
    ///
    Kit::System::Mutex m_lock;
    ///
    uint32_t m_delay;
    ///
    uint32_t m_restartDelay;
    ///
    uint32_t m_timeMarker;
    ///
    bool m_outputTrace;
    ///
    Kit::TShell::StdioThread& m_stdioThread;

public:
    ///
    Apple( Kit::TShell::StdioThread& stdioThread )
        : m_delay( 250 )
        , m_restartDelay( 0 )
        , m_timeMarker( 0 )
        , m_outputTrace( false )
        , m_stdioThread( stdioThread )
    {
    }

public:
    ///
    void setOutputState( bool newstate )
    {
        Kit::System::Mutex::ScopeLock criticalSection( m_lock );
        m_outputTrace = newstate;
    }

    ///
    void setDelay( uint32_t newdelay )
    {
        Kit::System::Mutex::ScopeLock criticalSection( m_lock );
        m_delay = newdelay;
    }

    /// Restarts the TShell after the specified delay (in milliseconds)
    void restartTShell( uint32_t delay )
    {
        m_restartDelay = delay;
        m_timeMarker   = Kit::System::ElapsedTime::milliseconds();
    }

public:
    ///
    void entry() noexcept override
    {
        unsigned counter = 0;
        for ( ;; )
        {
            Bsp_toggle_debug1();
            counter++;

            m_lock.lock();
            bool     output = m_outputTrace;
            uint32_t delay  = m_delay;
            m_lock.unlock();

            Kit::System::sleep( delay );
            if ( output )
            {
                KIT_SYSTEM_TRACE_MSG( SECT_, "Trace (_0test): loop counter=%u", counter );
                KIT_SYSTEM_TRACE_MSG( "bob", "Trace (bob): loop counter=%u", counter );
            }

            if ( m_restartDelay > 0 )
            {
                if ( Kit::System::ElapsedTime::expiredMilliseconds( m_timeMarker, m_restartDelay ) )
                {
                    bool shellRunning = m_stdioThread.isTShellRunning();
                    KIT_SYSTEM_TRACE_MSG( "bob", "TShell is running=%s", shellRunning ? "true" : "false" );
                    KIT_SYSTEM_TRACE_MSG( "bob", "Restarting TShell after %u milliseconds", m_restartDelay );
                    m_stdioThread.restartTShell();
                    m_restartDelay = 0;
                    Kit::System::sleep( 200 );  // Allow time for the TShell to restart before continuing
                    shellRunning = m_stdioThread.isTShellRunning();
                    KIT_SYSTEM_TRACE_MSG( "bob", "AFTER RESTART: TShell is running=%s", shellRunning ? "true" : "false" );
                }
            }
        }
    }
};


class Bob : public Kit::TShell::Command::Base
{
public:
    /// See Kit::TShell::Command
    const char* getUsage() const noexcept { return "bob on|off [delay]\nbob restart <delay>"; }

    /// See Kit::TShell::Command
    const char* getHelp() const noexcept { return "  Sets the test trace output to on/off and delay time between msgs\n  Restarts the TShell after the specified delay (msecs)"; }

    ///
    Apple& m_app;


public:
    /// Constructor
    Bob( Kit::Container::OrderedList<Kit::TShell::ICommand>& commandList,
         Apple&                                              application,
         Permissions_T                                       permission = OPTION_KIT_TSHELL_SECURITY_DEFAULT_PERMISSIONS ) noexcept
        : Base( commandList, "bob", permission )
        , m_app( application )
    {
    }

public:
    /// See Kit::TShell::Command
    Result_T execute( Kit::TShell::IContext& context, char* cmdString ) noexcept
    {
        Kit::Text::Tokenizer::TextBlock tokens( cmdString );
        Kit::Text::IString&             token = context.getInputBuffer();

        // Error checking
        if ( tokens.numParameters() > 3 || tokens.numParameters() < 2 )
        {
            return Result_T::CMD_ERR_BAD_SYNTAX;
        }

        if ( tokens.numParameters() == 3 && tokens.getParameter( 1 )[0] == 'r' )
        {
            uint32_t restartDelay = 500;
            Kit::Text::StringTo::unsignedInt( restartDelay, tokens.getParameter( 2 ) );
            m_app.restartTShell( restartDelay );
        }
        else
        {
            // Set output state
            token = tokens.getParameter( 1 );
            m_app.setOutputState( token == "on" ? true : false );

            // Set delay
            if ( tokens.numParameters() > 2 )
            {
                uint32_t newdelay = 250;
                Kit::Text::StringTo::unsignedInt( newdelay, tokens.getParameter( 2 ) );
                m_app.setDelay( newdelay );
            }
        }

        return Result_T::CMD_SUCCESS;
    }
};

}  // end anonymous namespace

////////////////////////////////////

static Kit::Container::OrderedList<Kit::TShell::ICommand> commandList_;
static Kit::TShell::NoSecurity                            securityPolicy_;
static Kit::System::Mutex                                 lock_;
static Kit::Framing::StreamSource                         streamSrc_;
static Kit::Framing::StreamDestination                    streamDst_;

static Kit::TShell::Processor tshell_( commandList_,
                                       streamSrc_,
                                       streamDst_,
                                       securityPolicy_,
                                       Kit::System::PrivateLocks::tracingOutput() );

static Kit::TShell::StdioThread    stdioThread_( tshell_ );
static Kit::TShell::Command::Bye   byeCmd_( commandList_ );
static Kit::TShell::Command::Help  helpCmd_( commandList_ );
static Kit::TShell::Command::Echo  echoCmd_( commandList_ );
static Kit::TShell::Command::Trace traceCmd_( commandList_ );
static Kit::TShell::Command::Wait  waitCmd_( commandList_ );
static Apple                       app_( stdioThread_ );
static Bob                         bobCmd_( commandList_, app_ );


void shell_test( Kit::Io::IInput& infd, Kit::Io::IOutput& outfd )
{
    stdioThread_.launchTShell( infd, outfd );

    // Create thread for my mock-application to run in
    Kit::System::Thread::create( app_, "APP-BOB" );

    // Wait forever - the 'bye' command is responsible for exiting
    Kit::System::sleep( 0xFFFFFFFF );
}

echo
wait 1000
echo
