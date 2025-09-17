#ifndef KIT_IO_FILE_DIR_LIST_HAL_H_
#define KIT_IO_FILE_DIR_LIST_HAL_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "kit_map.h"


/** Defer the HAL type to the platform */
#define KitIoFileDirListHal_T KitIoFileDirListHal_T_MAP

///
namespace Kit {
///
namespace Io {
///
namespace File {


//////////////////////
/** This static class defines a platform independent interface for enumerating the
    files and directory contained within a parent directory.

    KitIoFileDirListHal_T is platform specific and contains all of the necessary
    platform specific types and data needed to enumerate a directory.

    NOTE: This interface/class is NOT thread safe - i.e. do NOT list a directory
          from multiple threads at the same time.
 */
class DirListHal
{
public:
    /** Returns the first entry in the directory.  If the directory cannot be
        opened false is returned; else true is returned.

        The 'hdl' argument is populated with the platform specific directory
        handle information needed for subsequent calls to getNextEntry().

        The 'firstEntryName' argument is populated with the name of the first
        entry in the directory.  If the directory is empty then 'firstEntryName'
        is set to an empty string

        The 'maxNameLen' argument specifies the byte size - which needs to include
        space for the null terminator - of the 'firstEntryName' buffer.
     */
    static bool getFirstEntry( KitIoFileDirListHal_T& hdl,
                               const char*            dirNameToList,
                               char*                  firstEntryName,
                               unsigned               maxNameLen ) noexcept;

    /** Returns the next entry in the directory.  If a file system error was
        encountered false is returned; else true is returned.

        The 'hdl' argument must be the instance that was populated by
        getFirstEntry() or a previous call to getNextEntry().

        The 'nextEntryName' argument is populated with the name of the next
        entry in the directory.  If there are no more entries then
        'nextEntryName' is set to an empty string

        The 'maxNameLen' argument specifies the size - which needs to include
        space for the null terminator, in bytes, of the 'nextEntryName' buffer.
     */
    static bool getNextEntry( KitIoFileDirListHal_T& hdl, char* nextEntryName, unsigned maxNameLen ) noexcept;

    /** Closes the directory handle. This method MUST always be called after a
        successful call to getFirstEntry().
     */
    static void close( KitIoFileDirListHal_T& hdl ) noexcept;
};

}  // end namespaces
}
}
#endif  // end header latch