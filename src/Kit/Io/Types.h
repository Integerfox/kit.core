#ifndef KIT_IO_TYPES_H_
#define KIT_IO_TYPES_H_
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
#include "kit_map.h"
#include <stdint.h>

///
namespace Kit {
///
namespace Io {

/*----------------------------------------------------------------------------*/
/** Defer the "file descriptor" for STDIO file handle' type to the host platform.
    This type is used when defining concrete STDIO file classes.
 */
#define KitIoStdioHandle_T KitIoStdioHandle_T_MAP

/// Provide a alias (that is less verbose)
using StdioHdl_T = KitIoStdioHandle_T;

#if 0 // Move to the File/ namespace once we get there
/** Let the Platform set the type/class for an 'Input Worker' that is used by 
    concrete File classes to perform the actual Input operations.  The default
    implementation is to the STDIO 'worker' class
 */
#ifndef OPTION_KIT_IO_FILE_INPUT_WORKER_TYPE
using InputWorker_T = class NativeInput;
#else
using InputWorker_T = OPTION_KIT_IO_FILE_INPUT_WORKER_TYPE;
#endif
#endif 

/*----------------------------------------------------------------------------*/
/** Signed data type for arguments, variables, etc. with respect to how many
    bytes can be read/written from/to streams, files, etc.  The default is
    for a signed 32 bit integer.  A platform can override the default to make
    larger or smaller as needed.

    Note: This is my work-around for the fact that C++ does not have a
          standard 'ssize_t' type. ssize_t is defined in POSIX and is used when
          read/write operations using file descriptors.
 */
#ifndef OPTION_KIT_IO_LENGTH_TYPE
using ByteCount_T = int32_t;
using Offset_T    = int32_t;
#else
using ByteCount_T = OPTION_KIT_IO_LENGTH_TYPE;
using Offset_T    = OPTION_KIT_IO_LENGTH_TYPE;
#endif

}  // end namespaces
}
#endif  // end header latch
