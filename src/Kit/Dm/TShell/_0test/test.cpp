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
#include "Kit/TShell/Command/Trace.h"
#include "Kit/TShell/Command/Wait.h"
#include "Kit/TShell/StdioThread.h"
#include "Kit/Dm/TShell/Read.h"
#include "Kit/Dm/TShell/Write.h"
#include "Kit/Dm/ModelDatabase.h"
#include "Kit/Dm/Mp/String.h"
#include "Kit/Dm/Mp/Int32.h"
#include <inttypes.h>

#define SECT_ "_0test"

///
using namespace Kit::System;
using namespace Kit::TShell;
using namespace Kit::Type;
using namespace Kit::Dm;
using namespace Kit::Dm::TShell;


static ModelDatabase          mpDatabase_( "ignore_static_constructor" );
//
static Mp::StringAllocate<16> mpS1_( mpDatabase_,
                                     "str1" );
static Mp::StringAllocate<8>  mpS2_( mpDatabase_,
                                    "str2",
                                    "bob" );
//
static Mp::Int32 mpInt1_( mpDatabase_,
                          "int1",
                          42 );
static Mp::Int32 mpInt2_( mpDatabase_,
                          "int2" );


Kit::Container::OrderedList<Kit::TShell::ICommand> g_commandList( "ignore_static_constructor" );
static NoSecurity                                  securityPolicy_;
static Kit::Framing::StreamSource                  streamSrc_;
static Kit::Framing::StreamDestination             streamDst_;

static Processor tshell_( g_commandList,
                          streamSrc_,
                          streamDst_,
                          securityPolicy_,
                          Kit::System::PrivateLocks::tracingOutput() );

static StdioThread    stdioThread_( tshell_ );
static Command::Bye   byeCmd_( g_commandList );
static Command::Help  helpCmd_( g_commandList );
static Command::Echo  echoCmd_( g_commandList );
static Command::Trace traceCmd_( g_commandList );
static Command::Wait  waitCmd_( g_commandList );
static Read           dmrCmd_( g_commandList, mpDatabase_ );
static Write          dmwCmd_( g_commandList, mpDatabase_ );

void shell_test( Kit::Io::IInput& infd, Kit::Io::IOutput& outfd )
{
    stdioThread_.launchTShell( infd, outfd );

    // Wait forever - the 'bye' command is responsible for exiting
    Kit::System::sleep( 0xFFFFFFFF );
}