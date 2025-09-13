#ifndef KIT_SYSTEM_WIN32_MAPPINGS_X_H_
#define KIT_SYSTEM_WIN32_MAPPINGS_X_H_
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

#ifndef NOMINMAX
#define NOMINMAX  // Prevents min/max macros from windows.h
#endif

#include <windows.h>
#include <time.h>
#include <stdint.h>
#include <stdlib.h>
#include "Kit/System/FatalError.h"

/// PRETTY_FUNCTION macro is non-standard
#if defined( __GNUC__ )
/// Take advantage of GCC's pretty function symbol
#define KIT_SYSTEM_ASSERT_PRETTY_FUNCNAME __PRETTY_FUNCTION__

#elif defined( _MSC_VER )
/// Take advantage of Microsoft's pretty function symbol
#define KIT_SYSTEM_ASSERT_PRETTY_FUNCNAME __FUNCSIG__

#else
/// Use C++11 function name
#define KIT_SYSTEM_ASSERT_PRETTY_FUNCNAME __func__
#endif  // end __PRETTY_FUNCTION__


/// Win32 Mapping
#define KIT_SYSTEM_ASSERT_MAP( e )                                                                                                                   \
    do                                                                                                                                               \
    {                                                                                                                                                \
        if ( !( e ) )                                                                                                                                \
            Kit::System::FatalError::logf( Kit::System::Shutdown::eASSERT, "ASSERT Failed at: file=%s, line=%d, func=%s\n", __FILE__, __LINE__, KIT_SYSTEM_ASSERT_PRETTY_FUNCNAME ); \
    }                                                                                                                                                \
    while ( 0 )

/// Win32 Mapping
#define KitSystemThreadID_T_MAP HANDLE

/// Win32 Mapping
#define KitSystemMutex_T_MAP CRITICAL_SECTION

/// Win32 Mapping
#define KitSystemSema_T_MAP HANDLE


/// Win32 Mapping
inline uint32_t KitSystemElapsedTime_getTimeInMilliseconds_MAP()
{
    return (uint32_t)clock();
}


/// Thread Priorities
#define KIT_SYSTEM_THREAD_PRIORITY_HIGHEST_MAP 15

/// Thread Priorities
#define KIT_SYSTEM_THREAD_PRIORITY_NORMAL_MAP 0

/// Thread Priorities
#define KIT_SYSTEM_THREAD_PRIORITY_LOWEST_MAP -15

/// Thread Priorities
#define KIT_SYSTEM_THREAD_PRIORITY_RAISE_MAP ( +1 )

/// Thread Priorities
#define KIT_SYSTEM_THREAD_PRIORITY_LOWER_MAP ( -1 )


#endif  // end header latch
