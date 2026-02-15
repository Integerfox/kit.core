/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Api.h"

/// Default SPI timeout in milliseconds
#ifndef OPTION_KIT_DRIVER_SPI_ST_M32F4_TIMEOUT
#define OPTION_KIT_DRIVER_SPI_ST_M32F4_TIMEOUT 1000
#endif

///
using namespace Kit::Driver::SPI::ST::M32F4;


/////////////////////////////////////////////////////////////////////////////
Api::Api( SPI_HandleTypeDef* spiHandle ) noexcept
    : m_spiHandle( spiHandle )
    , m_isStarted( false )
{
}


/////////////////////////////////////////////////////////////////////////////
bool Api::start( void* startArgs ) noexcept
{
    if ( m_isStarted )
    {
        return true;
    }

    if ( m_spiHandle == nullptr )
    {
        return false;
    }

    // Verify the SPI peripheral is in ready state
    if ( HAL_SPI_GetState( m_spiHandle ) != HAL_SPI_STATE_READY )
    {
        return false;
    }

    m_isStarted = true;
    return true;
}


void Api::stop() noexcept
{
    if ( !m_isStarted )
    {
        return;
    }

    // Abort any ongoing transfers
    HAL_SPI_Abort( m_spiHandle );

    m_isStarted = false;
}


/////////////////////////////////////////////////////////////////////////////
bool Api::transfer( const void* txData,
                    void*       rxData,
                    size_t      numBytes ) noexcept
{
    if ( !m_isStarted || numBytes == 0 )
    {
        return false;
    }

    HAL_StatusTypeDef status;

    if ( txData != nullptr && rxData != nullptr )
    {
        // Full duplex transfer
        status = HAL_SPI_TransmitReceive( m_spiHandle,
                                          const_cast<uint8_t*>( static_cast<const uint8_t*>( txData ) ),
                                          static_cast<uint8_t*>( rxData ),
                                          static_cast<uint16_t>( numBytes ),
                                          OPTION_KIT_DRIVER_SPI_ST_M32F4_TIMEOUT );
    }
    else if ( txData != nullptr )
    {
        // Transmit only
        status = HAL_SPI_Transmit( m_spiHandle,
                                   const_cast<uint8_t*>( static_cast<const uint8_t*>( txData ) ),
                                   static_cast<uint16_t>( numBytes ),
                                   OPTION_KIT_DRIVER_SPI_ST_M32F4_TIMEOUT );
    }
    else if ( rxData != nullptr )
    {
        // Receive only
        status = HAL_SPI_Receive( m_spiHandle,
                                  static_cast<uint8_t*>( rxData ),
                                  static_cast<uint16_t>( numBytes ),
                                  OPTION_KIT_DRIVER_SPI_ST_M32F4_TIMEOUT );
    }
    else
    {
        return false;
    }

    return ( status == HAL_OK );
}


bool Api::write( const void* txData,
                 size_t      numBytes ) noexcept
{
    if ( !m_isStarted || txData == nullptr || numBytes == 0 )
    {
        return false;
    }

    HAL_StatusTypeDef status = HAL_SPI_Transmit( m_spiHandle,
                                                 const_cast<uint8_t*>( static_cast<const uint8_t*>( txData ) ),
                                                 static_cast<uint16_t>( numBytes ),
                                                 OPTION_KIT_DRIVER_SPI_ST_M32F4_TIMEOUT );

    return ( status == HAL_OK );
}


bool Api::read( void*  rxData,
                size_t numBytes ) noexcept
{
    if ( !m_isStarted || rxData == nullptr || numBytes == 0 )
    {
        return false;
    }

    HAL_StatusTypeDef status = HAL_SPI_Receive( m_spiHandle,
                                                static_cast<uint8_t*>( rxData ),
                                                static_cast<uint16_t>( numBytes ),
                                                OPTION_KIT_DRIVER_SPI_ST_M32F4_TIMEOUT );

    return ( status == HAL_OK );
}
