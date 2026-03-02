/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "colony_config.h"
#include "Kit/System/Api.h"

#include "Bsp/Api.h"
#include "Kit/System/Thread.h"
#include "Kit/System/Mutex.h"
#include "Kit/System/Private_.h"
#include "Kit/Text/atob.h"
#include "Kit/Text/FString.h"
#include "Kit/Text/Tokenizer/TextBlock.h"

#include "Kit/TShell/Cmd/Help.h"
#include "Kit/TShell/Cmd/Bye.h"
#include "Kit/TShell/Cmd/Trace.h"
#include "Kit/TShell/Cmd/TPrint.h"


#define SECT_     "_0test"

////////////////////////////////////////////////////////////////////////////////
namespace {


class Apple : public Kit::System::Runnable
{
public:
	///
	Kit::System::Mutex m_lock;
	///
	unsigned long      m_delay;
	///
	bool               m_outputTrace;

public:
	///
	Apple()
		:m_delay( 250 )
		, m_outputTrace( false )
	{
	}

public:
	/// 
	void setOutputState( bool newstate )
	{
		Kit::System::Mutex::ScopeBlock lock( m_lock );
		m_outputTrace = newstate;
	}

	/// 
	void setDelay( unsigned long newdelay )
	{
		Kit::System::Mutex::ScopeBlock lock( m_lock );
		m_delay = newdelay;
	}

public:
	///
	void appRun()
	{
		unsigned counter = 0;
		for ( ;;)
		{
			Bsp_Api_toggle_debug1();
			counter++;

			m_lock.lock();
			bool          output = m_outputTrace;
			unsigned long delay  = m_delay;
			m_lock.unlock();

			Kit::System::Api::sleep( delay );
			if ( output )
			{
				CPL_SYSTEM_TRACE_MSG( SECT_, ( "Trace (_0test): loop counter=%u", counter ) );
				CPL_SYSTEM_TRACE_MSG( "bob", ( "Trace (bob): loop counter=%u", counter ) );
			}
		}
	}
};


class Bob : public Kit::TShell::Cmd::Command
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
	Bob( Kit::Container::SList<Kit::TShell::Command>& commandList, Apple& application ) noexcept
		:Command( commandList, "bob" )
		, m_app( application )
	{
	}

public:
	/// See Kit::TShell::Command
	Kit::TShell::Command::Result_T execute( Kit::TShell::Context_& context, char* cmdString, Kit::Io::Output& outfd ) noexcept
	{
		Kit::Text::Tokenizer::TextBlock tokens( cmdString, context.getDelimiterChar(), context.getTerminatorChar(), context.getQuoteChar(), context.getEscapeChar() );
		Kit::Text::String& token = context.getTokenBuffer();

		// Error checking
		if ( tokens.numParameters() > 3 || tokens.numParameters() < 2 )
		{
			return Command::eERROR_INVALID_ARGS;
		}

		// Set output state
		token = tokens.getParameter( 1 );
		m_app.setOutputState( token == "on" ? true : false );

		// Set delay
		if ( tokens.numParameters() > 2 )
		{
			unsigned long newdelay = 250;
			Kit::Text::a2ul( newdelay, tokens.getParameter( 2 ) );
			m_app.setDelay( newdelay );
		}

		return Command::eSUCCESS;
	}
};


};  // end namespace






