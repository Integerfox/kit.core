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

static inline unsigned long long fileTimeTo100ns( const FILETIME& timeVal )
{
    return ( static_cast<unsigned long long>( timeVal.dwHighDateTime ) << 32 ) |
           static_cast<unsigned long long>( timeVal.dwLowDateTime );
}

static inline void toDurationFields( unsigned long long time100ns,
                                     unsigned long long& days,
                                     unsigned long long& hours,
                                     unsigned long long& minutes,
                                     unsigned long long& seconds,
                                     unsigned long long& milliseconds )
{
    const unsigned long long totalMilliseconds = time100ns / 10000ULL;

    days         = totalMilliseconds / 86400000ULL;
    hours        = ( totalMilliseconds / 3600000ULL ) % 24ULL;
    minutes      = ( totalMilliseconds / 60000ULL ) % 60ULL;
    seconds      = ( totalMilliseconds / 1000ULL ) % 60ULL;
    milliseconds = totalMilliseconds % 1000ULL;
}


void Threads::hookHeaderTitle( Kit::Text::IString& text )
{
    text.formatAppend( "%-4s  %-15s  %-15s", "Pri", "User(dd:hh:mm:ss:ms)", "Kernel(dd:hh:mm:ss:ms)" );
}

void Threads::hookThreadEntry( Kit::Text::IString& text, Kit::System::Thread& currentThread )
{
    HANDLE threadHdl = static_cast<HANDLE>( currentThread.getId() );

    int priority = GetThreadPriority( threadHdl );

    FILETIME createTime = {};
    FILETIME exitTime   = {};
    FILETIME kernelTime = {};
    FILETIME userTime   = {};
    GetThreadTimes( threadHdl, &createTime, &exitTime, &kernelTime, &userTime );

    unsigned long long userDays;
    unsigned long long userHours;
    unsigned long long userMinutes;
    unsigned long long userSeconds;
    unsigned long long userMilliseconds;
    toDurationFields( fileTimeTo100ns( userTime ), userDays, userHours, userMinutes, userSeconds, userMilliseconds );

    unsigned long long kernelDays;
    unsigned long long kernelHours;
    unsigned long long kernelMinutes;
    unsigned long long kernelSeconds;
    unsigned long long kernelMilliseconds;
    toDurationFields( fileTimeTo100ns( kernelTime ), kernelDays, kernelHours, kernelMinutes, kernelSeconds, kernelMilliseconds );

    text.formatAppend( "%-4d  %02llu:%02llu:%02llu:%02llu:%03llu  %02llu:%02llu:%02llu:%02llu:%03llu",
                       priority,
                       userDays,
                       userHours,
                       userMinutes,
                       userSeconds,
                       userMilliseconds,
                       kernelDays,
                       kernelHours,
                       kernelMinutes,
                       kernelSeconds,
                       kernelMilliseconds );
}

}  // end namespaces
}
}
}
//------------------------------------------------------------------------------
