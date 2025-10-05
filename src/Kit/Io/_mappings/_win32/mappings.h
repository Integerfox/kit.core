#ifndef KIT_IO_MAPPINGS_WIN32_MAPPINGS_H_
#define KIT_IO_MAPPINGS_WIN32_MAPPINGS_H_
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
#include <Windows.h>
#define KitIoStdioHandle_T_MAP                HANDLE  //!< Mapping
#define KitIoSocketHandle_T_MAP               SOCKET  //!< Mapping
#define KitIoFileHandle_T_MAP                 HANDLE  //!< Mapping
#define KitIoFileDirectory_T_MAP              HANDLE  //!< Mapping


#define KIT_IO_NEW_LINE_NATIVE_MAP            "\015\012"  //!< Mapping
#define KIT_IO_DIRECTORY_SEPARATOR_NATIVE_MAP '\\'        //!< Mapping
#define KIT_IO_FILE_MAX_NAME_MAP              _MAX_PATH   //!< Mapping


///////////////////////////////////////////////////////////////////////////////


#endif  // end header latch
