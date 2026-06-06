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
*/

#include "Kit/Bsp/Api.h"
#include "Kit/System/Api.h"
#include "Kit/System/Shutdown.h"
#include "Kit/System/Thread.h"
#include "Kit/System/FatalError.h"
#include "Kit/System/ElapsedTime.h"
#include "Kit/System/Trace.h"
#include "Kit/Driver/Dio/ST/M32F4/Output.h"
#include <cstdint>


#define SECT_ "_0test"

///
using namespace Kit::System;


////////////////////////////////////////////////////////////////////////////////
namespace {


class DioTestRunnable : public IRunnable
{
public:
    Kit::Driver::Dio::IOutput& m_output;

public:
    DioTestRunnable( Kit::Driver::Dio::IOutput& output )
        : m_output( output )
    {
    }

public:
    void entry() noexcept override
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "Starting DIO test..." );

        // Start the driver
        if ( !m_output.start() )
        {
            FatalError::logf( Shutdown::eFAILURE, "DIO output start() failed" );
        }

        for ( ;; )
        {
            // Test assert
            m_output.assertPin();
            sleep( 100 );
            if ( !m_output.isAsserted() )
            {
                FatalError::logf( Shutdown::eFAILURE, "DIO: assertPin() did not set asserted state" );
            }

            // Test deassert
            m_output.deassertPin();
            sleep( 100 );
            if ( m_output.isAsserted() )
            {
                FatalError::logf( Shutdown::eFAILURE, "DIO: deassertPin() did not clear asserted state" );
            }

            // Test set(true)
            m_output.set( true );
            sleep( 100 );
            if ( !m_output.isAsserted() )
            {
                FatalError::logf( Shutdown::eFAILURE, "DIO: set(true) did not set asserted state" );
            }

            // Test set(false)
            m_output.set( false );
            sleep( 100 );
            if ( m_output.isAsserted() )
            {
                FatalError::logf( Shutdown::eFAILURE, "DIO: set(false) did not clear asserted state" );
            }

            // Test toggle from deasserted
            m_output.deassertPin();
            m_output.toggle();
            sleep( 100 );
            if ( !m_output.isAsserted() )
            {
                FatalError::logf( Shutdown::eFAILURE, "DIO: toggle() from deasserted did not assert" );
            }

            // Test toggle from asserted
            m_output.toggle();
            sleep( 100 );
            if ( m_output.isAsserted() )
            {
                FatalError::logf( Shutdown::eFAILURE, "DIO: toggle() from asserted did not deassert" );
            }

            // Visual heartbeat - toggles debug LED to indicate test is running
            Bsp_toggle_debug1();
            sleep( 500 );
        }
    }
};

};  // end namespace


////////////////////////////////////////////////////////////////////////////////

// NOTE: I create the Runnable objects on the Heap - because depending on the
//       platform - FreeRTOS will corrupt the raw 'main stack' when it starts
//       the first thread.

void runtests( void )
{
    // Create the DIO output driver for a test pin
    // Uses LD2 LED pin (GPIOB, GPIO_PIN_7 on NUCLEO-F413ZH)
    Kit::Driver::Dio::ST::M32F4::Output* testOutput =
        new ( std::nothrow ) Kit::Driver::Dio::ST::M32F4::Output( LD2_GPIO_Port, LD2_Pin );

    // Create and start the test thread
    DioTestRunnable* testRunnable = new ( std::nothrow ) DioTestRunnable( *testOutput );
    Thread::create( *testRunnable, "DioTest" );

    // Start the scheduler
    KIT_SYSTEM_TRACE_MSG( SECT_, "Starting scheduler..." );
    enableScheduling();
}
