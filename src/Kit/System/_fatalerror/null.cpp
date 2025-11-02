/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file

    A null implementation of the Fatal Error Handler

*/

#include "Kit/System/FatalError.h"
#include "Kit/System/Shutdown.h"

using namespace Kit::System;


////////////////////////////////////////////////////////////////////////////////
void FatalError::log( int exitCode, const char* message )
{
    Shutdown::failure( exitCode );
}

void FatalError::log( int exitCode, const char* message, size_t value )
{

    Shutdown::failure( exitCode );
}


void FatalError::logf( int exitCode, const char* format, ... )
{
    Shutdown::failure( exitCode );
}


////////////////////////////////////////////////////////////////////////////////
void FatalError::logRaw( int exitCode, const char* message )
{
    Shutdown::failure( exitCode );
}

void FatalError::logRaw( int exitCode, const char* message, size_t value )
{
    Shutdown::failure( exitCode );
}