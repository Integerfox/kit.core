#ifndef KIT_CHECKSUM_UTILS_H_
#define KIT_CHECKSUM_UTILS_H_
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
/** @file

    The file provides utility functions for the Checksum module.

 */

#include "Kit/Text/IString.h"
#include "Kit/Text/Format.h"

///
namespace Kit {
///
namespace Checksum {

/** This 'convenience' method converts the a binary EDC or Hash value into an
    ASCII Hex string. Returns true if the conversion is successful; else false
    is returned.

    NOTE: If the IString is too small to hold the entire binary value as ASCII Hex
          the dstString argument is still updated with a  truncated value and the
          method return false.
 */
inline bool binToAsciiHex( void*               srcBinValue,
                           unsigned            srcBinSize,
                           Kit::Text::IString& dstString,
                           bool                uppercase = true,
                           bool                append    = false,
                           char                separator = '\0' ) noexcept
{
    return Kit::Text::Format::asciiHex( srcBinValue, srcBinSize, dstString, uppercase, append, separator );
}


}  // end namespaces
}
#endif  // end header latch
