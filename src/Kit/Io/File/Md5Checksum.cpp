/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Md5Checksum.h"
#include "Input.h"
#include "Kit/Checksum/Utils.h"

//------------------------------------------------------------------------------
namespace Kit {
namespace Io {
namespace File {

bool calcMd5Checksum( const char*                  fullPathName,
                      Kit::Checksum::Md5::Digest_T dstDigest,
                      Kit::Text::IString*          dstString,
                      size_t*                      dstFileLen ) noexcept
{
    Input infd( fullPathName );
    if ( !infd.isOpened() )
    {
        return false;
    }

    // Read the entire file contents
    Kit::Checksum::Md5 md5;
    uint8_t            workBuffer[OPTION_KIT_IO_FILE_MD5_WORK_BUFFER_SIZE];
    size_t             fileLen = 0;
    Kit::Type::SSize_T bytesRead;
    for ( ;; )
    {
        // Read one 'chunk' at time
        if ( !infd.read( workBuffer, sizeof( workBuffer ), bytesRead ) )
        {
            // Error OTHER than End-of-File
            if ( !infd.isEof() )
            {
                return false;
            }

            // Success
            if ( !md5.finalize( dstDigest, sizeof( Kit::Checksum::Md5::Digest_T ) ) )
            {
                return false;
            }
            if ( dstString )
            {
                Kit::Checksum::binToAsciiHex( dstDigest, sizeof( Kit::Checksum::Md5::Digest_T ), *dstString );
            }
            if ( dstFileLen )
            {
                *dstFileLen = fileLen;
            }
            return true;
        }

        // Calculate the checksum
        md5.accumulate( workBuffer, static_cast<unsigned>( bytesRead ) );
        fileLen += static_cast<size_t>( bytesRead );
    }
}

}  // end namespace
}
}
//------------------------------------------------------------------------------
