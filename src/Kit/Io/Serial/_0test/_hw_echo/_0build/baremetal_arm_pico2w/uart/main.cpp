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
#include "Kit/Io/IInputOutput.h"

extern void echo_test( Kit::Io::IInputOutput& fd );
extern size_t getErrorCounts( bool clearCounts = false );

// Get access to the BSP's console stream
extern Kit::Io::IInputOutput& g_bspConsoleStream;

/*-----------------------------------------------------------*/
int main( void )
{
    // Initialize the board
    Bsp_initialize();

    // Initialize KIT
    Kit::System::initialize();

    // Go run the test(s) (Note: This method should never return)
    echo_test( g_bspConsoleStream );

    // I should never get here!
    for ( ;; );
    return 0;
}


// Not supported in this test
size_t getErrorCounts( bool clearCounts )
{
    return 0;
}

