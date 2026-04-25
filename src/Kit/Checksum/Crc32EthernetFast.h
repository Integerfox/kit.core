#ifndef KIT_CHECKSUM_CRC32ETHERNETFAST_H_
#define KIT_CHECKSUM_CRC32ETHERNETFAST_H_
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

/** This class provides an implementation for the 32 Bit "Ethernet"
    standard CRC.  The CRC has following characteristics:
        o The polynomial is: x32 + x26 + x23 + x22 + x16 + x12 + x11 + x10 + x8 + x7 + x5 + x4 + x2 + x + 1
        o The Data bytes are NOT reflected
        o The remainder is NOT reflected.
        o The final remainder is NOT XOR'd
 */
class Crc32EthernetFast : public IEdc
{
public:
    /// Constructor
    Crc32EthernetFast() noexcept;


public:
    /// See Kit::Checksum::IEdc
    void reset( void ) noexcept override;

    /// See Kit::Checksum::IEdc
    void accumulate( const void* bytes, unsigned numbytes = 1 ) noexcept override;

    /// See Kit::Checksum::IEdc. The checksum is Big Endian in the 'destBuffer'
    bool finalize( void* destBuffer, unsigned destBufferSize ) noexcept override;

    /// See Kit::Checksum::IEdc
    bool isOkay( void ) noexcept override;

    /// See Kit::Checksum::IEdc
    unsigned getEdcSize() const noexcept override { return sizeof( m_crc ); }

protected:
    /// Calculated CRC value
    uint32_t m_crc;
};

}  // end namespaces
}
#endif  // end header latch
