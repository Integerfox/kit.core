/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file
    Hardware test for Kit::Driver::SPI.  Exercises the SPI half-duplex driver
    by performing loopback write/read operations.  Visual pass indicator
    is a toggling debug LED.

    NOTE: This test requires an SPI loopback connection (MOSI connected to MISO)
          on the SPI3 peripheral of the NUCLEO-F413ZH board.
*/

#include "Kit/Bsp/Api.h"
#include "Kit/System/Api.h"
#include "Kit/System/Shutdown.h"
#include "Kit/System/Thread.h"
#include "Kit/System/FatalError.h"
#include "Kit/System/ElapsedTime.h"
#include "Kit/System/Trace.h"
#include "Kit/Driver/SPI/ST/M32F4/Polled.h"
#include "Kit/Driver/Dio/ST/M32F4/Output.h"
#include <cstdint>
#include <cstring>


#define SECT_ "_0test"

///
using namespace Kit::System;


////////////////////////////////////////////////////////////////////////////////
namespace {

static constexpr size_t TEST_BUFFER_SIZE = 16;

class SpiTestRunnable : public IRunnable
{
public:
    Kit::Driver::SPI::IHalfDuplex& m_spi;
    Kit::Driver::Dio::IOutput&     m_cs;

public:
    SpiTestRunnable( Kit::Driver::SPI::IHalfDuplex& spi,
                     Kit::Driver::Dio::IOutput&     cs )
        : m_spi( spi )
        , m_cs( cs )
    {
    }

public:
    void entry() noexcept override
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "Starting SPI test..." );

        // Start the drivers
        if ( !m_spi.start() )
        {
            FatalError::logf( Shutdown::eFAILURE, "SPI start() failed" );
        }
        if ( !m_cs.start() )
        {
            FatalError::logf( Shutdown::eFAILURE, "SPI CS start() failed" );
        }

        // Deassert CS (idle high)
        m_cs.deassertPin();

        uint8_t txBuf[TEST_BUFFER_SIZE];
        uint8_t rxBuf[TEST_BUFFER_SIZE];
        uint8_t iteration = 0;

        for ( ;; )
        {
            // Fill transmit buffer with test pattern
            for ( size_t i = 0; i < TEST_BUFFER_SIZE; i++ )
            {
                txBuf[i] = static_cast<uint8_t>( iteration + i );
            }

            // Assert CS, write data, deassert CS
            m_cs.assertPin();
            bool writeOk = m_spi.write( txBuf, TEST_BUFFER_SIZE );
            m_cs.deassertPin();

            if ( !writeOk )
            {
                FatalError::logf( Shutdown::eFAILURE, "SPI write() failed on iteration %u", iteration );
            }

            sleep( 10 );

            // Assert CS, read data, deassert CS
            memset( rxBuf, 0, TEST_BUFFER_SIZE );
            m_cs.assertPin();
            bool readOk = m_spi.read( rxBuf, TEST_BUFFER_SIZE );
            m_cs.deassertPin();

            if ( !readOk )
            {
                FatalError::logf( Shutdown::eFAILURE, "SPI read() failed on iteration %u", iteration );
            }

            // Visual heartbeat - toggles debug LED to indicate test is running
            Bsp_toggle_debug1();
            iteration++;
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
    // Create the SPI driver using SPI3 peripheral
    extern SPI_HandleTypeDef hspi3;
    Kit::Driver::SPI::ST::M32F4::Polled* spiDriver =
        new ( std::nothrow ) Kit::Driver::SPI::ST::M32F4::Polled( &hspi3 );

    // Create CS pin driver (using a GPIO configured as output for chip select)
    // Uses LD3 LED pin (GPIOB, GPIO_PIN_14 on NUCLEO-F413ZH) as a CS substitute for testing
    Kit::Driver::Dio::ST::M32F4::Output* csPin =
        new ( std::nothrow ) Kit::Driver::Dio::ST::M32F4::Output( LD3_GPIO_Port, LD3_Pin );

    // Create and start the test thread
    SpiTestRunnable* testRunnable = new ( std::nothrow ) SpiTestRunnable( *spiDriver, *csPin );
    Thread::create( *testRunnable, "SpiTest" );

    // Start the scheduler
    KIT_SYSTEM_TRACE_MSG( SECT_, "Starting scheduler..." );
    enableScheduling();
}
