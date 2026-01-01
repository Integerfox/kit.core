#ifndef KIT_SYSTEM_RPPICO_MAPPINGS_H_
#define KIT_SYSTEM_RPPICO_MAPPINGS_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file 

    This file provides the mappings for Platform specific types, etc. needed
    by Kit::System

 */


#include <stdint.h>
#include "Kit/System/RPPico/mutex_t_.h"
#include "Kit/System/RPPico/semaphore_t_.h"
#include "Kit/System/_assert/c_assert.h"
#include "pico/time.h"

/// PRETTY_FUNCTION macro is non-standard
#if defined(__GNUC__)
/// Take advantage of GCC's pretty function symbol
#define KIT_SYSTEM_ASSERT_PRETTY_FUNCNAME    __PRETTY_FUNCTION__

#elif defined(_MSC_VER)
/// Take advantage of Microsoft's pretty function symbol
#define KIT_SYSTEM_ASSERT_PRETTY_FUNCNAME    __FUNCSIG__

#else
/// Use C++11 function name
#define KIT_SYSTEM_ASSERT_PRETTY_FUNCNAME    __func__
#endif  // end __PRETTY_FUNCTION__

/// Number of cores on the RPPico MCU
#define KIT_SYSTEM_RPPICO_NUM_CORES                 2


/// RPPico Mapping
#define KIT_SYSTEM_ASSERT_MAP(e)                    Kit_System_assert_c_wrapper( e, __FILE__, __LINE__, KIT_SYSTEM_ASSERT_PRETTY_FUNCNAME )

/// Mapping
#define KitSystemThreadID_T_MAP                     unsigned  

/// Mapping
#define KitSystemMutex_T_MAP                        Kit::System::RPPico::Mutex_T

/// Mapping
#define KitSystemSema_T_MAP                         Kit::System::RPPico::Semaphore_T



/// Mapping
inline unsigned long KitSystemElapsedTime_getTimeInMilliseconds_MAP()
{
    return to_ms_since_boot( get_absolute_time() );
}


/// Thread Priorities (has no meaning since each thread is one-to-one with a core)
#define KIT_SYSTEM_THREAD_PRIORITY_HIGHEST_MAP      0

/// Thread Priorities (has no meaning since each thread is one-to-one with a core)
#define KIT_SYSTEM_THREAD_PRIORITY_NORMAL_MAP       0

/// Thread Priorities (has no meaning since each thread is one-to-one with a core)
#define KIT_SYSTEM_THREAD_PRIORITY_LOWEST_MAP       0

/// Thread Priorities (has no meaning since each thread is one-to-one with a core) 
#define KIT_SYSTEM_THREAD_PRIORITY_RAISE_MAP        0

/// Thread Priorities (has no meaning since each thread is one-to-one with a core)
#define KIT_SYSTEM_THREAD_PRIORITY_LOWER_MAP        0



#endif  // end header latch
