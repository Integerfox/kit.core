/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "helpers.h"
#include "Kit/TShell/Maker.h"

#include "Kit/Dm/TShell/Dm.h"
#include "Kit/Dm/ModelDatabase.h"
#include "Kit/Dm/Mp/Int32.h"



////////////////////////////////////////////////////////////////////////////////

extern Kit::Container::SList<Kit::TShell::Command>   cmdlist;

static Kit::TShell::Maker cmdProcessor_( cmdlist );

static Kit::TShell::Cmd::Help    helpCmd_( cmdlist );
static Kit::TShell::Cmd::Bye     byeCmd_( cmdlist );
static Kit::TShell::Cmd::Trace   traceCmd_( cmdlist );
static Kit::TShell::Cmd::TPrint  tprintCmd_( cmdlist );


static Apple   mockApp;
static Bob     bobCmd( cmdlist, mockApp );


// Allocate/create my Model Database
static Kit::Dm::ModelDatabase   modelDb_( "ignoreThisParameter_usedToInvokeTheStaticConstructor" );

static Kit::Dm::TShell::Dm		dmCmd_( cmdlist, modelDb_, "dm" );

// Allocate my Model Points
static Kit::Dm::Mp::Int32       mp_apple_( modelDb_, "APPLE"  );

static Kit::Dm::Mp::Int32       mp_orange_( modelDb_, "ORANGE", 32 );
