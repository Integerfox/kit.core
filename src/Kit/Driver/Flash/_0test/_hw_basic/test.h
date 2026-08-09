#ifndef KIT_DRIVER_FLASH_0TEST_HW_BASIC_TEST_H_
#define KIT_DRIVER_FLASH_0TEST_HW_BASIC_TEST_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file
    Platform-independent hardware test for the Kit::Driver::Flash driver.  A
    single test that can be re-used for all concrete Flash drivers (e.g. the
    W25Q SPI NOR family).
*/


#include "Kit/Driver/Flash/IApi.h"


/** Runs the Flash hardware test.  This method does NOT return.

    WARNING: This test is DESTRUCTIVE.  It erases and re-writes the first few
             sectors of the flash device.  Do NOT run it against a device that
             holds data you wish to keep.

    The caller is responsible for:
      - Creating the thread that this method executes in
      - Ensuring that the OSAL scheduler is running
      - Creating and starting the concrete Flash driver

    @param flash  The (already started) Flash driver under test
 */
void runtests( Kit::Driver::Flash::IApi& flash );


#endif  // end header latch
