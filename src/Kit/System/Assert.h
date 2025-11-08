#ifndef KIT_SYSTEM_ASSERT_H_
#define KIT_SYSTEM_ASSERT_H_
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
#include "kit_map.h"

#ifdef USE_KIT_SYSTEM_ASSERT

/** This macro is platform/compiler independent replacement for 'assert()'
    function/macro.  If the passed boolean expression is false (i.e. zero)
    then the application is terminated.  The specific actions that occur when
    the application is terminated is platform specific.  The 'signature' for
    the macro/function is:

    \b Prototype: void KIT_SYSTEM_ASSERT( <boolean expression> );

    Note: This assert macro must be "turned on" at compile time by defining
          the USE_KIT_SYSTEM_ASSERT_MACROS symbol.
 */
#define KIT_SYSTEM_ASSERT                   KIT_SYSTEM_ASSERT_MAP

#else
    
/// Empty macro
#define KIT_SYSTEM_ASSERT(e)

#endif  // end USE_KIT_SYSTEM_ASSERT_MACROS


#endif  // end header latch

