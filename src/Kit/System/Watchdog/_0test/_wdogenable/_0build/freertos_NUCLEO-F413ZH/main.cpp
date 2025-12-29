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

#define SECT_ "_0test"

extern bool runTests();

/*-----------------------------------------------------------*/
int main( void )
{
    // Initialize the board (HAL, clocks, GPIOs, UART)
    Bsp_initialize();

    // Small delay to let UART stabilize
    HAL_Delay( 200 );

    // Initialize the KIT system
    Kit::System::initialize();

    KIT_SYSTEM_TRACE_ENABLE();
    KIT_SYSTEM_TRACE_ENABLE_SECTION( SECT_ );
    KIT_SYSTEM_TRACE_SET_INFO_LEVEL( Kit::System::Trace::eVERBOSE );

    // Run the platform-independent test (Note: This method should never return)
    runTests();

    // Should never reach here
    return 0;
}
