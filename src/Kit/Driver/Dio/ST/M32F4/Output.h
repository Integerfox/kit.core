#ifndef KIT_DRIVER_DIO_ST_M32F4_OUTPUT_H_
#define KIT_DRIVER_DIO_ST_M32F4_OUTPUT_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */


#include "Kit/Driver/Dio/IOutput.h"
#include "Kit/Bsp/Api.h"


///
namespace Kit {
///
namespace Driver {
///
namespace Dio {
///
namespace ST {
///
namespace M32F4 {


/** Concrete digital output driver for the STM32F4 HAL.  Wraps the STM32
    HAL GPIO functions (HAL_GPIO_WritePin, HAL_GPIO_ReadPin,
    HAL_GPIO_TogglePin) into the Kit::Driver::Dio::IOutput interface.

    The driver abstracts pin polarity: when 'activeHigh' is true (default),
    assert() drives the pin HIGH; when false, assert() drives the pin LOW.

    The 'port' and 'pin' must correspond to a GPIO pin configured as a
    push-pull output (e.g., via CubeMX MX_GPIO_Init).
 */
class Output : public IOutput
{
public:
    /** Optional start-up arguments for the start() method.  Provides the
        initial logical state to drive the output signal to when the driver
        is started.
     */
    struct StartArgs_T
    {
        bool istate;  //!< Initial logical state of the output (true=asserted, false=deasserted)

        /// Constructor
        StartArgs_T( bool initialState = false )
            : istate( initialState )
        {
        }
    };


public:
    /** Constructor.

        NOTE: The constructor can NOT set the initial state of the output pin.
              If the driver is statically allocated, the constructor runs
              before main() executes (i.e. the HAL/BSP/SDK/RTOS/etc. has not
              been initialized yet).  The initial output state is set by
              start(); the stopped-state is set by stop().

        @param port              Pointer to the GPIO port (e.g., GPIOD).  Must
                                 remain valid for the lifetime of this object.
        @param pin               GPIO pin mask (e.g., GPIO_PIN_14).
        @param activeHigh        If true, assert() drives pin HIGH.  If false,
                                 assert() drives pin LOW.
        @param stateOnStopCalled Logical state to drive the output signal to
                                 when stop() is called (true=asserted,
                                 false=deasserted).
     */
    Output( GPIO_TypeDef* port, uint16_t pin, bool activeHigh = true, bool stateOnStopCalled = false ) noexcept;


public:
    /// See Kit::Driver::IStart
    bool start( void* startArgs ) noexcept override;

    /// See Kit::Driver::IStop
    void stop() noexcept override;


public:
    /// See Kit::Driver::Dio::IOutput
    void assertPin() noexcept override;

    /// See Kit::Driver::Dio::IOutput
    void deassertPin() noexcept override;

    /// See Kit::Driver::Dio::IOutput
    void toggle() noexcept override;

    /// See Kit::Driver::Dio::IOutput
    void set( bool asserted ) noexcept override;

    /// See Kit::Driver::Dio::IOutput
    bool isAsserted() const noexcept override;


protected:
    GPIO_TypeDef* m_port;               //!< Pointer to the GPIO port
    uint16_t      m_pin;                //!< GPIO pin mask
    bool          m_activeHigh;         //!< True if assert=HIGH, false if assert=LOW
    bool          m_stateOnStopCalled;  //!< Logical state driven to the output when stop() is called
    bool          m_started;            //!< Driver started flag
};


}  // end namespaces
}
}
}
}
#endif  // end header latch
