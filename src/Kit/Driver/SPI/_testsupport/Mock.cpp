/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */


#include "Kit/Driver/SPI/_testsupport/Mock.h"
#include <string.h>

///
using namespace Kit::Driver::SPI::TestSupport;


//////////////////////////////////////////////////////////////////////////////
Mock::Mock() noexcept
    : m_started( false )
    , m_failNext( false )
    , m_txCount( 0 )
    , m_rxCount( 0 )
    , m_rxDataPos( 0 )
    , m_rxDataLen( 0 )
{
    memset( m_txBuffer, 0, sizeof( m_txBuffer ) );
    memset( m_rxBuffer, 0, sizeof( m_rxBuffer ) );
    memset( m_rxData, 0, sizeof( m_rxData ) );
}


//////////////////////////////////////////////////////////////////////////////
bool Mock::transfer( const void* txData,
                     void*       rxData,
                     size_t      numBytes ) noexcept
{
    if ( m_failNext )
    {
        m_failNext = false;
        return false;
    }

    if ( txData && m_txCount + numBytes <= MAX_BUFFER_SIZE )
    {
        memcpy( m_txBuffer + m_txCount, txData, numBytes );
        m_txCount += numBytes;
    }

    if ( rxData )
    {
        if ( m_rxDataPos + numBytes <= m_rxDataLen )
        {
            memcpy( rxData, m_rxData + m_rxDataPos, numBytes );
            m_rxDataPos += numBytes;
        }
        else
        {
            memset( rxData, 0xFF, numBytes );
        }
        m_rxCount += numBytes;
    }

    return true;
}


//////////////////////////////////////////////////////////////////////////////
bool Mock::write( const void* txData,
                  size_t      numBytes ) noexcept
{
    if ( m_failNext )
    {
        m_failNext = false;
        return false;
    }

    if ( txData && m_txCount + numBytes <= MAX_BUFFER_SIZE )
    {
        memcpy( m_txBuffer + m_txCount, txData, numBytes );
        m_txCount += numBytes;
    }

    return true;
}


//////////////////////////////////////////////////////////////////////////////
bool Mock::read( void*  rxData,
                 size_t numBytes ) noexcept
{
    if ( m_failNext )
    {
        m_failNext = false;
        return false;
    }

    if ( rxData )
    {
        if ( m_rxDataPos + numBytes <= m_rxDataLen )
        {
            memcpy( rxData, m_rxData + m_rxDataPos, numBytes );
            m_rxDataPos += numBytes;
        }
        else
        {
            memset( rxData, 0xFF, numBytes );
        }
        m_rxCount += numBytes;
    }

    return true;
}


//////////////////////////////////////////////////////////////////////////////
void Mock::setRxData( const void* data, size_t len ) noexcept
{
    if ( len <= MAX_BUFFER_SIZE )
    {
        memcpy( m_rxData, data, len );
        m_rxDataLen = len;
        m_rxDataPos = 0;
    }
}


//////////////////////////////////////////////////////////////////////////////
void Mock::reset() noexcept
{
    m_failNext   = false;
    m_txCount    = 0;
    m_rxCount    = 0;
    m_rxDataPos  = 0;
    m_rxDataLen  = 0;
    memset( m_txBuffer, 0, sizeof( m_txBuffer ) );
    memset( m_rxBuffer, 0, sizeof( m_rxBuffer ) );
    memset( m_rxData, 0, sizeof( m_rxData ) );
}
