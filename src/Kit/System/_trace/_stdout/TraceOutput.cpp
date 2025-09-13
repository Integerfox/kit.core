/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/

// TODO: Implement tracing via Kit::Io::Output streams

 #if 0
#include "Kit/System/Trace.h"
#include "Kit/Io/Stdio/StdOut.h"


/// 
using namespace Kit::System;

///
static Kit::Io::Stdio::StdOut fd_;


////////////////////////////////////////////////////////////////////////////////
Kit::Io::Output* Trace::getDefaultOutputStream_() noexcept
{
    return &fd_;
}
#endif
