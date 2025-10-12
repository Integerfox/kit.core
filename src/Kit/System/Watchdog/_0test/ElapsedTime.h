#ifndef KIT_SYSTEM_ELAPSEDTIME_H_
#define KIT_SYSTEM_ELAPSEDTIME_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file
    Stub implementation of ElapsedTime for watchdog testing
*/

#include <cstdint>
#include <chrono>

namespace Kit {
namespace System {

/** Stub ElapsedTime class for watchdog testing */
class ElapsedTime
{
public:
    ElapsedTime()
        : m_start( std::chrono::steady_clock::now() ) {}

    uint32_t deltaInMilliseconds() const
    {
        auto now      = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>( now - m_start );
        return static_cast<uint32_t>( duration.count() );
    }

    void reset()
    {
        m_start = std::chrono::steady_clock::now();
    }

private:
    std::chrono::steady_clock::time_point m_start;
};

}  // namespace System
}  // namespace Kit

#endif