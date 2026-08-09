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
#include "Kit/Type/SSize.h"
#include <stdint.h>


///
namespace Kit {
///
namespace Io {

/*----------------------------------------------------------------------------*/
/** The KIT standard newline is "\n", but allow the application to override/change it.
 */
#ifndef OPTION_KIT_IO_NEW_LINE_STANDARD
static constexpr const char* const newline = "\n";
#else
static constexpr const char* const newline = OPTION_KIT_IO_NEW_LINE_STANDARD;
#endif

/// The application must provide/map the native newline character
#define KIT_IO_NEW_LINE_NATIVE KIT_IO_NEW_LINE_NATIVE_MAP

/// Provide an alias for the native newline character(s)
static constexpr const char* const nativeNewline = KIT_IO_NEW_LINE_NATIVE;


/*----------------------------------------------------------------------------*/
/** Defers the "file descriptor" for STDIO file handle type to the target platform.
    This type is used when defining concrete STDIO file classes.
 */
#define KitIoStdioHandle_T KitIoStdioHandle_T_MAP

/** Defers the "file descriptor" for a file handle type to the target platform.
    This type is used when defining concrete file classes.
 */
#define KitIoFileHandle_T KitIoFileHandle_T_MAP


/** Defers the "file descriptor" for a socket handle type to the target platform.
    This type is used when defining concrete socket classes.
 */
#define KitIoSocketHandle_T KitIoSocketHandle_T_MAP


}  // end namespaces
}
#endif  // end header latch
