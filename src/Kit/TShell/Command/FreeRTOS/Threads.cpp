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
#include "task.h"

//------------------------------------------------------------------------------
namespace Kit {
namespace TShell {
namespace Command {
namespace FreeRTOS {

static inline const char* stateToString( eTaskState state )
{
    switch ( state )
    {
    case eRunning:
        return "RUN";
    case eReady:
        return "READY";
    case eBlocked:
        return "BLOCK";
    case eSuspended:
        return "SUSP";
    case eDeleted:
        return "DEL";
    default:
        return "UNK";
    }
}

void Threads::hookHeaderTitle( Kit::Text::IString& text )
{
    text.formatAppend( "  %-4s  %-5s  %-5s", "Pri", "State", "Stack" );
}

void Threads::hookThreadEntry( Kit::Text::IString& text, Kit::System::Thread& currentThread )
{
    TaskHandle_t taskHdl        = currentThread.getId();
    unsigned     priority       = static_cast<unsigned>( uxTaskPriorityGet( taskHdl ) );
    const char*  stateStr       = stateToString( eTaskGetState( taskHdl ) );
    unsigned     stackHighWater = static_cast<unsigned>( uxTaskGetStackHighWaterMark( taskHdl ) );

    text.formatAppend( "  %-4u  %-5s  %-5u", priority, stateStr, stackHighWater );
}

}  // end namespaces
}
}
}
//------------------------------------------------------------------------------
