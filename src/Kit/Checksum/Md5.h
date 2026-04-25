#ifndef Cpl_Checksum_Md5Aladdin_h_
#define Cpl_Checksum_Md5Aladdin_h_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Checksum/IHash.h"
#include "Kit/Checksum/md5_aladdin_.h"

///
namespace Kit {
///
namespace Checksum {


/** This class provides an implementation for the MD5 Hash interface
    that is wrapper to the third party MD5 Library code developed
    by  L. Peter Deutsch, ghost@aladdin.com
 */
class Md5 : public IHash
{
public:
    /// Number of bytes in the digest
    static constexpr unsigned eDIGEST_LEN = 16;

    /// Digest/result of the hash
    typedef uint8_t Digest_T[eDIGEST_LEN];

public:
    /// Constructor
    Md5() noexcept;


public:
    /// See Kit::Checksum::IHash
    void reset( void ) noexcept;

    /// See Kit::Checksum::IHash
    void accumulate( const void* bytes, unsigned numbytes = 1 ) noexcept;

    /// See Kit::Checksum::IHash
     bool finalize( void* destBuffer, unsigned destBufferSize ) noexcept;

    /// See Kit::Checksum::IHash
    unsigned getDigestSize() const noexcept { return eDIGEST_LEN; }

protected:
    /// Hash state
    md5_state_t m_state;
};

}       // end namespaces
}
#endif  // end header latch
