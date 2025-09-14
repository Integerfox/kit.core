#ifndef KIT_SYSTEM_PRIVATE_COUNTER_H_
#define KIT_SYSTEM_PRIVATE_COUNTER_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Container/ListItem.h"
#include <stdint.h>

/// 
namespace Kit {
/// 
namespace System {


/** This abstract class defines the call-back interface for a Counter object.
    A Counter object is used to maintain a counter down mechanism for tracking
    the time remaining for an individual timer.

    The Counter interface is NOT inherently thread safe.  
 */
class ICounter : public Kit::Container::ExtendedListItem
{
public:
    /// Notification of the count reaching zero
    virtual void expired() noexcept = 0;

    /// Decrements the counter
    virtual void decrement( uint32_t milliseconds=1 ) noexcept = 0;

    /// Increments the counter
    virtual void increment( uint32_t milliseconds ) noexcept = 0;

    /// Returns the current count (in milliseconds)
    virtual uint32_t count() const noexcept = 0;

public:
    /// Virtual Destructor
    virtual ~ICounter() = default;

};


}       // end namespaces
}
#endif  // end header latch

