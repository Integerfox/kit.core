/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */


#include "Kit/Driver/SPI/ST/M32F4/Polled.h"
#include "Kit/System/Assert.h"

///
using namespace Kit::Driver::SPI::ST::M32F4;


//////////////////////////////////////////////////////////////////////////////
Polled::Polled( SPI_HandleTypeDef* spiHandle,
                uint32_t           timeoutMs ) noexcept
    : m_spiHandle( spiHandle )
    , m_timeoutMs( timeoutMs )
    , m_started( false )
{
    KIT_SYSTEM_ASSERT( spiHandle != nullptr );
}


//////////////////////////////////////////////////////////////////////////////
bool Polled::start( void* startArgs ) noexcept
{
    m_started = true;
    return true;
}


void Polled::stop() noexcept
{
    m_started = false;
}


//////////////////////////////////////////////////////////////////////////////
bool Polled::write( const void* txData,
                    size_t      numBytes ) noexcept
{
    if ( !m_started || txData == nullptr || numBytes == 0 )
    {
        return false;
    }

    HAL_StatusTypeDef status = HAL_SPI_Transmit(
        m_spiHandle,
        static_cast<uint8_t*>( const_cast<void*>( txData ) ),
        static_cast<uint16_t>( numBytes ),
        m_timeoutMs );

    return ( status == HAL_OK );
}


//////////////////////////////////////////////////////////////////////////////
bool Polled::read( void*  rxData,
                   size_t numBytes ) noexcept
{
    if ( !m_started || rxData == nullptr || numBytes == 0 )
    {
        return false;
    }

    HAL_StatusTypeDef status = HAL_SPI_Receive(
        m_spiHandle,
        static_cast<uint8_t*>( rxData ),
        static_cast<uint16_t>( numBytes ),
        m_timeoutMs );

    return ( status == HAL_OK );
}
