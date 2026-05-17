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
/** @file
    Concrete digital output driver for the STM32F4 HAL.  Wraps the STM32
    HAL GPIO functions (HAL_GPIO_WritePin, HAL_GPIO_ReadPin,
    HAL_GPIO_TogglePin) into the Kit::Driver::Dio::IOutput interface.
*/


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


/** This class implements the Dio::IOutput interface for a single GPIO pin
    on an STM32F4 microcontroller using the STM32 HAL.

    The 'port' and 'pin' must correspond to a GPIO pin configured as a
    push-pull output (e.g., via CubeMX MX_GPIO_Init).
 */
class Output : public IOutput
{
public:
    /** Constructor.

        @param port  Pointer to the GPIO port (e.g., GPIOD).  Must remain
                     valid for the lifetime of this object.
        @param pin   GPIO pin mask (e.g., GPIO_PIN_14).
     */
    Output( GPIO_TypeDef* port, uint16_t pin ) noexcept;


public:
    /// See Kit::Driver::Dio::IOutput
    void setHigh() noexcept override;

    /// See Kit::Driver::Dio::IOutput
    void setLow() noexcept override;

    /// See Kit::Driver::Dio::IOutput
    void toggle() noexcept override;

    /// See Kit::Driver::Dio::IOutput
    void set( bool newState ) noexcept override;

    /// See Kit::Driver::Dio::IOutput
    bool getState() const noexcept override;


protected:
    GPIO_TypeDef* m_port;  //!< Pointer to the GPIO port
    uint16_t      m_pin;   //!< GPIO pin mask
};


}  // end namespaces
}
}
}
}
#endif  // end header latch
