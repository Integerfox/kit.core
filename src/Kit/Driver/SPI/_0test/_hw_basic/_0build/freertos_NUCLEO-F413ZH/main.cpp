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
#include "Kit/Driver/SPI/ST/M32F4/Polled.h"
#include "Kit/Driver/Dio/ST/M32F4/Output.h"
#include "Kit/Driver/SPI/_0test/_hw_basic/test.h"
#include <new>

#define SECT_ "_0test"

///
using namespace Kit::System;


////////////////////////////////////////////////////////////////////////////////
namespace {

/// Runnable that executes the platform-independent SPI test in its own thread
class TestRunnable : public IRunnable
{
public:
    Kit::Driver::SPI::IHalfDuplex& m_spi;
    Kit::Driver::Dio::IOutput&     m_cs;

public:
    TestRunnable( Kit::Driver::SPI::IHalfDuplex& spi,
                  Kit::Driver::Dio::IOutput&     cs )
        : m_spi( spi )
        , m_cs( cs )
    {
    }

public:
    void entry() noexcept override { runtests( m_spi, m_cs ); }
};

};  // end namespace


////////////////////////////////////////////////////////////////////////////////
int main( void )
{
    // Initialize the board
    Bsp_initialize();
    printf( "\n**** KIT-DRIVER-SPI TEST APPLICATION STARTED ****\n\n" );  // May not print on all targets

    // Initialize KIT
    Kit::System::initialize();

    KIT_SYSTEM_TRACE_ENABLE();
    KIT_SYSTEM_TRACE_ENABLE_SECTION( "_0test" );
    KIT_SYSTEM_TRACE_SET_INFO_LEVEL( Kit::System::Trace::eVERBOSE );
    KIT_SYSTEM_TRACE_MSG( SECT_, "KIT System initialized" );

    // NOTE: The Runnable objects are created on the Heap - because depending on
    //       the platform - FreeRTOS will corrupt the raw 'main stack' when it
    //       starts the first thread.

    // Create the concrete drivers (caller responsibility).
    // Uses the SPI3 peripheral and the LD3 LED pin (GPIOB, GPIO_PIN_14 on
    // NUCLEO-F413ZH) as a chip-select substitute for loopback testing.
    extern SPI_HandleTypeDef hspi3;
    Kit::Driver::SPI::ST::M32F4::Polled* spiDriver =
        new ( std::nothrow ) Kit::Driver::SPI::ST::M32F4::Polled( &hspi3 );
    Kit::Driver::Dio::ST::M32F4::Output* csPin =
        new ( std::nothrow ) Kit::Driver::Dio::ST::M32F4::Output( LD3_GPIO_Port, LD3_Pin );

    // Start the drivers (caller responsibility).  The chip-select idles
    // de-asserted (logical false).
    spiDriver->start();
    static Kit::Driver::Dio::ST::M32F4::Output::StartArgs_T csStartArgs( false );
    csPin->start( &csStartArgs );

    // Create the test thread (caller responsibility)
    TestRunnable* testRunnable = new ( std::nothrow ) TestRunnable( *spiDriver, *csPin );
    Thread::create( *testRunnable, "SpiTest" );

    // Start the scheduler (caller responsibility)
    KIT_SYSTEM_TRACE_MSG( SECT_, "Starting scheduler..." );
    enableScheduling();

    // I should never get here!
    for ( ;; );
    return 0;
}
