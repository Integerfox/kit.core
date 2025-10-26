/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */


#include "Cpl/System/Shell.h"


bool Cpl::System::Shell::isAvailable()
{
    return false;
}


int Cpl::System::Shell::execute( const char* cmdstring, bool noEchoStdOut, bool noEchoStdErr )
{
    return -1;
}





