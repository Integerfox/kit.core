#ifndef KIT_IO_MAPPINGS_POSIX_MAPPINGS_H_
#define KIT_IO_MAPPINGS_POSIX_MAPPINGS_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file

    This file provides the mapping of the platform specific types for the
    Kit::Io namespace

 */
#include <limits.h>    // For PATH_MAX

#define KitIoStdioHandle_T_MAP         int       //!< Mapping
#define KitIoSocketHandle_T_MAP        int       //!< Mapping
#define KitIoFileHandle_T_MAP          int       //!< Mapping
#define KIT_IO_NEW_LINE_NATIVE_MAP     "\n"      //!< Mapping
#define KIT_IO_FILE_NATIVE_DIR_SEP_MAP '/'       //!< Mapping
#define KIT_IO_FILE_MAX_NAME_MAP       PATH_MAX  //!< Mapping


///////////////////////////////////////////////////////////////////////////////


#endif  // end header latch
