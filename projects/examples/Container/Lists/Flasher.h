#ifndef CONTAINER_LISTS_FLASHER_H_
#define CONTAINER_LISTS_FLASHER_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Container/DList.h"
#include "Kit/Bsp/Api.h"
#include "Kit/System/Api.h"
#include "Kit/System/Trace.h"
#include "Container/Lists/Period.h"
#include <stdint.h>
#include <inttypes.h>

///
namespace Container {
///
namespace Lists {

/** This concrete class flashes an LED based on a series of off/on cycle times
    contained in linked list.

    This class is not thread safe.
 */
class Flasher
{
public:
    /// Constructor
    Flasher()
    {
        Bsp_turn_off_debug1();
    }

    /** Method to run a flash pattern.  The pattern is defined by a list of
        Period objects that specify the on/off state and duration of each period
        in the pattern.
    */
    void runPattern( Kit::Container::DList<Period>& pattern ) noexcept
    {
        // Print out the flash pattern. The list content is NOT changed
        KIT_SYSTEM_TRACE_MSG( "main", "Flasher::runPattern()" );
        uint32_t totalDurationMs = 0;
        Period*  item            = pattern.first();  // Get the first item in the pattern list without modifying the list
        while ( item )
        {
            KIT_SYSTEM_TRACE_MSG( "main", "  %s period: %" PRIu32 " ms", item->getLEDOnOffState() ? "ON " : "OFF", item->getDurationMs() );
            totalDurationMs += item->getDurationMs();
            // Skip to the next item in the pattern list without modifying the list
            item = pattern.next( *item );
        }
        KIT_SYSTEM_TRACE_MSG( "main", "Total duration: %" PRIu32 " ms", totalDurationMs );

        // Execute the flash pattern.
        // NOTE: The list is empty as the items are removed from the list as they are executed.
        item = pattern.get();  // Get the first item in the pattern list (the item IS removed from list)
        while ( item )
        {
            if ( item->getLEDOnOffState() )
            {
                Bsp_turn_on_debug1();
            }
            else
            {
                Bsp_turn_off_debug1();
            }
            Kit::System::sleep( item->getDurationMs() );

            // Get the next item in the pattern list (the item IS removed from list)
            item = pattern.get();
        }

        // Once the pattern is done, make sure the LED is off
        Bsp_turn_off_debug1();
    }
};


}  // end namespaces
}
#endif  // end header latch
