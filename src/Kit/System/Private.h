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
#include "Kit/System/BSemaMutex.h"
#include "Kit/System/Mutex.h"
#include "Kit/System/Trace.h"
#include <new>
#include <type_traits>


/// 
namespace Kit {
/// 
namespace System {


/** Holds a lazily constructed, never destroyed instance of T.

    Declare instances as function-local statics. Construction-on-first-use
    removes the static initialization order dependency for globals that are
    used from other translation units' static constructors.

    This class MUST remain trivially destructible (see the static_assert
    below) - a non-trivial destructor on a function-local static is registered
    via atexit()/__cxa_atexit(), which pulls newlib's exit machinery (and the
    undefined '_fini') into the bare-metal targets.
 */
template <class T>
class NeverDestroyed
{
public:
    ///
    NeverDestroyed() noexcept
    {
        static_assert( std::is_trivially_destructible<NeverDestroyed>::value,
                       "NeverDestroyed<T> must stay trivially destructible - do NOT add a destructor" );
        ::new ( m_storage ) T();
    }

    /// Returns the wrapped instance
    T& get() noexcept { return *reinterpret_cast<T*>( m_storage ); }

private:
    alignas( T ) unsigned char m_storage[sizeof( T )];
};


/** This class provide access to KIT Library's internal global mutexes.
 */
class PrivateLocks
{
public:
    /// Global mutex for use for interfaces/class in the Kit::System namespace
    static BSemaMutex& system() noexcept;

    /** Same as 'system', except this mutex is REQUIRED to have recursive
        semantics, i.e. it is for critical sections that can nest and/or that
        call out to client/application code while holding the lock.
     */
    static Mutex& systemRecursive() noexcept;

    /** Global mutex for use for tracing sub-system.  This mutex is to protect 
        the Trace engine's internal/meta data.  This mutex is NOT used to guard
        the Trace output.  The Trace engine uses two mutex so that its output
        can be 'serialized' with the TShell's output (or other sub-systems) BUT
        not cause high priority threads to block on the trace output when their
        trace statements are not enabled at run time.
     */
    static BSemaMutex& tracing() noexcept;

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
    static void appendInfo( Kit::Text::IString& dst, TraceLevel info, const char* section, const char* filename, int linenum, const char* funcname ) noexcept;

    /** This method output the string to the logging destination/stream/media
     */
    static void output( Kit::Text::IString& src ) noexcept;
};


}       // end namespaces
}
#endif  // end header latch

