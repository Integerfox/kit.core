#ifndef KIT_DRIVER_SPI_TESTSUPPORT_MOCK_H_
#define KIT_DRIVER_SPI_TESTSUPPORT_MOCK_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file
    Mock SPI driver for unit testing.  Records all transactions for
    verification and allows injection of rx data and error conditions.
*/


#include "Kit/Driver/SPI/IApi.h"
#include <stdint.h>


///
namespace Kit {
///
namespace Driver {
///
namespace SPI {
///
namespace TestSupport {


/** Mock SPI driver that records all SPI transactions.
    When read() is called, it returns data from the rxBuffer
    previously configured via setRxData().
 */
class Mock : public IApi
{
public:
    /// Maximum buffer size for tracking SPI traffic
    static constexpr size_t MAX_BUFFER_SIZE = 16 * 1024;

public:
    /// Constructor
    Mock() noexcept;


public:
    /// See Kit::Driver::IStart
    bool start( void* startArgs = nullptr ) noexcept override { m_started = true; return true; }

    /// See Kit::Driver::IStop
    void stop() noexcept override { m_started = false; }


public:
    /// See Kit::Driver::SPI::IApi
    bool transfer( const void* txData,
                   void*       rxData,
                   size_t      numBytes ) noexcept override;

    /// See Kit::Driver::SPI::IApi
    bool write( const void* txData,
                size_t      numBytes ) noexcept override;

    /// See Kit::Driver::SPI::IApi
    bool read( void*  rxData,
               size_t numBytes ) noexcept override;


public:
    /// Configures data to be returned on subsequent read/transfer calls
    void setRxData( const void* data, size_t len ) noexcept;

    /// Causes the next SPI operation to fail
    void setFailNext() noexcept { m_failNext = true; }

    /// Resets all state
    void reset() noexcept;

    /// Returns the transmitted data buffer
    const uint8_t* getTxBuffer() const noexcept { return m_txBuffer; }

    /// Returns the number of bytes transmitted
    size_t getTxCount() const noexcept { return m_txCount; }

    /// Returns the number of bytes received
    size_t getRxCount() const noexcept { return m_rxCount; }


protected:
    bool     m_started;
    bool     m_failNext;
    uint8_t  m_txBuffer[MAX_BUFFER_SIZE];
    uint8_t  m_rxBuffer[MAX_BUFFER_SIZE];
    uint8_t  m_rxData[MAX_BUFFER_SIZE];
    size_t   m_txCount;
    size_t   m_rxCount;
    size_t   m_rxDataPos;
    size_t   m_rxDataLen;
};


}  // end namespaces
}
}
}
#endif  // end header latch
