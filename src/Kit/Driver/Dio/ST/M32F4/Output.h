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


#include "kit_config.h"
#include "Kit/Bsp/Api.h"
#include "Kit/Driver/Dio/IOutput.h"

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


/** This concrete class implements the Digital Output interface for STM32F4xx
    microcontrollers using the STM32 HAL library.

    ## Prerequisites

    The GPIO pin must be initialized before use. This is typically done via
    STM32CubeMX generated code (MX_GPIO_Init) or manually:
    - Enable GPIO peripheral clock
    - Configure pin as output (GPIO_MODE_OUTPUT_PP or GPIO_MODE_OUTPUT_OD)
    - Set initial output level

    ## Active Low vs Active High

    The driver can be configured for active-low signals (like SPI chip select)
    by using the `activeHigh` constructor parameter. When activeHigh is false:
    - setHigh() will output a logic LOW on the pin (assert)
    - setLow() will output a logic HIGH on the pin (deassert)
 */
class Output : public Kit::Driver::Dio::IOutput
{
public:
    /** Constructor.

        @param port         GPIO port (e.g., GPIOA, GPIOB)
        @param pin          GPIO pin mask (e.g., GPIO_PIN_4)
        @param activeHigh   If true (default), high means active.
                            If false, logic is inverted (for active-low signals)
     */
    Output( GPIO_TypeDef* port,
            uint16_t      pin,
            bool          activeHigh = true ) noexcept;


public:
    /// See Kit::Driver::Dio::IOutput
    void setHigh() noexcept override;

    /// See Kit::Driver::Dio::IOutput
    void setLow() noexcept override;

    /// See Kit::Driver::Dio::IOutput
    void toggle() noexcept override;

    /// See Kit::Driver::Dio::IOutput
    void set( bool high ) noexcept override;

    /// See Kit::Driver::Dio::IOutput
    bool getState() const noexcept override;


protected:
    /// GPIO port
    GPIO_TypeDef* m_port;

    /// GPIO pin mask
    uint16_t m_pin;

    /// Active high flag
    bool m_activeHigh;


private:
    /// Prevent access to the copy constructor
    Output( const Output& ) = delete;

    /// Prevent access to the assignment operator
    const Output& operator=( const Output& ) = delete;
};


};  // end namespaces
};
};
};
};
#endif  // end header latch
