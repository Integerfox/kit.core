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

#include "kit_config.h"
#include "kit_map.h"
#include "Kit/Type/TraverserStatus.h"
#include "Kit/Io/File/System.h"
#include "Kit/Io/File/_directory/DirListHal.h"
#include "Kit/Container/Stack.h"
#include "Kit/System/Assert.h"
#include "Kit/System/Trace.h"
#include "Kit/Text/Strip.h"
#include <dirent.h>
#include <string.h>


/** Defer the HAL type to the platform */
#define KitIoFileDirListHal_T KitIoFileDirListHal_T_MAP

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
/** This concrete class is a platform independent class that traverse a parent
    directory and optionally 'walks' the parent's child directories.  What is
    'different' about this class - is that it does NOT use recursion to walk the
    child directories.

    The platform specific operation are provided by the DirListHal interface.

    NOTE: For an embedded system, this class can have a 'large' memory footprint.
          The recommendation is to NOT allocate this class on the stack.  For
          truly constrained systems, setting OPTION_KIT_IO_FILE_DIRLIST_MAX_DEPTH
          to 1 will minimize the memory footprint (but still do NOT allocate
          on the stack). However, this will limit the traversal to only a single
          directory level.
 */
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
    /// Constructor.  When using this constructor, the caller must call reset() before using the instance
    DirList() noexcept
        : m_curDir( "" )
        , m_stack( OPTION_KIT_IO_FILE_DIRLIST_MAX_DEPTH, m_stackMemory )
    {
    }

    /// Constructor
    DirList( const char* rootDir,
             int         depth     = 1,
             bool        filesOnly = false,
             bool        dirsOnly  = false ) noexcept
        : m_stack( OPTION_KIT_IO_FILE_DIRLIST_MAX_DEPTH, m_stackMemory )
    {
        reset( rootDir, depth, filesOnly, dirsOnly );
    }

public:
    /// This method 'reset's the instance so that it can be reused (e.g. can be statically allocated)
    void reset( const char* rootDir,
                int         depth     = 1,
                bool        filesOnly = false,
                bool        dirsOnly  = false ) noexcept
    {
        KIT_SYSTEM_ASSERT( rootDir != nullptr );
        m_stack.clearTheStack();
        m_curDir.name     = rootDir;
        m_curDir.curDepth = 1;
        m_filesOnly       = filesOnly;
        m_dirsOnly        = dirsOnly;
        m_depth           = depth;
        if ( m_depth > OPTION_KIT_IO_FILE_DIRLIST_MAX_DEPTH )
        {
            m_depth = OPTION_KIT_IO_FILE_DIRLIST_MAX_DEPTH;
        }
    }

public:
    /// See Kit::Io::File::System::walkDirectory
    bool traverse( System::DirectoryWalker& callback ) noexcept
    {
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
            KIT_SYSTEM_TRACE_MSG( MYSECT_, "curDir=%s, curDepth=%d", m_curDir.name.getString(), curDepth );

            // Open the current directory
            KitIoFileDirListHal_T hdl;
            if ( DirListHal::getFirstEntry( hdl, m_curDir.name(), m_curFsEntry, sizeof( m_curFsEntry ) ) == false )
            {
                return false;
            }

            // Read the content's of the directory
            while ( m_curFsEntry[0] != '\0' )
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
                        DirListHal::close( hdl );
                        return false;
                    }
                    else
                    {
                        // The current entry is a directory
                        if ( cbInfo.m_isDir )
                        {
                            // Callback the client
                            if ( ( !m_filesOnly && !m_dirsOnly ) || m_dirsOnly )
                            {
                                if ( callback.item( m_curDir.name, m_curFsEntry, cbInfo ) == Kit::Type::TraverserStatus::eABORT )
                                {
                                    DirListHal::close( hdl );
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
                                                  m_depth );
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
                                ensureTrailingDirSep( pushedItem.name );
                                pushedItem.name     += m_curFsEntry;
                                pushedItem.curDepth  = curDepth;
                                KIT_SYSTEM_TRACE_MSG( MYSECT_,
                                                      "\nPUSHED dir=%s (parent=%s), curDepth=%d",
                                                      pushedItem.name.getString(),
                                                      m_curDir.name.getString(),
                                                      curDepth );
                            }
                        }

                        // The current entry is a file
                        else
                        {
                            if ( ( !m_filesOnly && !m_dirsOnly ) || m_filesOnly )
                            {
                                if ( callback.item( m_curDir.name, m_curFsEntry, cbInfo ) == Kit::Type::TraverserStatus::eABORT )
                                {
                                    DirListHal::close( hdl );
                                    return false;
                                }
                            }
                        }
                    }

                    // Get the next entry in the directory
                    if ( DirListHal::getNextEntry( hdl, m_curFsEntry, sizeof( m_curFsEntry ) ) == false )
                    {
                        // Error reading the directory
                        DirListHal::close( hdl );
                        return false;
                    }
                }
            }

            // Close the current directory
            DirListHal::close( hdl );
        }

        return true;
    }

protected:
    /// Helper method
    inline void ensureTrailingDirSep( NameString& name ) noexcept
    {
        // Ensure my current root ends with a trailing dir separator
        if ( name[name.length() - 1] != dirSep )
        {
            name += dirSep;
        }
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

    /// Temporary/work variable
    NameString m_wrkEntryName;

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