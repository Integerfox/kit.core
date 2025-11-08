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
#include "Kit/System/Api.h"
#include "Kit/System/Trace.h"
#include "Kit/Bsp/ST/NUCLEO-F413ZH/stdio.h"

extern void echo_test( Kit::Io::IInputOutput& fd );
extern size_t getErrorCounts( bool clearCounts = false );

/*-----------------------------------------------------------*/
int main( void )
{
    // Initialize the board
    Bsp_initialize();

    // Initialize CPL
    Kit::System::initialize();

    // Go run the test(s) (Note: This method should never return)
    echo_test( g_bspConsoleStream );

    // I should never get here!
    for ( ;; );
    return 0;
}


size_t getErrorCounts( bool clearCounts )
{
    return g_bspConsoleStream.getRxErrorsCounts( clearCounts );
}

