/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */


#include "Kit/Driver/SPI/ST/M32F4/Api.h"
#include <string.h>

///
using namespace Kit::Driver::SPI::ST::M32F4;


//////////////////////////////////////////////////////////////////////////////
Api::Api( SPI_HandleTypeDef* spiHandle,
          uint32_t           timeoutMs ) noexcept
    : m_spiHandle( spiHandle )
    , m_timeoutMs( timeoutMs )
    , m_started( false )
{
}


//////////////////////////////////////////////////////////////////////////////
bool Api::start( void* startArgs ) noexcept
{
    if ( m_spiHandle == nullptr )
    {
        return false;
    }

    m_started = true;
    return true;
}


void Api::stop() noexcept
{
    m_started = false;
}


//////////////////////////////////////////////////////////////////////////////
bool Api::transfer( const void* txData,
                    void*       rxData,
                    size_t      numBytes ) noexcept
{
    if ( !m_started || txData == nullptr || rxData == nullptr || numBytes == 0 )
    {
        return false;
    }

    HAL_StatusTypeDef status = HAL_SPI_TransmitReceive(
        m_spiHandle,
        const_cast<uint8_t*>( static_cast<const uint8_t*>( txData ) ),
        static_cast<uint8_t*>( rxData ),
        static_cast<uint16_t>( numBytes ),
        m_timeoutMs );

    return ( status == HAL_OK );
}


//////////////////////////////////////////////////////////////////////////////
bool Api::write( const void* txData,
                 size_t      numBytes ) noexcept
{
    if ( !m_started || txData == nullptr || numBytes == 0 )
    {
        return false;
    }

    HAL_StatusTypeDef status = HAL_SPI_Transmit(
        m_spiHandle,
        const_cast<uint8_t*>( static_cast<const uint8_t*>( txData ) ),
        static_cast<uint16_t>( numBytes ),
        m_timeoutMs );

    return ( status == HAL_OK );
}


//////////////////////////////////////////////////////////////////////////////
bool Api::read( void*  rxData,
                size_t numBytes ) noexcept
{
    if ( !m_started || rxData == nullptr || numBytes == 0 )
    {
        return false;
    }

    // Use HAL_SPI_TransmitReceive with separate dummy TX buffer instead
    // of HAL_SPI_Receive.  The STM32F4 HAL's HAL_SPI_Receive() does not
    // clock data correctly in SPI Mode 3 (CPOL=1, CPHA=1), resulting
    // in all-zero reads.  TransmitReceive avoids this by driving the
    // clock via explicit dummy byte transmission.
    static constexpr size_t CHUNK_SIZE = 32;
    uint8_t  dummyTx[CHUNK_SIZE];
    uint8_t* dst       = static_cast<uint8_t*>( rxData );
    size_t   remaining = numBytes;

    while ( remaining > 0 )
    {
        size_t n = ( remaining > CHUNK_SIZE ) ? CHUNK_SIZE : remaining;
        memset( dummyTx, 0xFF, n );

        HAL_StatusTypeDef status = HAL_SPI_TransmitReceive(
            m_spiHandle,
            dummyTx,
            dst,
            static_cast<uint16_t>( n ),
            m_timeoutMs );

        if ( status != HAL_OK )
        {
            return false;
        }

        dst       += n;
        remaining -= n;
    }

    return true;
}
