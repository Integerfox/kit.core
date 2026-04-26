#ifndef KIT_CHECKSUM_CRC16CCITTFAST_H_
#define KIT_CHECKSUM_CRC16CCITTFAST_H_
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


/** This class provides an implementation for the 16 Bit CRC-CCITT
    standard.  The CRC has following characteristics (aka CRC-CCITT(0xFFFF):
        o The polynomial is x16 + x12 + x5 + 1
        o The Data bytes are NOT reflected
        o The remainder is NOT reflected.
        o The final remainder is NOT XOR'd
 */

class Crc16CcittFast : public IEdc
{
public:
    /// Constructor
    Crc16CcittFast() noexcept;


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
    uint16_t m_crc;
};


}  // end namespaces
}
#endif  // end header latch
