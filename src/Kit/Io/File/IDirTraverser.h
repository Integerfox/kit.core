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

#include "Kit/Type/TraverserStatus.h"
#include "Kit/Io/File/System.h"


///
namespace Kit {
///
namespace Io {
///
namespace File {

/** This abstract class defines the client interface for walking
    the contents of a directory, i.e. defines the callback method for
    when walking/traversing the entries in a directory
 */
class IDirTraverser
{
public:
    /** This method is called once for ever item in the "traversee's"
        list.  The return code from the method is used by the traverser
        to continue the traversal (eCONTINUE), or abort the traversal
        (eABORT).
     */
    virtual Kit::Type::TraverserStatus item( const char*     currentParent,
                                             const char*     fsEntryName,
                                             System::Info_T& entryInfo ) = 0;
public:
    /// Virtual Destructor
    virtual ~IDirTraverser() = default;
};

}  // end namespaces
}
}
#endif  // end header latch