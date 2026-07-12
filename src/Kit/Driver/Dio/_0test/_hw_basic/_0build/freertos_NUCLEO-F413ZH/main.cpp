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
#include "Kit/System/Thread.h"
#include "Kit/System/Trace.h"
#include "Kit/Driver/Dio/ST/M32F4/Output.h"
#include "Kit/Driver/Dio/_0test/_hw_basic/test.h"
#include <new>

#define SECT_ "_0test"

///
using namespace Kit::System;


////////////////////////////////////////////////////////////////////////////////
namespace {

/// Runnable that executes the platform-independent DIO test in its own thread
class TestRunnable : public IRunnable
{
public:
    Kit::Driver::Dio::IOutput& m_output;

public:
    TestRunnable( Kit::Driver::Dio::IOutput& output )
        : m_output( output )
    {
    }

public:
    void entry() noexcept override { runtests( m_output ); }
};

};  // end namespace


////////////////////////////////////////////////////////////////////////////////
int main( void )
{
    // Initialize the board
    Bsp_initialize();
    printf( "\n**** KIT-DRIVER-DIO TEST APPLICATION STARTED ****\n\n" );  // May not print on all targets

    // Initialize KIT
    Kit::System::initialize();

    KIT_SYSTEM_TRACE_ENABLE();
    KIT_SYSTEM_TRACE_ENABLE_SECTION( "_0test" );
    KIT_SYSTEM_TRACE_SET_INFO_LEVEL( Kit::System::Trace::eVERBOSE );
    KIT_SYSTEM_TRACE_MSG( SECT_, "KIT System initialized" );

    // NOTE: The Runnable objects are created on the Heap - because depending on
    //       the platform - FreeRTOS will corrupt the raw 'main stack' when it
    //       starts the first thread.

    // Create the concrete driver (caller responsibility).
    // Uses the LD2 LED pin (GPIOB, GPIO_PIN_7 on NUCLEO-F413ZH).
    Kit::Driver::Dio::ST::M32F4::Output* testOutput =
        new ( std::nothrow ) Kit::Driver::Dio::ST::M32F4::Output( LD2_GPIO_Port, LD2_Pin );

    // Start the driver (caller responsibility).  Initial output state is
    // de-asserted (logical false).
    static Kit::Driver::Dio::ST::M32F4::Output::StartArgs_T startArgs( false );
    testOutput->start( &startArgs );

    // Create the test thread (caller responsibility)
    TestRunnable* testRunnable = new ( std::nothrow ) TestRunnable( *testOutput );
    Thread::create( *testRunnable, "DioTest" );

    // Start the scheduler (caller responsibility)
    KIT_SYSTEM_TRACE_MSG( SECT_, "Starting scheduler..." );
    enableScheduling();

    // I should never get here!
    for ( ;; );
    return 0;
}
