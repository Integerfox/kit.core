/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "kit_config.h"
#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include "catch2/catch_test_macros.hpp"
#include "Kit/System/Shell.h"
#include "Kit/System/Trace.h"
#include <inttypes.h>

/** NOTE: The test requires that the application define the following
          symbols in colony_config.h for the test to work.  The values
          of the symbols are based on the actual native OS.
 */
#ifndef MY_DIR_COMMAND
#define MY_DIR_COMMAND "dir"
#endif
#ifndef MY_BAD_COMMAND
#define MY_BAD_COMMAND "not-a-valid-command"
#endif

#define SECT_ "_0test"
///
using namespace Kit::System;

#define ANSWER_IS_AVAILABLE              true
#define ANSWER_EXECUTE_CMD               0
#define ANSWER_EXECUTE_NOT_BAD_CMD       0

#define NOT_SUPPORTED_ANSWER_EXECUTE_CMD -1

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "shell" )
{
    KIT_SYSTEM_TRACE_FUNC( SECT_ );
    ShutdownUnitTesting::clearAndUseCounter();

    if ( Shell::isAvailable() )
    {
        REQUIRE( Shell::execute( MY_DIR_COMMAND ) == ANSWER_EXECUTE_CMD );
        REQUIRE( Shell::execute( MY_DIR_COMMAND, false ) == ANSWER_EXECUTE_CMD );
        REQUIRE( Shell::execute( MY_DIR_COMMAND, false, false ) == ANSWER_EXECUTE_CMD );
        REQUIRE( Shell::execute( MY_DIR_COMMAND, true, false ) == ANSWER_EXECUTE_CMD );

        REQUIRE( Shell::execute( MY_BAD_COMMAND ) != ANSWER_EXECUTE_NOT_BAD_CMD );
        REQUIRE( Shell::execute( MY_BAD_COMMAND, false ) != ANSWER_EXECUTE_NOT_BAD_CMD );
        REQUIRE( Shell::execute( MY_BAD_COMMAND, false, false ) != ANSWER_EXECUTE_NOT_BAD_CMD );
        REQUIRE( Shell::execute( MY_BAD_COMMAND, true, false ) != ANSWER_EXECUTE_NOT_BAD_CMD );
    }
    else
    {
        REQUIRE( Shell::execute( MY_DIR_COMMAND ) == NOT_SUPPORTED_ANSWER_EXECUTE_CMD );
    }

    REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
}
