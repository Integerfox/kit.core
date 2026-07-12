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

    This test is platform-independent: it operates solely on the
    Kit::Driver::SPI::IHalfDuplex and Kit::Driver::Dio::IOutput interfaces so
    that it can be re-used for all concrete SPI drivers.  The caller (e.g. a
    platform-specific main()) is responsible for creating/starting the
    concrete drivers, creating the thread this runs in, and starting the OSAL
    scheduler.

    NOTE: This test requires an SPI loopback connection (MOSI connected to MISO).
*/

#include "test.h"
#include "Kit/System/Api.h"
#include "Kit/System/Shutdown.h"
#include "Kit/System/FatalError.h"
#include "Kit/System/Trace.h"
#include "Kit/Bsp/Api.h"
#include <cstdint>
#include <cstring>


#define SECT_ "_0test"

///
using namespace Kit::System;


////////////////////////////////////////////////////////////////////////////////
static constexpr size_t TEST_BUFFER_SIZE = 16;


void runtests( Kit::Driver::SPI::IHalfDuplex& spi,
               Kit::Driver::Dio::IOutput&     cs )
{
    KIT_SYSTEM_TRACE_MSG( SECT_, "Starting SPI test..." );

    // Deassert CS (idle high)
    cs.deassertPin();

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
        cs.assertPin();
        bool writeOk = spi.write( txBuf, TEST_BUFFER_SIZE );
        cs.deassertPin();

        if ( !writeOk )
        {
            FatalError::logf( Shutdown::eFAILURE, "SPI write() failed on iteration %u", iteration );
        }

        sleep( 10 );

        // Assert CS, read data, deassert CS
        memset( rxBuf, 0, TEST_BUFFER_SIZE );
        cs.assertPin();
        bool readOk = spi.read( rxBuf, TEST_BUFFER_SIZE );
        cs.deassertPin();

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
