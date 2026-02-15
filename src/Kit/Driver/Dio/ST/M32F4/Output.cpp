/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Output.h"

///
using namespace Kit::Driver::Dio::ST::M32F4;


/////////////////////////////////////////////////////////////////////////////
Output::Output( GPIO_TypeDef* port,
                uint16_t      pin,
                bool          activeHigh ) noexcept
    : m_port( port )
    , m_pin( pin )
    , m_activeHigh( activeHigh )
{
}


/////////////////////////////////////////////////////////////////////////////
void Output::setHigh() noexcept
{
    if ( m_activeHigh )
    {
        HAL_GPIO_WritePin( m_port, m_pin, GPIO_PIN_SET );
    }
    else
    {
        HAL_GPIO_WritePin( m_port, m_pin, GPIO_PIN_RESET );
    }
}


void Output::setLow() noexcept
{
    if ( m_activeHigh )
    {
        HAL_GPIO_WritePin( m_port, m_pin, GPIO_PIN_RESET );
    }
    else
    {
        HAL_GPIO_WritePin( m_port, m_pin, GPIO_PIN_SET );
    }
}


void Output::toggle() noexcept
{
    HAL_GPIO_TogglePin( m_port, m_pin );
}


void Output::set( bool high ) noexcept
{
    if ( high )
    {
        setHigh();
    }
    else
    {
        setLow();
    }
}


bool Output::getState() const noexcept
{
    GPIO_PinState state = HAL_GPIO_ReadPin( m_port, m_pin );

    if ( m_activeHigh )
    {
        return ( state == GPIO_PIN_SET );
    }
    else
    {
        return ( state == GPIO_PIN_RESET );
    }
}
