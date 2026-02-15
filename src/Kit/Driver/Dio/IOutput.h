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


/** This class defines the interface for a platform independent Digital Output
    driver. The interface provides basic operations to control a digital output
    pin (GPIO configured as output).

    Typical use cases include:
    - SPI chip select (CS) control
    - LED control
    - Enable/disable signals for peripherals

    The interface itself is NOT thread safe. It is the responsibility of
    the users/clients of the driver to handle any threading issues.

    NOTE: The interface does NOT include start/stop methods as GPIO pins are
          typically configured once during system initialization and remain
          active throughout the application lifetime.
 */
class IOutput
{
public:
    /** This method sets the output pin to the logic high state.
     */
    virtual void setHigh() noexcept = 0;

    /** This method sets the output pin to the logic low state.
     */
    virtual void setLow() noexcept = 0;

    /** This method toggles the output pin state.
     */
    virtual void toggle() noexcept = 0;

    /** This method sets the output pin to the specified state.

        @param high  If true, sets the pin high; if false, sets the pin low.
     */
    virtual void set( bool high ) noexcept = 0;

    /** This method returns the current state of the output pin.

        @return true if the pin is high, false if the pin is low.
     */
    virtual bool getState() const noexcept = 0;


public:
    /// Virtual destructor
    virtual ~IOutput() noexcept = default;
};


};  // end namespaces
};
};
#endif  // end header latch
