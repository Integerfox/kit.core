#ifndef KIT_SYSTEM_THREAD_H_
#define KIT_SYSTEM_THREAD_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file
    Stub implementation of Thread for watchdog testing
*/

#include <cstdint>

namespace Kit {
namespace System {

/** Stub Thread class for watchdog testing */
class Thread
{
public:
    static void sleepInMilliseconds( uint32_t milliseconds )
    {
        // Simple stub - in real testing this would call appropriate sleep function
        // For now, just busy wait for simulation
        auto start = std::chrono::steady_clock::now();
        while ( std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::steady_clock::now() - start )
                    .count() < milliseconds )
        {
            // busy wait
        }
    }
};

}  // namespace System
}  // namespace Kit

#include <chrono>
#endif