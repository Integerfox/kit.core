#ifndef KIT_IO_FILE_MD5CHECKSUM_H_
#define KIT_IO_FILE_MD5CHECKSUM_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "kit_config.h"
#include "Kit/Checksum/Md5.h"
#include "Kit/Text/IString.h"
#include <stddef.h>

/** Size, in bytes, of the work buffer used when reading the file contents.
    WARNING: The work buffer is allocated on the STACK - so do NOT make this
             too large
 */
#ifndef OPTION_KIT_IO_FILE_MD5_WORK_BUFFER_SIZE
#define OPTION_KIT_IO_FILE_MD5_WORK_BUFFER_SIZE 128
#endif

///
namespace Kit {
///
namespace Io {
///
namespace File {


/** This method calculates the MD5 Checksum of the specified file.  The method
    returns true when successful.  The caller is required to provide the memory
    for 'dstDigest'.

    When 'dstString' is non null, the checksum as an upper-case ASCII HEX string
    is returned.

    When 'dstFileLen' is non null, the length (in bytes) of the file is returned.
 */
bool calcMd5Checksum( const char*                  fullPathName,
                      Kit::Checksum::Md5::Digest_T dstDigest,
                      Kit::Text::IString*          dstString  = nullptr,
                      size_t*                      dstFileLen = nullptr ) noexcept;


}  // end namespaces
}
}
#endif  // end header latch
