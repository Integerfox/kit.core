#ifndef Kit_System_Watchdog_Macros_h_
#define Kit_System_Watchdog_Macros_h_
/*-----------------------------------------------------------------------------
* This file is part of the KIT C++ Class Library.  The KIT C++ Class Library
* is an open source project with a BSD type of licensing agreement.  See the 
* license agreement (license.txt) in the top/ directory or on the Internet at
* https://integerfox.com/kit/license.txt
*
* Copyright (c) 2025  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file */

/** Compile-time macros for instrumenting EventLoop with watchdog monitoring.
    These macros can be enabled/disabled at compile time and support both
    direct function calls and pointer-based calls for flexibility.
 */

#ifdef USE_KIT_SYSTEM_WATCHDOG

// Include the watchdog API header
#include "Kit/System/Watchdog/WatchedEventLoopApi.h"

/** Macro to start watchdog monitoring in an EventLoop.
    Usage: KIT_WDOG_START_EVENTLOOP(watcherInstance, eventLoopInstance)

    @param watcher The WatchedEventLoopApi instance or pointer to use
    @param eventLoop The EventLoop instance that provides the timing source
 */
#define KIT_WDOG_START_EVENTLOOP(watcher, eventLoop) \
    do { \
        if (&(watcher)) { \
            (watcher).startWatcher(eventLoop); \
        } \
    } while(0)

/** Macro to stop watchdog monitoring in an EventLoop.
    Usage: KIT_WDOG_STOP_EVENTLOOP(watcherInstance)

    @param watcher The WatchedEventLoopApi instance or pointer to use
 */
#define KIT_WDOG_STOP_EVENTLOOP(watcher) \
    do { \
        if (&(watcher)) { \
            (watcher).stopWatcher(); \
        } \
    } while(0)

/** Macro to perform watchdog monitoring in an EventLoop.
    Usage: KIT_WDOG_EVENTLOOP_MONITOR(watcherInstance)

    @param watcher The WatchedEventLoopApi instance or pointer to use
 */
#define KIT_WDOG_EVENTLOOP_MONITOR(watcher) \
    do { \
        if (&(watcher)) { \
            (watcher).monitorWdog(); \
        } \
    } while(0)

#else

// When watchdog is disabled, macros expand to nothing
#define KIT_WDOG_START_EVENTLOOP(watcher, eventLoop)    do { } while(0)
#define KIT_WDOG_STOP_EVENTLOOP(watcher)                do { } while(0)
#define KIT_WDOG_EVENTLOOP_MONITOR(watcher)             do { } while(0)

#endif  // USE_KIT_SYSTEM_WATCHDOG

#endif  // end header latch