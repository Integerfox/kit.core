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

#include "WatchedThread.h"

using namespace Kit::System::Watchdog;

/////////////////////////
WatchedThread::WatchedThread(unsigned long wdogTimeoutMs) noexcept
    : currentCountMs(wdogTimeoutMs)
    , wdogTimeoutMs(wdogTimeoutMs)
    , isBeingWatched(false)
{
}