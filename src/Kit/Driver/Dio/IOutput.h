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


///
namespace Kit {
///
namespace Driver {
///
namespace Dio {


/** This class defines the interface for a digital output pin.  The interface
    provides methods to set the pin high, low, toggle it, and query its
    current state.

    The interface itself is NOT thread safe. It is the responsibility of
    the users/clients of the driver to handle any threading issues.
 */
class IOutput
{
public:
    /// Drives the output pin HIGH
    virtual void setHigh() noexcept = 0;

    /// Drives the output pin LOW
    virtual void setLow() noexcept = 0;

    /// Toggles (inverts) the current output state
    virtual void toggle() noexcept = 0;

    /// Sets the output pin to the specified state (true=HIGH, false=LOW)
    virtual void set( bool newState ) noexcept = 0;

    /// Returns the current output state (true=HIGH, false=LOW)
    virtual bool getState() const noexcept = 0;


public:
    /// Virtual destructor
    virtual ~IOutput() noexcept = default;
};


}  // end namespaces
}
}
#endif  // end header latch
