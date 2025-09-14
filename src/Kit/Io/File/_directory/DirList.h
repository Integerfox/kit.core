#ifndef KIT_IO_FILE_DIR_LIST_H_
#define KIT_IO_FILE_DIR_LIST_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Type/TraverserStatus.h"
#include "kit_config.h"
#include "Kit/Io/File/System.h"
#include "Kit/Io/File/_directory/DirListHal.h"
#include "Kit/Container/Stack.h"
#include "Kit/System/Assert.h"

/** The maximum supported directory depth */
#ifndef OPTION_KIT_IO_FILE_DIRLIST_MAX_DEPTH
#define OPTION_KIT_IO_FILE_DIRLIST_MAX_DEPTH 4
#endif


///
namespace Kit {
///
namespace Io {
///
namespace File {


//////////////////////
/** This concrete template is a platform independent class that traverse a parent
    directory and optionally 'walks' the parent's child directories.  What is
    'different' about this class - is that it does NOT use recursion to walk the
    child directories.

    The platform specific operation are provided by the DirListHal interface.

    The template parameter 'DIRLIST_HDL' is the platform specific and is the
    same type used with the DirListHal class.
 */
template <typename DIRLIST_HDL>
class DirList
{
public:
    /// Trace section identifier
    static constexpr const char* const MYSECT_ = "Kit::Io::File::DirList";

public:
    /// Tracks depth
    struct DirDepth_T
    {
        Kit::Io::File::NameString name;      //!< Current directory name (at depth 'curDepth')
        unsigned                  curDepth;  //!< Current depth (1=root dir)

        /// Constructor
        DirDepth_T( const char* name )
            : name( name )
            , curDepth( 1 )
        {
        }

        /// Constructor
        DirDepth_T()
            : name( "" )
            , curDepth( 1 )
        {
        }
    };

public:
    /// Constructor
    DirList( const char* rootDir,
             int         depth     = 1,
             bool        filesOnly = false,
             bool        dirsOnly  = false ) noexcept
        : m_curDir( rootDir )
        , m_stack( OPTION_KIT_IO_FILE_DIRLIST_MAX_DEPTH, m_stackMemory )
        , m_depth( depth )
        , m_filesOnly( filesOnly )
        , m_dirsOnly( dirsOnly )
    {
        KIT_SYSTEM_ASSERT( rootDir != nullptr );
    }

public:
    /// See Kit::Io::File::System::walkDirectory
    bool traverse( System::DirectoryWalker& callback ) noexcept
    {
        // Push the root directory on the stack
        m_stack.push( m_curDir );
        while ( !m_stack.isEmpty() )
        {
            bool newDirLevel = false;
            m_stack.pop( m_curDir );
            unsigned curDepth = m_curDir.curDepth;
            KIT_SYSTEM_TRACE_MSG( MYSECT_, "curDir=%s, curDepth=%d", m_curDir.name.getString(), curDepth );

            // Open the current directory
            DIRLIST_HDL hdl;
            if ( DirListHal<DIRLIST_HDL>::getFirstEntry( hdl, m_curDir.name(), m_curFsEntry, sizeof( m_curFsEntry ) ) == false )
            {
                return false;
            }

            // Read the content's of the directory
            do
            {
                // Exit if there are no more entries
                if ( m_curFsEntry[0] == 0 )
                {
                    break;
                }

                // Filter out the "." and ".." directories
                if ( strcmp(m_curFsEntry[0], ".") == 0 || strcmp(m_curFsEntry[0], "..") == 0 )
                {
                    continue;
                }

                // Populate the Info struct
                System::Info_T cbInfo;
                DirListHal<DIRLIST_HDL>::setInfo( hdl, cbInfo );

                // The current entry is a directory
                if ( cbInfo.m_isDir )
                {
                    // Callback the client
                    if ( ( !m_filesOnly && !m_dirsOnly ) || m_dirsOnly )
                    {
                        if ( callback.item( m_curDir.name, m_curFsEntry, cbInfo ) == Kit::Type::TraverserStatus::eABORT )
                        {
                            // TODO: Close DIR on error
                            return false;
                        }
                    }

                    // Track my depth
                    KIT_SYSTEM_TRACE_MSG( MYSECT_,
                                           "\nISDIR: m_name=%s, fs=%s, newDirLevel=%d, curDepth=%d, m_depth=%d",
                                            m_curDir.name.getString(),
                                            m_curFsEntry,
                                            newDirLevel,
                                            curDepth,
                                            m_depth ) ;
                    if ( !newDirLevel )
                    {
                        curDepth++;
                    }

                    // Limit the depth of the traversal
                    if ( curDepth <= m_depth )
                    {
                        newDirLevel = true;

                        // Push the found directory name onto the stack
                        m_stack.push( m_curDir );
                        DirDepth_T& pushedItem = *( m_stack.peekTop() );  // Cheat here and use the memory on the stack instead of allocating a local variable
                        if ( pushedItem.name != "/" )
                        {
                            pushedItem.name += "/";
                        }
                        pushedItem.name     += m_curFsEntry;
                        pushedItem.curDepth  = curDepth;
                        KIT_SYSTEM_TRACE_MSG( MYSECT_,
                                               "\nPUSHED dir=%s (parent=%s), curDepth=%d",
                                                pushedItem.name.getString(),
                                                m_curDir.name.getString(),
                                                curDepth ) ;
                    }
                }

                // The current entry is a file
                else
                {
                    if ( ( !m_filesOnly && !m_dirsOnly ) || m_filesOnly )
                    {
                        if ( callback.item( m_curDir.name, m_curFsEntry, cbInfo ) == Kit::Type::TraverserStatus::eABORT )
                        {
                            // TODO: Close DIR on error
                            return false;
                        }
                    }
                }
            }
            while ( DirListHal<DIRLIST_HDL>::getNextEntry( hdl, m_curFsEntry, sizeof( m_curFsEntry ) ) );

            // Housekeeping
            DirListHal<DIRLIST_HDL>::close( hdl );
        }

        return true;
    }

protected:
    /// The current directory being processed
    DirDepth_T m_curDir;

    /// Stack of directory names (so we don't use recursion)
    Kit::Container::Stack<DirDepth_T> m_stack;

    /// Maximum depth of directory traversal
    unsigned m_depth;

    /// Memory for the stack
    DirDepth_T m_stackMemory[OPTION_KIT_IO_FILE_DIRLIST_MAX_DEPTH];

    /// Memory for the current file/dir entry (include space for the null terminator)
    char m_curFsEntry[Kit::Io::File::maxName + 1];

    /// Filter flag for files only
    bool m_filesOnly;

    /// Filter flag for directories only
    bool m_dirsOnly;
};

}  // end namespaces
}
}
#endif  // end header latch