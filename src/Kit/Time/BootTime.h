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
#include "Kit/Dm/Mp/Uint16.h"

// The following are NOT used directly - but are present in case the application changes KitTimeBootCount_T type 
#include "Kit/Dm/Mp/Uint8.h"
#include "Kit/Dm/Mp/Uint32.h"

/** This macro defines the integer type (aka the size of) of the boot counter.
    The default is uint16_t which allows for 65536 boots before the counter rolls
    over.  If your application has very frequent reboots, you may want to
    increase the size of the boot counter type.  However, the more bits allocated
    to the boot counter - less bits are available for elapsed milliseconds.

    NOTE: The application is REQUIRED to change KitTimeMpBootCount to match the type
          of the 'overridden' boot counter type.

 */
#ifndef KitTimeBootCount_T
#define KitTimeBootCount_T uint16_t
#endif

/** The model point type large enough to hold the boot counter type.  See 
    comments for `KitTimeBootCount_T` for additional details
 */
#ifndef KitTimeMpBootCount
#define KitTimeMpBootCount Kit::Dm::Mp::Uint16
#endif

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
          used for the elapsed time portion of the boot time.
 */
uint64_t getBootTime() noexcept;

/// This method breaks 'boot time' into itself individual fields
void parseBootTime( uint64_t            srcBootTime,
                    KitTimeBootCount_T& dstBootCounter,
                    uint64_t&           dstElapsedTimeMs ) noexcept;

/// This method is used to manually construct a boot time value
uint64_t constructBootTime( KitTimeBootCount_T srcBootCounter, uint64_t srcElapsedTimeMs ) noexcept;

/** This method is used to initialize boot time logic.  It MUST be called
    BEFORE the first call to getBootTime().

    'bootCounterMp' is the Model Point instance that contains current boot count
 */
void initializeBootTime( KitTimeMpBootCount& bootCounterMp ) noexcept;

}  // end namespaces
}
#endif  // end header latch