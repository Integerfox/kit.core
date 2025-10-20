/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "WatchedThread.h"
#include "Supervisor.h"

using namespace Kit::System;

/////////////////////////
WatchedThread::WatchedThread( uint32_t wdogTimeoutMs ) noexcept
    : m_currentCountMs( wdogTimeoutMs )
    , m_wdogTimeoutMs( wdogTimeoutMs )
{
}