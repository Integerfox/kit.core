#ifndef KIT_DRIVER_DIO_IOUTPUT_H_
#define KIT_DRIVER_DIO_IOUTPUT_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */


#include "Kit/Driver/IStart.h"
#include "Kit/Driver/IStop.h"

///
namespace Kit {
///
namespace Driver {
///
namespace Dio {


/** This class defines the interface for a digital output pin.  The interface
    operates on the LOGICAL state of the output, abstracting away the physical
    polarity (active-high vs active-low).  When the output is 'asserted', the
    pin is in its logically active state regardless of the underlying electrical
    polarity.

    The interface itself is NOT thread safe. It is the responsibility of
    the users/clients of the driver to handle any threading issues.
 */
class IOutput : public virtual Kit::Driver::IStart,
                public virtual Kit::Driver::IStop
{
public:
    /// Asserts (logically activates) the output
    virtual void assertPin() noexcept = 0;

    /// Deasserts (logically deactivates) the output
    virtual void deassertPin() noexcept = 0;

    /// Toggles (inverts) the current logical output state
    virtual void toggle() noexcept = 0;

    /// Sets the logical output state (true=asserted, false=deasserted)
    virtual void set( bool asserted ) noexcept = 0;

    /// Returns the current logical output state (true=asserted, false=deasserted)
    virtual bool isAsserted() const noexcept = 0;


public:
    /// Virtual destructor
    virtual ~IOutput() noexcept = default;
};


}  // end namespaces
}
}
#endif  // end header latch
