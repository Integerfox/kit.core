#ifndef KIT_SYSTEM_PRIVATE_H_
#define KIT_SYSTEM_PRIVATE_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file

    This file contains a collection of classes, interfaces, etc. that are
    are intended to be USE ONLY by Colony.Core sub-system - NOT by clients,
    consumers of the Colony.Core package.
*/

#include "Kit/Text/IString.h"
#include "Kit/System/Mutex.h"
#include "Kit/System/Trace.h"


/// 
namespace Kit {
/// 
namespace System {


/** This class provide access to KIT Library's internal global mutexes.
 */
class PrivateLocks
{
public:
    /// Global mutex for use for interfaces/class in the Kit::System namespace
    static Mutex& system() noexcept;

    /// Same as 'system', except usage is for 'long' critical sections (i.e. walking lists, printing, etc.)
    static Mutex& sysLists() noexcept;

    /** Global mutex for use for tracing sub-system.  This mutex is to protect 
        the Trace engine's internal/meta data.  This mutex is NOT used to guard
        the Trace output.  The Trace engine uses two mutex so that its output
        can be 'serialized' with the TShell's output (or other sub-systems) BUT
        not cause high priority threads to block on the trace output when their
        trace statements are not enabled at run time.
     */
    static Mutex& tracing() noexcept;

    /// Global mutex for use for the tracing sub-system OUTPUT (see above for more details)
    static Mutex& tracingOutput() noexcept;
};

/** This class defines the interface to the platform specific implementation of the
    Kit::System::Trace interface
 */
class PrivateTracePlatform
{
public:
    /** This method append the 'info' information to the provide String for the
        specified infoLevel.
     */
    static void appendInfo( Kit::Text::IString& dst, Trace::InfoLevel_T info, const char* section, const char* filename, int linenum, const char* funcname ) noexcept;

    /** This method output the string to the logging destination/stream/media
     */
    static void output( Kit::Text::IString& src ) noexcept;
};


}       // end namespaces
}
#endif  // end header latch

