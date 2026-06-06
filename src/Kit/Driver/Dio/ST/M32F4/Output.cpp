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
Output::Output( GPIO_TypeDef* port, uint16_t pin, bool activeHigh ) noexcept
    : m_port( port )
    , m_pin( pin )
    , m_activeHigh( activeHigh )
    , m_started( false )
{
}


//////////////////////////////////////////////////////////////////////////////
bool Output::start( void* startArgs ) noexcept
{
    m_started = true;
    return true;
}


void Output::stop() noexcept
{
    m_started = false;
}


//////////////////////////////////////////////////////////////////////////////
void Output::assertPin() noexcept
{
    HAL_GPIO_WritePin( m_port, m_pin, m_activeHigh ? GPIO_PIN_SET : GPIO_PIN_RESET );
}


void Output::deassertPin() noexcept
{
    HAL_GPIO_WritePin( m_port, m_pin, m_activeHigh ? GPIO_PIN_RESET : GPIO_PIN_SET );
}


void Output::toggle() noexcept
{
    HAL_GPIO_TogglePin( m_port, m_pin );
}


void Output::set( bool asserted ) noexcept
{
    if ( asserted )
    {
        assertPin();
    }
    else
    {
        deassertPin();
    }
}


bool Output::isAsserted() const noexcept
{
    GPIO_PinState pinState = HAL_GPIO_ReadPin( m_port, m_pin );
    return m_activeHigh ? ( pinState == GPIO_PIN_SET ) : ( pinState == GPIO_PIN_RESET );
}
