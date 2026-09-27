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
    Hardware-in-loop runner for the Kit::Driver::Flash test suite.  Wraps the
    platform-independent runtests() function and loops forever on completion
    to provide a visual heartbeat via the debug LED.
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

    NOTE: If the test passes, the debug LED blinks slowly (500ms on/off).  If 
          any check fails, the debug LED blinks quickly (100ms on/off).
    */
void runHwTests( Kit::Driver::Flash::IApi& flash );


#endif  // end header latch
