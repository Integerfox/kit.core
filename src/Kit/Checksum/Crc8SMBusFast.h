#ifndef KIT_CHECKSUM_CRC8SMBUSFAST_H_
#define KIT_CHECKSUM_CRC8SMBUSFAST_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Checksum/IEdc.h"


///
namespace Kit {
///
namespace Checksum {


/** This class provides a CRC8 "SMBus" algorithm using a table look-up.  The CRC
    has following characteristics:
        o The polynomial is x8 + x2 + x + 1 (0x07)
        o The initial remainder is 0x00
        o The Data bytes are NOT reflected
        o The remainder is NOT reflected.
        o The final remainder is NOT XOR'd
 */
class Crc8SMBusFast : public IEdc
{
public:
    /// Constructor
    Crc8SMBusFast() noexcept;


public:
    /// See Kit::Checksum::IEdc
    void reset( void ) noexcept override;

    /// See Kit::Checksum::IEdc
    void accumulate( const void* bytes, unsigned numbytes = 1 ) noexcept override;

    /// See Kit::Checksum::IEdc
    bool finalize( void* destBuffer, unsigned destBufferSize ) noexcept override;

    /// See Kit::Checksum::IEdc
    bool isOkay( void ) noexcept override;

    /// See Kit::Checksum::IEdc
    unsigned getEdcSize() const noexcept override { return sizeof( m_crc ); }

protected:
    /// Current CRC
    uint8_t m_crc;
};


}  // end namespaces
}
#endif  // end header latch
