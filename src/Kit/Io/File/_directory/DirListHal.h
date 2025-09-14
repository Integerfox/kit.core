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

#include "Kit/Io/File/System.h"

///
namespace Kit {
///
namespace Io {
///
namespace File {


//////////////////////
/** This template class defines a platform independent interface for enumerating the
    files and directory contained within a parent directory.  The template
    argument 'DIRLIST_HDL' is the same type that is used with the DirList class.

    DIRLIST_HDL is platform specific and contains all of the necessary platform
    specific types and data needed to enumerate a directory.

    NOTE: This interface/class is NOT thread safe - i.e. do NOT list a directory
          from multiple threads at the same time.
 */
template <typename DIRLIST_HDL>
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

        The 'maxNameLen' argument specifies the size - which needs to include
        space for the null terminator, in bytes, of the 'firstEntryName' buffer.
     */
    static bool getFirstEntry( DIRLIST_HDL& hdl,
                               const char*  dirNameToList,
                               char*        firstEntryName,
                               unsigned     maxNameLen ) noexcept;

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
    static bool getNextEntry( DIRLIST_HDL& hdl, char* nextEntryName, unsigned maxNameLen ) noexcept;

    /** Populates the System::Info_T struct based on the current directory entry */
    static void setInfo( DIRLIST_HDL& hdl, System::Info_T& info ) noexcept;

    /** Closes the directory handle. This method MUST always be called after a
        successful call to getFirstEntry().
     */
    static void close( DIRLIST_HDL& hdl ) noexcept;
};

}  // end namespaces
}
}
#endif  // end header latch