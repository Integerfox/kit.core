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
#define KitSystemThreadID_T_MAP       TaskHandle_t  

/// Mapping
#define KitSystemMutex_T_MAP                  SemaphoreHandle_t

/// Mapping
#define KitSystemSema_T_MAP                   SemaphoreHandle_t



/// Mapping
inline unsigned long KitSystemElapsedTime_getTimeInMilliseconds_MAP()
{
    return xTaskGetTickCount() / portTICK_PERIOD_MS;
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

