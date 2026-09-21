#ifndef KIT_DRIVER_SPI_0TEST_HW_BASIC_TEST_H_
#define KIT_DRIVER_SPI_0TEST_HW_BASIC_TEST_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file
    Platform-independent hardware test for the Kit::Driver::SPI half-duplex
    driver.  A single test that can be re-used for all concrete SPI drivers.
*/


#include "Kit/Driver/SPI/IHalfDuplex.h"
#include "Kit/Driver/Dio/IOutput.h"


/** Runs the SPI loopback hardware test.  This method does NOT return.

    The caller is responsible for:
      - Creating the thread that this method executes in
      - Ensuring that the OSAL scheduler is running
      - Creating and starting the concrete SPI and DIO (chip select) drivers

    @param spiDriver  The (already started) half-duplex SPI driver under test
    @param csDriver   The (already started) chip select digital output driver
 */
void runtests( Kit::Driver::SPI::IHalfDuplex& spiDriver,
               Kit::Driver::Dio::IOutput&     csDriver );


#endif  // end header latch
