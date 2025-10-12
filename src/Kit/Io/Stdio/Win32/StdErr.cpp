/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Io/Stdio/StdErr.h"

//
using namespace Kit::Io::Stdio;

StdErr::StdErr()
    :OutputDelegate( GetStdHandle( STD_ERROR_HANDLE ) )
{
}
