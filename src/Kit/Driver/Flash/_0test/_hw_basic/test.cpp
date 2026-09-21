/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file
    Hardware-in-loop runner: calls the platform-independent Flash test suite
    and then blinks the debug LED forever as a visual "test complete" signal.
*/

#include "test.h"
#include "Kit/Driver/Flash/_0test/test.h"
#include "Kit/Bsp/Api.h"
#include "Kit/System/Api.h"

using namespace Kit::System;


////////////////////////////////////////////////////////////////////////////////
void runHwTests( Kit::Driver::Flash::IApi& flash )
{
    bool result = runtests( flash );

    // Blink forever as a visual "test complete" heartbeat.
    for ( ;; )
    {
        Bsp_toggle_debug1();
        if ( result )
        {
        sleep( 500 );
        }
        else
        {
            sleep( 100 );
        }
    }
}
