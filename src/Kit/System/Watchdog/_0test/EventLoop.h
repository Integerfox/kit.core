#ifndef KIT_SYSTEM_EVENTLOOP_STUB_H_
#define KIT_SYSTEM_EVENTLOOP_STUB_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file
    Stub implementation of EventLoop for watchdog testing
*/

#include "Kit/System/Watchdog/IWatchedEventLoop.h"
#include <cstdint>

namespace Kit {
namespace System {

/** Stub EventLoop class for watchdog testing - does NOT include the problematic macros */
class EventLoop
{
public:
    EventLoop( IWatchedEventLoop* watchdog = nullptr )
        : m_watchdog( watchdog ) {}
    virtual ~EventLoop() = default;

    virtual void startEventLoop() noexcept
    {
        // Stub implementation - no watchdog macro calls
    }

    virtual void stopEventLoop() noexcept
    {
        // Stub implementation - no watchdog macro calls
    }

    virtual bool waitAndProcessEvents( bool skipWait = false ) noexcept
    {
        // Stub implementation - no watchdog macro calls
        return true;
    }

    virtual int signal() noexcept
    {
        return 0;
    }

private:
    IWatchedEventLoop* m_watchdog;
};

}  // namespace System
}  // namespace Kit

#endif