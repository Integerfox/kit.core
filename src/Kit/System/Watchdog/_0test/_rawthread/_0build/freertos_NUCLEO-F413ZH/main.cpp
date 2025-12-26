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

    KIT_SYSTEM_TRACE_MSG( SECT_, "\r\n" );
    KIT_SYSTEM_TRACE_MSG( SECT_, "========================================\r\n" );
    KIT_SYSTEM_TRACE_MSG( SECT_, "       BOOT SEQUENCE STARTED\r\n" );
    KIT_SYSTEM_TRACE_MSG( SECT_, "========================================\r\n" );
    KIT_SYSTEM_TRACE_MSG( SECT_, "\r\n" );

    // Check if this is a watchdog reset BEFORE starting the test
    if ( __HAL_RCC_GET_FLAG( RCC_FLAG_IWDGRST ) != RESET )
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "========================================\r\n" );
        KIT_SYSTEM_TRACE_MSG( SECT_, "*** WATCHDOG RESET DETECTED ***\r\n" );
        KIT_SYSTEM_TRACE_MSG( SECT_, "========================================\r\n" );
        KIT_SYSTEM_TRACE_MSG( SECT_, "Test PASSED - System was reset by watchdog\r\n" );

        __HAL_RCC_CLEAR_RESET_FLAGS();

        // Keep both LEDs on to indicate success
        Bsp_turn_on_debug1();
        Bsp_turn_on_debug2();

        for ( ;; )
        {
            HAL_Delay( 1000 );
        }
    }

    KIT_SYSTEM_TRACE_MSG( SECT_, "KIT System initialized\r\n" );

    // Run the platform-independent test (Note: This method should never return)
    runTests();

    // Should never reach here
    return 0;
}
