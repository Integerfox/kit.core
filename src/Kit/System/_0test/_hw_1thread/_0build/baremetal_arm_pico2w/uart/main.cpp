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
#include "Kit/System/ElapsedTime.h"
#include "Kit/System/EventLoop.h"
#include "Kit/System/Trace.h"
#include <cstdint>
#include <inttypes.h>

#define SECT_ "_0test"

extern void runtests( size_t maxLoopCount );

/*-----------------------------------------------------------*/
int main( void )
{
    // Initialize the board and the KIT library
    Bsp_initialize();
    Kit::System::initialize();

    KIT_SYSTEM_TRACE_ENABLE();
    KIT_SYSTEM_TRACE_ENABLE_SECTION( "_0test" );
    //    KIT_SYSTEM_TRACE_SET_INFO_LEVEL( Kit::System::Trace::eINFO );
    KIT_SYSTEM_TRACE_SET_INFO_LEVEL( Kit::System::Trace::eVERBOSE );
    KIT_SYSTEM_TRACE_MSG( SECT_, "\n**** KIT-SYSTEM TEST APPLICATION STARTED ****" );  // May not print on all targets

    // Go run the test(s) (Note: This method should never return)
    runtests( 10000 );  // 10000 Loop * 0.5sec = 5000 seconds max

    // I should never get here!
    for ( ;; )
        ;
    return 0;
}
