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
#include "Kit/TShell/Processor.h"
#include "Kit/TShell/NoSecurity.h"
#include "Kit/System/Api.h"
#include "Kit/System/Private.h"
#include "Kit/TShell/Command/Bye.h"
#include "Kit/TShell/Command/Help.h"
#include "Kit/TShell/Command/Echo.h"
#include "Kit/TShell/StdioThread.h"
#include "Kit/Time/TShell/Time.h"

#define SECT_ "_0test"

///
using namespace Kit::TShell;
Kit::Container::OrderedList<Kit::TShell::ICommand> g_commandList( "ignore_static_constructor" );

static NoSecurity                      securityPolicy_;
static Kit::Framing::StreamSource      streamSrc_;
static Kit::Framing::StreamDestination streamDst_;
static Processor                       tshell_( g_commandList,
                          streamSrc_,
                          streamDst_,
                          securityPolicy_,
                          Kit::System::PrivateLocks::tracingOutput() );
static StdioThread                     stdioThread_( tshell_ );
static Command::Bye                    byeCmd_( g_commandList );
static Command::Help                   helpCmd_( g_commandList );
static Command::Echo                   echoCmd_( g_commandList );
static Kit::Time::TShell::Time         timeCmd_( g_commandList );

void shell_test( Kit::Io::IInput& infd, Kit::Io::IOutput& outfd )
{
    stdioThread_.launchTShell( infd, outfd );

    // Wait forever - the 'bye' command is responsible for exiting
    Kit::System::sleep( 0xFFFFFFFF );
}
