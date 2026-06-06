#ifndef KIT_DRIVER_SPI_IHALFDUPLEX_H_
#define KIT_DRIVER_SPI_IHALFDUPLEX_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */


#include <stdlib.h>
#include "Kit/Driver/IStart.h"
#include "Kit/Driver/IStop.h"

///
namespace Kit {
///
namespace Driver {
///
namespace SPI {


/** This class defines the interface for a platform independent half-duplex
    SPI driver.  The interface provides write-only and read-only operations.
    Half-duplex mode requires only a single buffer per transaction and covers
    the vast majority of SPI peripheral usage.

    The interface itself is NOT thread safe. It is the responsibility of
    the users/clients of the driver to handle any threading issues.
 */
class IHalfDuplex : public virtual IStart,
                    public virtual IStop
{
public:
    /** This method transmits 'numBytes' from 'txData' on the SPI bus.
        Any data received during transmission is discarded.

        The method returns true if the operation was successful; else false
        is returned.
     */
    virtual bool write( const void* txData,
                        size_t      numBytes ) noexcept = 0;

    /** This method reads 'numBytes' from the SPI bus into 'rxData'.
        Dummy bytes (typically 0x00 or 0xFF) are transmitted during the read.

        The method returns true if the operation was successful; else false
        is returned.
     */
    virtual bool read( void*  rxData,
                       size_t numBytes ) noexcept = 0;


public:
    /// Virtual destructor
    virtual ~IHalfDuplex() noexcept = default;
};


}  // end namespaces
}
}
#endif  // end header latch
