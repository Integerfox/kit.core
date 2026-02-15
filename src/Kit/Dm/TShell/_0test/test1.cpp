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
#include "statics.h"


#include "Cpl/TShell/Stdio.h"
#include "Cpl/System/Api.h"
#include "Cpl/System/Assert.h"
#include "Cpl/System/Shutdown.h"
#include <stdio.h>

/// 
extern void shell_test( Cpl::Io::Input& infd, Cpl::Io::Output& outfd );


////////////////////////////////////////////////////////////////////////////////
static Cpl::TShell::Stdio shell_( cmdProcessor_ );


void shell_test( Cpl::Io::Input& infd, Cpl::Io::Output& outfd )
{
    // Start the shell
    shell_.launch( infd, outfd );

    // Create thread for my mock-application to run in
    Cpl::System::Thread::create( mockApp, "APP-BOB" );

    // Start the scheduler
    Cpl::System::Api::enableScheduling();

	// Give time for the commands to run
	Cpl::System::Api::sleep( 3 * 1000 );
	int32_t value;
	bool valid = mp_apple_.read( value );
	printf( "valid=%d\n", valid );
	CPL_SYSTEM_ASSERT( value == 111 );
	CPL_SYSTEM_ASSERT( valid);
	valid = mp_orange_.read( value );
	printf( "valid=%d\n", valid );
	CPL_SYSTEM_ASSERT( value == 32 );
	CPL_SYSTEM_ASSERT( valid);
	
	// Exit the application with a 'pass' result
	Cpl::System::Shutdown::success();
}


