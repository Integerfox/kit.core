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
#include <pthread.h>
#include <time.h>

//------------------------------------------------------------------------------
namespace Kit {
namespace TShell {
namespace Command {
namespace Posix {

void Threads::hookHeaderTitle( Kit::Text::IString& text )
{
    text.formatAppend( "%-6s  %-3s  %-10s", "Policy", "Pri", "CPU(s)" );
}

void Threads::hookThreadEntry( Kit::Text::IString& text, Kit::System::Thread& currentThread )
{
    int                policy;
    struct sched_param param;
    pthread_t          threadId = currentThread.getId();

    pthread_getschedparam( threadId, &policy, &param );

    const char* policyStr = "UNK";
    switch ( policy )
    {
    case SCHED_OTHER: policyStr = "OTHER"; break;
    case SCHED_FIFO: policyStr = "FIFO"; break;
    case SCHED_RR: policyStr = "RR"; break;
    }

    clockid_t       threadCpuClock;
    struct timespec cpuTime = {};
    pthread_getcpuclockid( threadId, &threadCpuClock );
    clock_gettime( threadCpuClock, &cpuTime );

    double cpuTimeSec = static_cast<double>( cpuTime.tv_sec );
    cpuTimeSec += static_cast<double>( cpuTime.tv_nsec ) / 1000000000.0;

    text.formatAppend( "%-6s  %-3d  %-10.4f", policyStr, param.sched_priority, cpuTimeSec );
}

}  // end namespaces
}
}
}
//------------------------------------------------------------------------------
