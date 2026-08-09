#ifndef KIT_TYPES_SSIZET_H_
#define KIT_TYPES_SSIZET_H_
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
#include <stdint.h>


///
namespace Kit {
///
namespace Type {

/*----------------------------------------------------------------------------*/
/** This is my work-around for the fact that C++ does not have a standard
    'ssize_t' type. ssize_t is defined in POSIX and one primary usage is with
    read/write operations using file descriptors.Signed data type for arguments,
    variables, etc. with respect to how many bytes can be read/written from/to
    streams, files, etc.  The default is for a signed 32 bit integer.  A
    platform can override the default to make larger or smaller as needed.


 */
#ifndef OPTION_KIT_TYPE_SSIZET_LENGTH_TYPE
using SSize_T = int32_t;

#else
using SSize_T = OPTION_KIT_TYPE_SSIZET_LENGTH_TYPE;
#endif

}  // end namespaces
}
#endif  // end header latch
