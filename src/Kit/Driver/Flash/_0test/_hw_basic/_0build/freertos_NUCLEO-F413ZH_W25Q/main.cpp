/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file
    Platform-specific entry point for the Kit::Driver::Flash hardware test on
    an STM32F4 (NUCLEO-F413ZH) target fitted with a W25Q SPI NOR flash device.

    This file is responsible for creating and starting the concrete SPI and
    W25Q Flash drivers, creating the test thread, and starting the OSAL
    scheduler.  The actual (platform-independent) test logic lives in
    src/Kit/Driver/Flash/_0test/_hw_basic/test.cpp.
*/

#include "Kit/Bsp/Api.h"
#include "Kit/System/Api.h"
#include "Kit/System/FatalError.h"
#include "Kit/System/Shutdown.h"
#include "Kit/System/Thread.h"
#include "Kit/System/Trace.h"
#include "Kit/Driver/SPI/ST/M32F4/Polled.h"
#include "Kit/Driver/Dio/ST/M32F4/Output.h"
#include "Kit/Driver/Flash/W25Q/StdSpi24.h"
#include "Kit/Driver/Flash/_0test/_hw_basic/test.h"
#include "spi.h"
#include <new>

#define SECT_ "_0test"

///
using namespace Kit::System;
using namespace Kit::Driver;


////////////////////////////////////////////////////////////////////////////////
namespace {

/// Runnable that executes the platform-independent Flash test in its own thread
class TestRunnable : public IRunnable
{
public:
    SPI::ST::M32F4::Polled& m_spi;
    Flash::W25Q::StdSpi24&  m_flash;

public:
    TestRunnable( SPI::ST::M32F4::Polled& spi, Flash::W25Q::StdSpi24& flash )
        : m_spi( spi )
        , m_flash( flash )
    {
    }

public:
    void entry() noexcept override
    {
        // The drivers are started here - i.e. once the scheduler is running -
        // because StdSpi24::start() calls Kit::System::sleep(), which maps to
        // vTaskDelay() and must not be used before the scheduler starts.
        m_spi.start();
        m_flash.start();

        runHwTests( m_flash );
    }
};

};  // end namespace


////////////////////////////////////////////////////////////////////////////////
int main( void )
{
    // Initialize the board (HAL, clocks, GPIOs, UART, SPI)
    Bsp_initialize();

    // Small delay to let the UART stabilize
    HAL_Delay( 200 );

    // Initialize KIT
    Kit::System::initialize();

    KIT_SYSTEM_TRACE_ENABLE();
    KIT_SYSTEM_TRACE_ENABLE_SECTION( SECT_ );
    KIT_SYSTEM_TRACE_SET_INFO_LEVEL( Kit::System::TraceLevel::eVERBOSE );
    KIT_SYSTEM_TRACE_MSG( SECT_, "KIT System initialized" );

    // NOTE: The driver/runnable objects are created on the Heap - because
    //       depending on the platform - FreeRTOS will corrupt the raw 'main
    //       stack' when it starts the first thread.

    // Create the concrete drivers (caller responsibility).  Uses the SPI3
    // peripheral and the CS_SPI_Flash chip-select pin from the board's MX
    // configuration.  NOTE: the drivers are STARTED by the test thread, not
    // here, because StdSpi24::start() requires a running scheduler.
    SPI::ST::M32F4::Polled* spiDriver =
        new ( std::nothrow ) SPI::ST::M32F4::Polled( &hspi3 );
    Dio::ST::M32F4::Output* csPin =
        new ( std::nothrow ) Dio::ST::M32F4::Output( CS_SPI_Flash_GPIO_Port, CS_SPI_Flash_Pin, false );
    if ( spiDriver == nullptr || csPin == nullptr )
    {
        FatalError::logf( Shutdown::eFAILURE, "Out of memory: SPI/CS driver" );
    }

    Flash::W25Q::StdSpi24* flashDriver =
        new ( std::nothrow ) Flash::W25Q::StdSpi24( *spiDriver, *csPin, Flash::W25Q::W25Q128 );
    TestRunnable* testRunnable =
        new ( std::nothrow ) TestRunnable( *spiDriver, *flashDriver );
    if ( flashDriver == nullptr || testRunnable == nullptr )
    {
        FatalError::logf( Shutdown::eFAILURE, "Out of memory: Flash driver/test runnable" );
    }

    // Create the test thread (caller responsibility)
    if ( Thread::create( *testRunnable, "FlashTest" ) == nullptr )
    {
        FatalError::logf( Shutdown::eFAILURE, "Failed to create test thread" );
    }

    // Start the scheduler - control transfers to testRunnable->entry()
    KIT_SYSTEM_TRACE_MSG( SECT_, "Starting scheduler..." );
    enableScheduling();

    // I should never get here!
    for ( ;; )
        ;
    return 0;
}
