#ifndef KIT_IO_STDIO_STDIN_H_
#define KIT_IO_STDIO_STDIN_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Io/Stdio/InputDelegate.h"

///
namespace Kit {
///
namespace Io {
///
namespace Stdio {


/** This concrete class represents the C-library 'stdin' stream.

    NOTE: Many instances of this class may be created - even
          though there is only ONE stdin.  This is OK because
          all instance reference the systems/platform 'stdin'

    NOTE: stdin under Windoze is problematic with how the available() method is
          implemented.  See the _win32/fdio.h file for more details.  There
          is partial work-around for the issue.  It can be enabled be defining
          the following in kit_confi.h: USE_KIT_IO_STDIO_WIN32_STDIN_CONSOLE_HACK
 */
class StdIn : public InputDelegate
{
public:
    /// Constructor
    StdIn();
};

}  // end namespaces
}
}
#endif  // end header latch
