/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */


#include "Kit/System/Shell.h"

//------------------------------------------------------------------------------
namespace Kit {
namespace System {

bool Shell::isAvailable() noexcept
{
    return false;
}


int Shell::execute( const char* cmdstring, bool noEchoStdOut, bool noEchoStdErr ) noexcept
{
    return -1;
}

} // end namespace
}
//------------------------------------------------------------------------------
