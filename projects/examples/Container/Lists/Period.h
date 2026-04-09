#ifndef CONTAINER_LISTS_PERIOD_H_
#define CONTAINER_LISTS_PERIOD_H_
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
#include "Kit/Container/DList.h"
#include "Kit/Bsp/Api.h"
#include "Kit/System/Api.h"
#include <stdint.h>

///
namespace Container {
///
namespace Lists {


/// This abstract class defines the interface for flash period.  A series of periods make up a flash pattern
class Period : public Kit::Container::ExtendedListItem
{
public:
    /// Returns the On/Off state of the LED for this period (true=On, false=Off)
    virtual bool getLEDOnOffState() const noexcept = 0;

    /// Returns the duration of this period in milliseconds
    virtual uint32_t getDurationMs() const noexcept = 0;
public:
    /// Virtual destructor
    virtual ~Period() = default;
};

/// This concrete class implements an ON period
class OnPeriod : public Period
{
public:
    /// Constructor
    OnPeriod( uint32_t durationMs )
        : m_durationMs( durationMs ) {}

    /// See Period
    bool getLEDOnOffState() const noexcept override { return true; }

    /// See Period
    uint32_t getDurationMs() const noexcept override { return m_durationMs; }

protected:
    /// Duration of this period in milliseconds
    uint32_t m_durationMs;
};

/// This concrete class implements an OFF period
class OffPeriod : public Period
{
public:
    /// Constructor
    OffPeriod( uint32_t durationMs )
        : m_durationMs( durationMs ) {}


    /// See Period
    bool getLEDOnOffState() const noexcept override { return false; }

    /// See Period
    uint32_t getDurationMs() const noexcept override { return m_durationMs; }
    
protected:
    /// Duration of this period in milliseconds
    uint32_t m_durationMs;
};


}  // end namespaces
}
#endif  // end header latch
