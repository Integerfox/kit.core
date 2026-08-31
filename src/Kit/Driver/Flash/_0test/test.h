#ifndef KIT_DRIVER_FLASH_0TEST_TEST_H_
#define KIT_DRIVER_FLASH_0TEST_TEST_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file
    Platform-independent test suite for the Kit::Driver::Flash driver.  Can be
    driven by an automated unit test (via a mock flash) or wrapped by a
    hardware-in-loop runner (see _hw_basic/test.h).
*/


#include "Kit/Driver/Flash/IApi.h"


/** Runs the Flash driver test suite against the provided driver instance.
    Returns true if all checks passed, false if any check failed.

    WARNING: This test is DESTRUCTIVE.  It erases and re-writes the first few
             sectors of the flash device.  Do NOT run it against a device that
             holds data you wish to keep.

    The caller is responsible for creating and starting the concrete Flash
    driver before calling this function.

    @param flash  The (already started) Flash driver under test.
    @return true if all tests passed; false if one or more tests failed.
 */
bool runtests( Kit::Driver::Flash::IApi& flash );


#endif  // end header latch
