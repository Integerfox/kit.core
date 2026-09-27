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
#include "Kit/Io/Serial/RPi/RP2xxx/Stdio/Input.h"
#include "Kit/Io/Serial/RPi/RP2xxx/Stdio/Output.h"
#include "Kit/Io/Serial/_0test/_hw_nothreads/test.h"
#include "pico/time.h"

/// Create my streams
static Kit::Io::Serial::RPi::RP2xxx::Stdio::Input  infd_;
static Kit::Io::Serial::RPi::RP2xxx::Stdio::Output outfd_;


/*-----------------------------------------------------------*/
int main( void )
{
    // Initialize the board
    Bsp_initialize();

    // Go run the test(s) (Note: This method should never return)
    test( infd_, outfd_ );

    // I should never get here!
    for ( ;; )
        ;
    return 0;
}

uint32_t elapsedTimeMs()
{
    return (uint32_t)( to_ms_since_boot( get_absolute_time() ) );
}
