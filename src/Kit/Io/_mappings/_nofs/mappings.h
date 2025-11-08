#ifndef KIT_IO_MAPPINGS_NOFS_MAPPINGS_H_
#define KIT_IO_MAPPINGS_NOFS_MAPPINGS_H_
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

#define KitIoStdioHandle_T_MAP                int  //!< Mapping.  No FS support -->not used
#define KitIoSocketHandle_T_MAP               int  //!< Mapping.  No FS support -->not used
#define KitIoFileHandle_T_MAP                 int  //!< Mapping.  No FS support -->not used
#define KitIoFileDirectory_T_MAP              int  //!< Mapping.  No FS support -->not used


#define KIT_IO_NEW_LINE_NATIVE_MAP            "\012"  //!< Mapping. No FS support -->not used
#define KIT_IO_DIRECTORY_SEPARATOR_NATIVE_MAP '/'     //!< Mapping. No FS support -->not used
#define KIT_IO_FILE_MAX_NAME_MAP              1       //!< Mapping. No FS support -->not used


///////////////////////////////////////////////////////////////////////////////


#endif  // end header latch
