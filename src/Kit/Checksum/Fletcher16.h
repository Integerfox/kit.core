#ifndef KIT_CHECKSUM_FLETCHER16_H_
#define KIT_CHECKSUM_FLETCHER16_H_
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


/** This class provides an implementation for the 16 Bit wide
    Fletcher Checksum
 */
class Fletcher16 : public IEdc
{
public:
    /// Constructor
    Fletcher16() noexcept;


public:
    /// See Kit::Checksum::IEdc
    void reset( void ) noexcept override;

    /// See Kit::Checksum::IEdc
    void accumulate( const void* bytes, unsigned numbytes = 1 ) noexcept override;

    /// See Kit::Checksum::IEdc. The checksum is Little Endian in the 'destBuffer'
    bool finalize( void* destBuffer, unsigned destBufferSize ) noexcept override;

    /// See Kit::Checksum::IEdc
    bool isOkay( void ) noexcept override;

    /// See Kit::Checksum::IEdc
    unsigned getEdcSize() const noexcept override { return sizeof( m_sum1 ) + sizeof( m_sum2 ); }

protected:
    /// current sum
    uint8_t m_sum1;

    /// current sum
    uint8_t m_sum2;
};


}       // end namespaces
}
#endif  // end header latch
