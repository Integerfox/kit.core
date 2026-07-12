/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file
    Hardware test for Kit::Driver::Dio.  Exercises the digital output driver
    by toggling pins and verifying state transitions.  Visual pass indicator
    is a toggling debug LED.

    This test is platform-independent: it operates solely on the
    Kit::Driver::Dio::IOutput interface so that it can be re-used for all
    concrete Dio drivers.  The caller (e.g. a platform-specific main()) is
    responsible for creating/starting the concrete driver, creating the
    thread this runs in, and starting the OSAL scheduler.
*/

#include "test.h"
#include "Kit/System/Api.h"
#include "Kit/System/Shutdown.h"
#include "Kit/System/FatalError.h"
#include "Kit/System/Trace.h"
#include "Kit/Bsp/Api.h"
#include <cstdint>


#define SECT_ "_0test"

///
using namespace Kit::System;


////////////////////////////////////////////////////////////////////////////////
void runtests( Kit::Driver::Dio::IOutput& output )
{
    KIT_SYSTEM_TRACE_MSG( SECT_, "Starting DIO test..." );

    for ( ;; )
    {
        // Test assert
        output.assertPin();
        sleep( 100 );
        if ( !output.isAsserted() )
        {
            FatalError::logf( Shutdown::eFAILURE, "DIO: assertPin() did not set asserted state" );
        }

        // Test deassert
        output.deassertPin();
        sleep( 100 );
        if ( output.isAsserted() )
        {
            FatalError::logf( Shutdown::eFAILURE, "DIO: deassertPin() did not clear asserted state" );
        }

        // Test set(true)
        output.set( true );
        sleep( 100 );
        if ( !output.isAsserted() )
        {
            FatalError::logf( Shutdown::eFAILURE, "DIO: set(true) did not set asserted state" );
        }

        // Test set(false)
        output.set( false );
        sleep( 100 );
        if ( output.isAsserted() )
        {
            FatalError::logf( Shutdown::eFAILURE, "DIO: set(false) did not clear asserted state" );
        }

        // Test toggle from deasserted
        output.deassertPin();
        output.toggle();
        sleep( 100 );
        if ( !output.isAsserted() )
        {
            FatalError::logf( Shutdown::eFAILURE, "DIO: toggle() from deasserted did not assert" );
        }

        // Test toggle from asserted
        output.toggle();
        sleep( 100 );
        if ( output.isAsserted() )
        {
            FatalError::logf( Shutdown::eFAILURE, "DIO: toggle() from asserted did not deassert" );
        }

        // Visual heartbeat - toggles debug LED to indicate test is running
        Bsp_toggle_debug1();
        sleep( 500 );
    }
}
