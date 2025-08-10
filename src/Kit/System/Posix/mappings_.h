#ifndef KIT_SYSTEM_POSIX_MAPPINGS_H_
#define KIT_SYSTEM_POSIX_MAPPINGS_H_
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


#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <limits.h>
#include "Kit/System/FatalError.h"

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


/// Mapping
#define KIT_SYSTEM_ASSERT_MAP(e)                do { if ( !(e) ) Kit::System::FatalError::logf( "ASSERT Failed at: file=%s, line=%d, func=%s\n", __FILE__, __LINE__, KIT_SYSTEM_ASSERT_PRETTY_FUNCNAME ); } while(0)


/// Mapping
#define Kit_System_Thread_NativeHdl_T_MAP       pthread_t

/// Mapping
#define Kit_System_Mutex_T_MAP                  pthread_mutex_t

/// Mapping
#define Kit_System_Sema_T_MAP                   sem_t

/// Mapping
#define Kit_System_TlsKey_T_MAP                 pthread_key_t

/// Mapping 
#define KIT_SYSTEM_SHELL_NULL_DEVICE_x_MAP      "/dev/null"

/// Mapping 
#define KIT_SYSTEM_SHELL_SUPPORTED_x_MAP        1

/// Mapping
#define KIT_IO_NEW_LINE_NATIVE_MAP              "\n"

/// Mapping
#define KIT_IO_FILE_NATIVE_DIR_SEP_MAP          '/'

/// Mapping
#define KIT_IO_FILE_MAX_NAME_MAP                PATH_MAX


/// Mapping
inline unsigned long KitSystemElapsedTime_getTimeInMilliseconds_MAP()
{
    struct timespec tm;
    clock_gettime( CLOCK_MONOTONIC, &tm );
    return tm.tv_sec * 1000 + tm.tv_nsec / 1000000;
}

//
// Thread Priorities
// Note: POSIX does not define/require specific Priority values, however
//       it does define that the priority range must be at 32 and the
//       a higher numerical value has higher priority... So the values
//       support ONLY a range of 32, with N+1 having a higher priority
//       than N.  At run time, the HIGHEST/LOWEST bounds will be mapped
//       the actual range defined by the functions:
//            sched_get_priority_max(), sched_get_priority_min()
//
/// Mapping
#define KIT_SYSTEM_THREAD_PRIORITY_HIGHEST_MAP      31
/// Mapping
#define KIT_SYSTEM_THREAD_PRIORITY_NORMAL_MAP       15
/// Mapping
#define KIT_SYSTEM_THREAD_PRIORITY_LOWEST_MAP       0
/// Mapping
#define KIT_SYSTEM_THREAD_PRIORITY_RAISE_MAP        (1)
/// Mapping
#define KIT_SYSTEM_THREAD_PRIORITY_LOWER_MAP        (-1)




#endif  // end header latch

