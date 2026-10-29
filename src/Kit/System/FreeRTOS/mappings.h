#ifndef KIT_SYSTEM_FREERTOS_MAPPINGS_H_
#define KIT_SYSTEM_FREERTOS_MAPPINGS_H_
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


#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "Kit/System/_assert/c_assert.h"
#include <stdint.h>

// PRETTY_FUNCTION macro is non-standard
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


/// Mapping
#define KIT_SYSTEM_ASSERT_MAP(e)                Kit_System_assert_c_wrapper( e, __FILE__, __LINE__, KIT_SYSTEM_ASSERT_PRETTY_FUNCNAME )

/// Mapping
#define KitSystemThreadID_T_MAP                 TaskHandle_t  

/// Mapping
#define KitSystemMutex_T_MAP                    SemaphoreHandle_t

/// Mapping
#define KitSystemSema_T_MAP                     SemaphoreHandle_t

/// Mapping
#define KitSystemBSema_T_MAP                    SemaphoreHandle_t



/** Converts RTOS ticks to milliseconds.

    NOTE: Deliberately does NOT use portTICK_PERIOD_MS.  That macro is an integer
          division (1000/configTICK_RATE_HZ) and is therefore lossy - or zero -
          for any tick rate that does not divide evenly into 1000.
 */
inline uint32_t KitSystemFreeRTOS_ticksToMsec( TickType_t ticks )
{
    return (uint32_t)( ( (uint64_t)ticks * 1000U ) / configTICK_RATE_HZ );
}

/** Converts milliseconds to RTOS ticks.

    Replaces pdMS_TO_TICKS() because that macro:
      o Truncates toward zero, so any delay shorter than one tick becomes
        vTaskDelay(0) - which neither blocks NOR yields.  Rounding UP guarantees
        a non-zero delay always yields for at least one tick.
      o Does its multiply in TickType_t, so it wraps for large millisecond values.
 */
inline TickType_t KitSystemFreeRTOS_msecToTicks( uint32_t msec )
{
    return (TickType_t)( ( ( (uint64_t)msec * configTICK_RATE_HZ ) + 999U ) / 1000U );
}

/// Mapping
inline uint32_t KitSystemElapsedTime_getTimeInMilliseconds_MAP()
{
    // The 32 bit roll-over is by design -->ElapsedTime::deltaMilliseconds() relies on it
    return KitSystemFreeRTOS_ticksToMsec( xTaskGetTickCount() );
}


//
// Thread Priorities
//    The maximum number of priority levels is defined FreeRTOSConfig.h
//    The highest priority is 'configMAX_PRIORITIES - 1', the lowest
//    priority is 1.
//
/// Mapping
#define KIT_SYSTEM_THREAD_PRIORITY_HIGHEST_MAP      (configMAX_PRIORITIES - 1)
/// Mapping
#define KIT_SYSTEM_THREAD_PRIORITY_NORMAL_MAP       (configMAX_PRIORITIES/2)
/// Mapping
#define KIT_SYSTEM_THREAD_PRIORITY_LOWEST_MAP       1
/// Mapping
#define KIT_SYSTEM_THREAD_PRIORITY_RAISE_MAP        (1)
/// Mapping
#define KIT_SYSTEM_THREAD_PRIORITY_LOWER_MAP        (-1)




#endif  // end header latch

