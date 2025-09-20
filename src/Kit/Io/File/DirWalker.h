#ifndef KIT_IO_FILE_DIR_WALKER_H_
#define KIT_IO_FILE_DIR_WALKER_H_
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
#include "Kit/Io/File/System.h"
#include "Kit/Io/File/IDirTraverser.h"
#include "Kit/Container/Stack.h"


/** The maximum supported directory depth */
#ifndef OPTION_KIT_IO_FILE_DIRWALKER_MAX_DEPTH
#define OPTION_KIT_IO_FILE_DIRWALKER_MAX_DEPTH 4
#endif


///
namespace Kit {
///
namespace Io {
///
namespace File {


/** This concrete class is a platform independent class that traverse a parent
    directory and optionally 'walks' the parent's child directories.  What is
    'different' about this class - is that it does NOT use recursion to walk the
    child directories.

    NOTE: For an embedded system, this class can have a 'large' memory footprint.
          The recommendation is to NOT allocate this class on the stack.  For
          truly constrained systems use the System::getFirstDirEntry(), 
          getFirstDirEntry, and closeDirectory() methods directly
 */
class DirWalker
{
public:
    /// Constructor.
    DirWalker() noexcept
        : m_curDir( "" )
        , m_stack( OPTION_KIT_IO_FILE_DIRWALKER_MAX_DEPTH, m_stackMemory )
    {
    }


public:
    /** This method allows the caller to walk the contents of the 'dirToList'
        directory (assuming the entry is a directory).  The default behavior is
        to list only the current directory.  The Caller can override the depth
        of the traversal.  Also, by default only the file name without its path
        is returned.  The caller can optionally have the traverse call only
        return the name of files (omitting any directory names found) OR names
        of directories (omitting any file names found).  Returns true when
        successfully and the entire traversal complete; else false is return if
        their is an error (e.g. 'dirToList' is a file or does not exist) or the
        'walker' aborted the traversal.

        NOTE: The is no guaranteed order to the traversal, only that all items
              per the specified depth will be traversed.

        WARNING: On most platforms, the underlying OS calls dynamically
                 allocate memory. The OS clean-up after itself but there still
                 is the risk of failure do to lack of memory.  Memory failures
                 are handled silently - but will not crash traversal.
     */
    bool traverse( const char*    dirToList,
                   IDirTraverser& callback,
                   unsigned       depth     = 1,
                   bool           filesOnly = false,
                   bool           dirsOnly  = false ) noexcept;
protected:
    /// Helper method
    void ensureTrailingDirSep( NameString& name ) noexcept;

protected:
    /// Tracks depth of directories found during a traversal
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

protected:
    /// The current directory being processed
    DirDepth_T m_curDir;

    /// Stack of directory names (so we don't use recursion)
    Kit::Container::Stack<DirDepth_T> m_stack;

    /// Memory for the stack
    DirDepth_T m_stackMemory[OPTION_KIT_IO_FILE_DIRWALKER_MAX_DEPTH];

    /// Temporary/work variable
    NameString m_wrkEntryName;

    /// Memory for the current file/dir entry (include space for the null terminator)
    NameString m_curFsEntry;
};

}  // end namespaces
}
}
#endif  // end header latch