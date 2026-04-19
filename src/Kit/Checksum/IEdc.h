#ifndef KIT_CHECKSUM_IAPI_H_
#define KIT_CHECKSUM_IAPI_H_
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


///
namespace Kit {
///
namespace Checksum {

/** This abstract class provides a generic interface for an Error-Detecting code
    (EDC) i.e. a Checksum, CRC, etc. The specifics of the EDC is determined by
    the concrete class implementing this interface.

    NOTE: The concrete child classes can have different 'sum' or 'digest' sizes.

    How to generate a EDC:
        1. Call reset() to initialize the EDC
        2. Call the accumulate() method for every byte being EDC'd
        3. Call finalize() to get the EDC value.

    How to verify a EDC:
        1. Call reset() to initialize the EDC
        2. Call the accumulate() method for every byte being EDC'd
           INCLUDING the previously generated EDC bytes.
        3. Call isOkay() which returns true if the buffer passes the EDC
           check.
 */
class IEdc
{
public:
    /// Used to re-use/restart the EDC object
    virtual void reset( void ) noexcept = 0;

    /// Call the method for every byte being EDC'd
    virtual void accumulate( const void* bytes, unsigned numbytes = 1 ) noexcept = 0;

    /** Call this method to finalize the EDC.  The calculated EDC
        value is returned via the 'destBuffer' argument. The byte ordering of 
        the copied EDC value is defined by the concrete child class, i.e. the
        finalize() method can be used to copy the final EDC into the 'outbound
        buffer'

        The method returns true if the EDC value is successfully copied to
        'destBuffer'; else false is returned.

        NOTE: The 'destBufferSize' parameter is verified against the concrete
        child class's EDC size.  If the provided 'destBufferSize' is too
        small, then the EDC value is NOT copied to 'destBuffer' and the
        method returns false.
     */
    virtual bool finalize( void* destBuffer, unsigned destBufferSize ) noexcept = 0;

    /** This method returns true if the data and the incoming EDC bytes,
        that accumulate() has been called on, is good.  Returns true if
        the EDC check passes; else false is returned
     */
    virtual bool isOkay( void ) noexcept = 0;

public:
    /// Retuns the size of the EDC value in bytes. 
    virtual unsigned getEdcSize() const noexcept = 0;

public:
    /// Virtual destructor
    virtual ~IEdc() = default;
};

}  // end namespaces
}
#endif  // end header latch
