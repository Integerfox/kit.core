#ifndef KIT_CHECKSUM_CRC32PLDMFAST_H_
#define KIT_CHECKSUM_CRC32PLDMFAST_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Checksum/Crc32EthernetFast.h"


///
namespace Kit {
///
namespace Checksum {

/** This class provides the 32 Bit CRC used by the DMTF PLDM standard (e.g. the
    PLDMVersionData integrity checksum).  It is a distinct type, but it adds
    nothing to Crc32EthernetFast, i.e. Crc32EthernetFast is the source of
    truth for the implementation.

    DSP0240 (PLDM Base Specification, v1.0.0) specifies:
        o The polynomial is the same as the one used by IEEE 802.3 (0x04C11DB7)
        o Each byte is processed least significant bit first, i.e. the data
          bytes and the remainder are reflected
        o The checksum is a uint32 field, i.e. it is Little Endian (the
          default PLDM byte ordering) in the 'destBuffer'

    DSP0240 does NOT specify the initial remainder or the final XOR value.
    The standard CRC-32 values are used (and are what PLDM implementations
    use): the initial remainder is 0xFFFFFFFF, and the final remainder is
    XOR'd with 0xFFFFFFFF.  These are exactly the characteristics of
    Crc32EthernetFast, including its Little Endian 'destBuffer' encoding.
    The CRC value for the text string "123456789" is 0xCBF43926.

    https://www.dmtf.org/sites/default/files/standards/documents/DSP0240_1.0.0.pdf
 */
class Crc32PldmFast : public Crc32EthernetFast
{
public:
    /// Constructor
    Crc32PldmFast() noexcept
        : Crc32EthernetFast()
    {
    }
};

}  // end namespaces
}
#endif  // end header latch
