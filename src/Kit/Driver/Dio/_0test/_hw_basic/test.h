#ifndef KIT_DRIVER_DIO_0TEST_HW_BASIC_TEST_H_
#define KIT_DRIVER_DIO_0TEST_HW_BASIC_TEST_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file
    Platform-independent hardware test for the Kit::Driver::Dio digital output
    driver.  A single test that can be re-used for all concrete Dio drivers.
*/


#include "Kit/Driver/Dio/IOutput.h"


/** Runs the digital output hardware test.  This method does NOT return.

    The caller is responsible for:
      - Creating the thread that this method executes in
      - Ensuring that the OSAL scheduler is running
      - Creating and starting the concrete DIO output driver

    @param output  The (already started) digital output driver under test
 */
void runtests( Kit::Driver::Dio::IOutput& output );


#endif  // end header latch
