/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */


#include "Kit/Driver/Dio/ST/M32F4/Output.h"

///
using namespace Kit::Driver::Dio::ST::M32F4;


//////////////////////////////////////////////////////////////////////////////
Output::Output( GPIO_TypeDef* port, uint16_t pin ) noexcept
    : m_port( port )
    , m_pin( pin )
{
}


//////////////////////////////////////////////////////////////////////////////
void Output::setHigh() noexcept
{
    HAL_GPIO_WritePin( m_port, m_pin, GPIO_PIN_SET );
}


void Output::setLow() noexcept
{
    HAL_GPIO_WritePin( m_port, m_pin, GPIO_PIN_RESET );
}


void Output::toggle() noexcept
{
    HAL_GPIO_TogglePin( m_port, m_pin );
}


void Output::set( bool newState ) noexcept
{
    HAL_GPIO_WritePin( m_port, m_pin, newState ? GPIO_PIN_SET : GPIO_PIN_RESET );
}


bool Output::getState() const noexcept
{
    return ( HAL_GPIO_ReadPin( m_port, m_pin ) == GPIO_PIN_SET );
}
