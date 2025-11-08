#ifndef KIT_SYSTEM_C_ASSERT_H_
#define KIT_SYSTEM_C_ASSERT_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif


/** These method is 'C wrapper' that calls Kit::System::FatalError handler when
    the assert fails.  The C Wrapper is need to as not to pollute the Platform
    mapping header files with C++ headers that cause compile issues when the
    underlying Platform is C based RTOS (e.g. FreeRTOS)
 
 */
void Kit_System_assert_c_wrapper( bool true_expression, const char* file, unsigned line, const char* func );

#ifdef __cplusplus
}
#endif

#endif  // end header latch

