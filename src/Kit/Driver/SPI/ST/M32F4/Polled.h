#ifndef KIT_DRIVER_SPI_ST_M32F4_POLLED_H_
#define KIT_DRIVER_SPI_ST_M32F4_POLLED_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */


#include "Kit/Driver/SPI/IHalfDuplex.h"
#include "Kit/Bsp/Api.h"
#include <stdint.h>


///
namespace Kit {
///
namespace Driver {
///
namespace SPI {
///
namespace ST {
///
namespace M32F4 {


/** Default HAL timeout in milliseconds for SPI operations */
static constexpr uint32_t DEFAULT_TIMEOUT_MS = 1000;


/** Concrete half-duplex SPI driver for the STM32F4 HAL using polling mode.
    Wraps the STM32 HAL SPI functions (HAL_SPI_Transmit, HAL_SPI_Receive)
    into the Kit::Driver::SPI::IHalfDuplex interface.

    The 'spiHandle' must be a pointer to a fully initialized
    SPI_HandleTypeDef (e.g., &hspi3 from CubeMX).

    The caller is responsible for managing the SPI chip select (CS) pin
    externally via a Dio::IOutput driver.
 */
class Polled : public IHalfDuplex
{
public:
    /** Constructor.

        @param spiHandle  Pointer to the STM32 HAL SPI handle.  Must not be
                          nullptr.  Must remain valid for the lifetime of
                          this object.
        @param timeoutMs  Timeout in milliseconds for HAL polling operations.
     */
    Polled( SPI_HandleTypeDef* spiHandle,
            uint32_t           timeoutMs = DEFAULT_TIMEOUT_MS ) noexcept;


public:
    /// See Kit::Driver::IStart
    bool start( void* startArgs = nullptr ) noexcept override;

    /// See Kit::Driver::IStop
    void stop() noexcept override;


public:
    /// See Kit::Driver::SPI::IHalfDuplex
    bool write( const void* txData,
                size_t      numBytes ) noexcept override;

    /// See Kit::Driver::SPI::IHalfDuplex
    bool read( void*  rxData,
               size_t numBytes ) noexcept override;


protected:
    SPI_HandleTypeDef* m_spiHandle;  //!< Pointer to STM32 HAL SPI handle
    uint32_t           m_timeoutMs;  //!< HAL polling timeout in ms
    bool               m_started;    //!< Driver started flag
};


}  // end namespaces
}
}
}
}
#endif  // end header latch
