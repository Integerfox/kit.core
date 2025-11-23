#ifndef KIT_TIME_BOOTTIME_H_
#define KIT_TIME_BOOTTIME_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "kit_config.h"
#include <stdint.h>
// #include "Kit/Dm/Mp/Uint32.h"

///
namespace Kit {
///
namespace Time {

/** This method returns 'Boot Time' which include the current boot counter
    and elapsed time - in milliseconds - since the MCU was reset.

    The method retuns boot time as 'flatten' uint64_t integer where the
    upper N bytes are the boot counter, the lower M bytes are elapsed MCU time
    since reset.

    NOTE: The default implementation uses the Kit::System::ElapsedTime interface
          as the source for elapsed milliseconds.  The implementation can be
          changed/overridden by defining USE_KIT_TIME_BOOTTIME_WITH_ABSOLUTETIME
          in the kit_config.h file.  In this case, the AbsoluteTime interface is
          used for the elapsed portion of the boot time.
 */
uint64_t getBootTime() noexcept;

/// This method breaks 'boot time' into itself individual fields
void parseBootTime( uint64_t  srcBootTime,
                    uint16_t& dstBootCounter,
                    uint64_t& dstElapsedTimeMs ) noexcept;

/// This method is used to manually construct a boot time value
uint64_t constructBootTime( uint16_t srcBootCounter, uint64_t srcElapsedTimeMs ) noexcept;

// TODO: Need Model Point support the persistent storage of the Boot Counter
// /** This method is used to initialize boot time logic.  It MUST be called
//     BEFORE the first call to getBootTime().

//     'bootCounterMp' is the Model Point instance that contains current boot count
//  */
// void initializeBootTime( Kit::Dm::Mp::Uint32& bootCounterMp ) noexcept;

}  // end namespaces
}
#endif  // end header latch