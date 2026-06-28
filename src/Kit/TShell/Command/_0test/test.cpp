/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Bsp/Api.h"
#include "Kit/Container/OrderedList.h"
#include "Kit/TShell/Processor.h"
#include "Kit/TShell/NoSecurity.h"
#include "Kit/System/Trace.h"
#include "Kit/System/Thread.h"
#include "Kit/System/Mutex.h"
#include "Kit/System/IRunnable.h"
#include "Kit/System/Api.h"
#include "Kit/System/Private.h"
#include "Kit/TShell/Command/Base.h"
#include "Kit/Text/Tokenizer/TextBlock.h"
#include "Kit/Text/StringTo.h"
#include <string>

#define SECT_ "_0test"

///
using namespace Kit::System;
using namespace Kit::TShell;
using namespace Kit::Type;


////////////////////////////////////////////////////////////////////////////////
namespace {


class Apple : public Kit::System::IRunnable
{
public:
    ///
    Kit::System::Mutex m_lock;
    ///
    unsigned long m_delay;
    ///
    bool m_outputTrace;

public:
    ///
    Apple()
        : m_delay( 250 )
        , m_outputTrace( false )
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
    void setDelay( unsigned long newdelay )
    {
        Kit::System::Mutex::ScopeLock criticalSection( m_lock );
        m_delay = newdelay;
    }

public:
    ///
    void appRun()
    {
        unsigned counter = 0;
        for ( ;; )
        {
            Bsp_toggle_debug1();
            counter++;

            m_lock.lock();
            bool          output = m_outputTrace;
            unsigned long delay  = m_delay;
            m_lock.unlock();

            Kit::System::sleep( delay );
            if ( output )
            {
                KIT_SYSTEM_TRACE_MSG( SECT_, "Trace (_0test): loop counter=%u", counter );
                KIT_SYSTEM_TRACE_MSG( "bob", "Trace (bob): loop counter=%u", counter );
            }
        }
    }
};


class Bob : public Kit::TShell::Command::Base
{
public:
    /// See Kit::TShell::Command
    const char* getUsage() const noexcept { return "bob on|off [delay]"; }

    /// See Kit::TShell::Command
    const char* getHelp() const noexcept { return "  Sets the test trace output to on/off and delay time between msgs"; }

    ///
    Apple& m_app;


public:
    /// Constructor
    Bob( Kit::Container::OrderedList<Kit::TShell::ICommand>& commandList,
         Apple&                                              application,
         Permissions_T                                       permission ) noexcept
        : Base( commandList, "bob", permission )
        , m_app( application )
    {
    }

public:
    /// See Kit::TShell::Command
    Result_T execute( Kit::TShell::IContext& context, char* cmdString ) noexcept
    {
        Kit::Text::Tokenizer::TextBlock tokens( cmdString,
                                                context.getDelimiterChar(),
                                                context.getTerminatorChar(),
                                                context.getQuoteChar(),
                                                context.getEscapeChar() );
        Kit::Text::IString&             token = context.getInputBuffer();

        // Error checking
        if ( tokens.numParameters() > 3 || tokens.numParameters() < 2 )
        {
            return Result_T::CMD_ERR_BAD_SYNTAX;
        }

        // Set output state
        token = tokens.getParameter( 1 );
        m_app.setOutputState( token == "on" ? true : false );

        // Set delay
        if ( tokens.numParameters() > 2 )
        {
            unsigned long newdelay = 250;
            Kit::Text::StringTo::unsignedInt( newdelay, tokens.getParameter( 2 ) );
            m_app.setDelay( newdelay );
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


///
extern void shell_test( Kit::Io::IInput& infd, Kit::Io::IOutput& outfd );

void shell_test( Kit::Io::IInput& infd, Kit::Io::IOutput& outfd )
{
    tshell_.launch( infd, outfd );

    // Create thread for my mock-application to run in
    Cpl::System::Thread::create( mockApp, "APP-BOB" );

    // Start the scheduler
    Cpl::System::Api::enableScheduling();

    // Wait forever - the 'bye' command is responsible for exiting
    Cpl::System::Api::sleep( 0xFFFFFFFF );
}