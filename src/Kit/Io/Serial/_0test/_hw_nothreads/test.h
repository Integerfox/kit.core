#ifndef TEST_HW_NOTHREADS_H
#define TEST_HW_NOTHREADS_H
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file 

This file tests the Kit::Io Serial stream implementation that does not rely on
the KIT threading model.  It uses the platform's standard I/O library - typically
this means busy-wait semantics.

*/

#include "Kit/Io/IInput.h"
#include "Kit/Io/IOutput.h"


/** The KIT Threading model/OSAL is NOT used.  The application MUST supply this
    function. Returns the current elapsed time in Milliseconds
 */
uint32_t elapsedTimeMs();

/// Entry point to the test.
void test( Kit::Io::IInput& infd, Kit::Io::IOutput& outfd );

#endif  // end header latch