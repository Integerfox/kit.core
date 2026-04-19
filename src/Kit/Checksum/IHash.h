#ifndef KIT_CHECKSUM_IHASH_H_
#define KIT_CHECKSUM_IHASH_H_
/*-----------------------------------------------------------------------------
 * This file is part of the Colony.Core Project.  The Colony.Core Project is an
 * open source project with a BSD type of licensing agreement.  See the license
 * agreement (license.txt) in the top/ directory or on the Internet at
 * http://integerfox.com/colony.core/license.txt
 *
 * Copyright (c) 2014-2025  John T. Taylor
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include <stdint.h>

///
namespace Kit {
///
namespace Checksum {


/** This class provides an interface for performing a non-cryptographic Hash on
    a collection of bytes.

    How to generate a Hash result:
        1. Call reset() initialize the Hash
        2. Call accumulate() method for every byte being hash'd
        3. Call finalize() to get the Hashed value.

    NOTE: See the Utils.h file for a utility function to convert the binary Hash
          value into an ASCII Hex string.
 */
class IHash
{
public:
    /// Used to re-use/restart the hash object
    virtual void reset( void ) noexcept = 0;

    ///  Call the method for every byte being hash'd
    virtual void accumulate( const void* bytes, unsigned numbytes = 1 ) noexcept = 0;

    /** Call this method to finalize the Hash.  The calculated Hash value is 
        returned via the 'destBuffer' parameter. The byte ordering of the copied
        Hash value is defined by the concrete child class.

        The method returns true if the Hash value is successfully copied to
        'destBuffer'; else false is returned.

        NOTE: The 'destBufferSize' parameter is verified against the concrete
        child class's Hash size.  If the provided 'destBufferSize' is too
        small, then the Hash value is NOT copied to 'destBuffer' and the
        method returns false.
     */
    virtual bool finalize( void* destBuffer, unsigned destBufferSize ) noexcept = 0;


public:
    /// Virtual destructor
    virtual ~IHash() = default;
};

}       // end namespaces
}
#endif  // end header latch
