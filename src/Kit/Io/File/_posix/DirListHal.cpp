/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Io/File/_directory/DirListHal.h"
#include <errno.h>
#include <string.h>


//------------------------------------------------------------------------------
namespace Kit {
namespace Io {
namespace File {

bool DirListHal::getFirstEntry( KitIoFileDirListHal_T& hdl,
                                const char*            dirNameToList,
                                char*                  firstEntryName,
                                unsigned               maxNameLen ) noexcept
{
    hdl = opendir( dirNameToList );
    if ( hdl != nullptr )
    {
        // Get the first entry
        if ( getNextEntry( hdl, firstEntryName, maxNameLen ) )
        {
            // Check for the Directory being empty -->need to make sure the directory FD gets closed
            if ( firstEntryName[0] == '\0' )
            {
                closedir( hdl );
            }
            return true;
        }

        // Error reading the directory -->need to make sure the directory FD gets closed
        closedir( hdl );
    }

    // Error opening the directory
    return false;
}

bool DirListHal::getNextEntry( KitIoFileDirListHal_T& hdl, char* nextEntryName, unsigned maxNameLen ) noexcept
{
    errno                   = 0;
    struct dirent* entryPtr = readdir( hdl );
    if ( entryPtr != nullptr )
    {
        strncpy( nextEntryName, entryPtr->d_name, maxNameLen );
        nextEntryName[maxNameLen - 1] = '\0';  // Ensure null termination
    }

    // No more entries
    else if ( errno == 0 )
    {
        nextEntryName[0] = '\0';
    }

    // File system error
    else
    {
        return false;
    }

    // If I get here - then success or end-of-directory
    return true;
}

void DirListHal::close( KitIoFileDirListHal_T& hdl ) noexcept
{
    closedir( hdl );
}


}  // end namespace
}
}
//------------------------------------------------------------------------------