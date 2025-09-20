/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "DirWalker.h"
#include "Kit/System/Assert.h"
#include "Kit/System/Trace.h"
#include "Kit/Text/Strip.h"

#define SECT_ "Kit::Io::File::DirWalker"

//------------------------------------------------------------------------------
namespace Kit {
namespace Io {
namespace File {


bool DirWalker::traverse( const char*    dirToList,
                          IDirTraverser& callback,
                          unsigned       depth,
                          bool           filesOnly,
                          bool           dirsOnly ) noexcept
{
    // House keeping
    KIT_SYSTEM_ASSERT( dirToList != nullptr );
    m_stack.clearTheStack();
    m_curDir.name     = dirToList;
    m_curDir.curDepth = 1;
    if ( depth > OPTION_KIT_IO_FILE_DIRWALKER_MAX_DEPTH )
    {
        depth = OPTION_KIT_IO_FILE_DIRWALKER_MAX_DEPTH;
    }

    // Sanity check (e.g. the default constructor was used, but reset() was not called)
    if ( m_curDir.name.isEmpty() )
    {
        return false;
    }

    // Ensure my current root ends with a trailing dir separator
    ensureTrailingDirSep( m_curDir.name );

    // Push the root directory on the stack
    m_stack.push( m_curDir );
    while ( !m_stack.isEmpty() )
    {
        bool newDirLevel = false;
        m_stack.pop( m_curDir );
        unsigned curDepth = m_curDir.curDepth;
        KIT_SYSTEM_TRACE_MSG( SECT_, "curDir=%s, curDepth=%d", m_curDir.name.getString(), curDepth );

        // Open the current directory
        KitIoFileDirectory_T hdl;
        if ( System::getFirstDirEntry( hdl, m_curDir.name, m_curFsEntry ) == false )
        {
            return false;
        }

        // Read the content's of the directory
        while ( !m_curFsEntry.isEmpty() )
        {
            // Ignore "." and ".." entries
            const char* ptr = Kit::Text::Strip::chars( m_curFsEntry, "." );
            if ( *ptr != '\0' )
            {

                // Construct full path for the current item
                m_wrkEntryName  = m_curDir.name();
                m_wrkEntryName += m_curFsEntry;

                // Populate the Info struct
                System::Info_T cbInfo;
                if ( System::getInfo( m_wrkEntryName, cbInfo ) == false )
                {
                    // Error getting the file system information
                    System::closeDirectory( hdl );
                    return false;
                }

                // The current entry is a directory
                if ( cbInfo.m_isDir )
                {
                    // Callback the client
                    if ( ( !filesOnly && !dirsOnly ) || dirsOnly )
                    {
                        if ( callback.item( m_curDir.name, m_curFsEntry, cbInfo ) == Kit::Type::TraverserStatus::eABORT )
                        {
                            System::closeDirectory( hdl );
                            return false;
                        }
                    }

                    // Track my depth
                    KIT_SYSTEM_TRACE_MSG( SECT_,
                                          "\nISDIR: name=%s, newDirLevel=%s, curDepth=%d, max depth=%d",
                                          m_wrkEntryName(),
                                          newDirLevel ? "YES" : "no",
                                          curDepth,
                                          depth );
                    if ( !newDirLevel )
                    {
                        curDepth++;
                    }

                    // Limit the depth of the traversal
                    if ( curDepth <= depth )
                    {
                        newDirLevel = true;

                        // Push the found directory name onto the stack
                        // Note: We cheat here and create a place holder item on the stack,
                        //       then once on the stack - we update the item to reflect the
                        //       directory found
                        m_stack.push( m_curDir );                         // Push a place holder item on to the stack
                        DirDepth_T& pushedItem = *( m_stack.peekTop() );  // Get reference to the new item on the stack
                        ensureTrailingDirSep( m_wrkEntryName );           // Make sure the new directory has a trailing '/'
                        pushedItem.name     = m_wrkEntryName;             // Update the item on the stack with the new dir found
                        pushedItem.curDepth = curDepth;
                        KIT_SYSTEM_TRACE_MSG( SECT_,
                                              "\nPUSHED dir=%s (parent=%s), curDepth=%d",
                                              pushedItem.name.getString(),
                                              m_curDir.name.getString(),
                                              curDepth );
                    }
                }

                // The current entry is a file
                else
                {
                    if ( ( !filesOnly && !dirsOnly ) || filesOnly )
                    {
                        if ( callback.item( m_curDir.name, m_curFsEntry, cbInfo ) == Kit::Type::TraverserStatus::eABORT )
                        {
                            System::closeDirectory( hdl );
                            return false;
                        }
                    }
                }
            }

            // Get the next entry in the directory
            if ( System::getNextDirEntry( hdl, m_curFsEntry ) == false )
            {
                // Error reading the directory
                System::closeDirectory( hdl );
                return false;
            }
        }

        // Close the current directory
        System::closeDirectory( hdl );
    }

    return true;
}

void DirWalker::ensureTrailingDirSep( NameString& name ) noexcept
{
    // Ensure my current root ends with a trailing dir separator
    if ( name[name.length() - 1] != dirSep )
    {
        name += dirSep;
    }
}

}  // end namespace
}
}
//------------------------------------------------------------------------------