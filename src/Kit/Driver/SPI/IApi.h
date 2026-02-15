#ifndef KIT_DRIVER_SPI_IAPI_H_
#define KIT_DRIVER_SPI_IAPI_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */


#include <stdint.h>
#include <stdlib.h>
#include "Kit/Driver/IStart.h"
#include "Kit/Driver/IStop.h"

///
namespace Kit {
///
namespace Driver {
///
namespace SPI {


/** This class defines the interface for a platform independent SPI (Serial
    Peripheral Interface) driver. The interface provides basic synchronous
    blocking operations for SPI communication.

    The interface is designed for SPI master mode operation where the 
    application controls the chip select (CS) signal externally via a
    Digital Output interface.

    The interface itself is NOT thread safe. It is the responsibility of
    the users/clients of the driver to handle any threading issues.
 */
class IApi : public virtual IStart,
             public virtual IStop
{
public:
    /** This method performs a simultaneous transmit and receive operation.
        SPI is full-duplex, so data is transmitted and received at the same
        time. If txData is nullptr, dummy bytes (typically 0xFF) are sent.
        If rxData is nullptr, received data is discarded.

        The method returns true if the operation was successful; else false
        is returned.

        Note: The application is responsible for managing the chip select (CS)
              signal before and after calling this method.
     */
    virtual bool transfer( const void* txData,
                           void*       rxData,
                           size_t      numBytes ) noexcept = 0;

    /** This method transmits the specified number of bytes. Any received
        data during transmission is discarded.

        The method returns true if the operation was successful; else false
        is returned.

        Note: The application is responsible for managing the chip select (CS)
              signal before and after calling this method.
     */
    virtual bool write( const void* txData,
                        size_t      numBytes ) noexcept = 0;

    /** This method receives the specified number of bytes. Dummy bytes
        (typically 0xFF) are transmitted during reception.

        The method returns true if the operation was successful; else false
        is returned.

        Note: The application is responsible for managing the chip select (CS)
              signal before and after calling this method.
     */
    virtual bool read( void*  rxData,
                       size_t numBytes ) noexcept = 0;


public:
    /// Virtual destructor
    virtual ~IApi() noexcept = default;
};


};  // end namespaces
};
};
#endif  // end header latch
