/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Threads.h"
#include <windows.h>

//------------------------------------------------------------------------------
namespace Kit {
namespace TShell {
namespace Command {
namespace Win32 {

void Threads::hookHeaderTitle( Kit::Text::IString& text )
{
    text.formatAppend( "  %-3s", "Pri" );
}

void Threads::hookThreadEntry( Kit::Text::IString& text, Kit::System::Thread& currentThread )
{
    HANDLE threadHdl = currentThread.getId();
    int    priority  = GetThreadPriority( threadHdl );

    text.formatAppend( "  %-3d", priority );
}

}  // end namespaces
}
}
}
//------------------------------------------------------------------------------
