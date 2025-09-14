#ifndef KIT_IO_STDIO_STDERR_H_
#define KIT_IO_STDIO_STDERR_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Io/Stdio/OutputDelegate.h"

///
namespace Kit {
///
namespace Io {
///
namespace Stdio {


/** This concrete class represents the C-library 'stderr' stream.
    NOTE: Many instances of this class may be created - even
          though there is only ONE stderr.  This is OK because
          all instance reference the systems/platform 'stderr'
 */
class StdErr : public OutputDelegate
{
public:
    /// Constructor
    StdErr();
};

}  // end namespaces
}
}
#endif  // end header latch
