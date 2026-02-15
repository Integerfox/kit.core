#ifndef KIT_DRIVER_SPI_ST_M32F4_API_H_
#define KIT_DRIVER_SPI_ST_M32F4_API_H_
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
#include "Kit/Driver/SPI/IApi.h"

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


/** This concrete class implements the SPI driver interface for STM32F4xx
    microcontrollers using the STM32 HAL library.

    The driver uses blocking (polling) mode for SPI transfers. For applications
    requiring non-blocking operation, consider using DMA-based implementations.

    ## Prerequisites

    The SPI peripheral must be initialized via STM32CubeMX or manually before
    calling start(). The driver assumes:
    - SPI peripheral clock is enabled
    - GPIO pins are configured for SPI function
    - SPI is configured for master mode (for flash communication)

    ## Thread Safety

    This implementation is NOT thread safe. The application must provide
    synchronization if the SPI bus is shared between multiple threads.

    ## Timeout

    A default timeout of 1000ms is used for SPI operations. This can be
    configured via the OPTION_KIT_DRIVER_SPI_ST_M32F4_TIMEOUT define.
 */
class Api : public Kit::Driver::SPI::IApi
{
public:
    /** Constructor.

        @param spiHandle    Pointer to HAL SPI handle (from CubeMX)
     */
    Api( SPI_HandleTypeDef* spiHandle ) noexcept;


public:
    /// See Kit::Driver::IStart
    bool start( void* startArgs = nullptr ) noexcept override;

    /// See Kit::Driver::IStop
    void stop() noexcept override;


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


protected:
    /// HAL SPI handle
    SPI_HandleTypeDef* m_spiHandle;

    /// Flag indicating if driver is started
    bool m_isStarted;


private:
    /// Prevent access to the copy constructor
    Api( const Api& ) = delete;

    /// Prevent access to the assignment operator
    const Api& operator=( const Api& ) = delete;
};


};  // end namespaces
};
};
};
};
#endif  // end header latch
