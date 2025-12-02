#ifndef KIT_SYSTEM_TRACE_H_
#define KIT_SYSTEM_TRACE_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file

    This file provides an interface for a 'printf' tracing mechanism. The output
    of tracing messages typically defaults (dependent on the target platform) to
    stdout.  However, the output can be redirected to any Kit::Stream::Output
    instance.

    The trace engine is thread safe, i.e. it guarantees that outputting a single
    trace message (info + user msg) is an Atomic operation within a multi-thread
    environment.

    All of the methods SHOULD be accessed via the preprocessor macros below to
    allow the 'trace code' to be compiled out of a 'release' build if desired.
    In addition to a compile time decision to use/exclude tracing, there are
    run-time switches to selective enable and/or control what is displayed.

    There are numerous application customizable (at compile time) options
    for how the internals of how the trace engine works (see OPTION_xxx
    macros below).

    NOTE: The trace engine supports does NOT support tracing BEFORE the KIT
          library is initialized.  This means that constructors of objects that
          are created statically can NOT contain trace logging calls.

    <pre>

    For trace message to appear on the logging media the following
    must done:

        1. Enable Trace at compile time by defining the USE_KIT_SYSTEM_TRACE
           preprocessor symbol.
        2. Enable the Trace engine at run time by calling KIT_SYSTEM_TRACE_ENABLE()
        3. Enable at least one 'section' by calling KIT_SYSTEM_TRACE_ENABLE_SECTION()



    How to use the KIT_SYSTEM_TRACE_xxx macros
    ------------------------------------------
    The trace functionality is provided by the KIT_SYSTEM_TRACE and
    KIT_SYSTEM_TRACEMSG macros.  The remaining macros are used to enable/disable
    and control the actual trace output.  If the symbol, USE_KIT_SYSTEM_TRACE,
    is NOT defined at compile time -- all of the trace macros are compiled
    out.  If the symbol is defined, then the trace output can be enabled
    and disabled at run time.

    KIT_SYSTEM_TRACE_FUNC() - This macro is used to trace the entry and exit of a
                              function.
                              For example:
                                  void foobar( int x )
                                  {
                                      // Traces the entry & exit of foobar() when tracing is enabled for section "bob"
                                      KIT_SYSTEM_TRACE_FUNC("bob");
                                      int y = 2*x;
                                      ...
                                  }

    KIT_SYSTEM_TRACE_MSG() - This macro is for the programmer/application to
                             generate formatted message that are outputted to the
                             "trace media." The macro has printf() semantics.

                             For example:
                                 void foobar( int x )
                                 {
                                     ...
                                     // Trace message outputs when tracing is enabled for section "bob"
                                     KIT_SYSTEM_TRACE_MSG( "bob, "This is an example Trace Msg. Y=%d, X=%d", y, x);
                                     ...
                                 }
</pre>
*/

#include "kit_config.h"
#include "Kit/Text/IString.h"
#include "Kit/System/printfchecker.h"
#include "Kit/Io/IOutput.h"


//////////////////////////////////////////////////////////////////////////////
#if defined( USE_KIT_SYSTEM_RESTRICTED_TRACE ) || defined( USE_KIT_SYSTEM_TRACE )

/** This is SPECIAL macro wrapper in that it NOT compiled out when USE_KIT_SYSTEM_TRACE
    is NOT defined.  The use case for this is when the Application needs to
    disable - at compile time - the trace statements because the memory footprint
    of all of the trace strings is 'too expensive' for the target.  However,
    there are situations where 'some' trace statements are needed (e.g. when the
    logging engine echos log entries to the trace engine, or when error occurs
    bring up the logging sub-system's persistent storage media).

    WARNING: Use this macro sparingly as it negates the purpose of being able
             to compile out the trace statements. If you are not sure if you
             should use this macro, you probably should NOT be using it!
 */
#define KIT_SYSTEM_TRACE_RESTRICTED_MSG( sect, ... )                                                              \
    do                                                                                                        \
    {                                                                                                         \
        if ( Kit::System::Trace::isSectionEnabled_( sect ) && Kit::System::Trace::passedThreadFilter_() )     \
        {                                                                                                     \
            Kit::System::Trace::traceLocation_( sect, __FILE__, __LINE__, KIT_SYSTEM_TRACE_PRETTY_FUNCNAME ); \
            Kit::System::Trace::traceUserMsg_( __VA_ARGS__ );                                                 \
        }                                                                                                     \
    }                                                                                                         \
    while ( 0 )

/// 'Restricted' version of the ALLOCATE macro.  Read the above comments before using!
#define KIT_SYSTEM_TRACE_RESTRICTED_ALLOCATE( type, varname, initval ) type varname = initval


// PRETTY_FUNCTION macro is non-standard
#if defined( __GNUC__ )
/// Take advantage of GCC's pretty function symbol
#define KIT_SYSTEM_TRACE_PRETTY_FUNCNAME __PRETTY_FUNCTION__

#elif defined( _MSC_VER )
/// Take advantage of Microsoft's pretty function symbol
#define KIT_SYSTEM_TRACE_PRETTY_FUNCNAME __FUNCSIG__

#else
/// Use C++11 function name
#define KIT_SYSTEM_TRACE_PRETTY_FUNCNAME __func__
#endif  // end __PRETTY_FUNCTION__


#ifdef USE_KIT_SYSTEM_TRACE
/// Macro Wrapper
#define KIT_SYSTEM_TRACE_FUNC( sect ) Kit::System::Trace kitSystemTraceInstance_( __FILE__, __LINE__, KIT_SYSTEM_TRACE_PRETTY_FUNCNAME, sect, KIT_SYSTEM_TRACE_PRETTY_FUNCNAME )

/// Macro Wrapper
#define KIT_SYSTEM_TRACE_SCOPE( sect, label ) Kit::System::Trace kitSystemTraceInstance_( __FILE__, __LINE__, KIT_SYSTEM_TRACE_PRETTY_FUNCNAME, sect, label )

/// Macro Wrapper
#define KIT_SYSTEM_TRACE_MSG( sect, ... )                                                                     \
    do                                                                                                        \
    {                                                                                                         \
        if ( Kit::System::Trace::isSectionEnabled_( sect ) && Kit::System::Trace::passedThreadFilter_() )     \
        {                                                                                                     \
            Kit::System::Trace::traceLocation_( sect, __FILE__, __LINE__, KIT_SYSTEM_TRACE_PRETTY_FUNCNAME ); \
            Kit::System::Trace::traceUserMsg_( __VA_ARGS__ );                                                 \
        }                                                                                                     \
    }                                                                                                         \
    while ( 0 )
#endif // end USE_KIT_SYSTEM_TRACE

/// Macro Wrapper
#define KIT_SYSTEM_TRACE_ENABLE() Kit::System::Trace::enable_()

/// Macro Wrapper
#define KIT_SYSTEM_TRACE_DISABLE() Kit::System::Trace::disable_()

/// Macro Wrapper
#define KIT_SYSTEM_TRACE_IS_ENABLED() Kit::System::Trace::isEnabled_()

/// Macro Wrapper
#define KIT_SYSTEM_TRACE_IS_SECTION_ENABLED( sect ) Kit::System::Trace::isSectionEnabled_( sect )

/// Macro Wrapper
#define KIT_SYSTEM_TRACE_SET_INFO_LEVEL( l ) Kit::System::Trace::setInfoLevel_( l )

/// Macro Wrapper
#define KIT_SYSTEM_TRACE_GET_INFO_LEVEL() Kit::System::Trace::getInfoLevel_()

/// Macro Wrapper
#define KIT_SYSTEM_TRACE_ENABLE_SECTION( sect ) Kit::System::Trace::enableSection_( sect )

/// Macro Wrapper
#define KIT_SYSTEM_TRACE_DISABLE_SECTION( sect ) Kit::System::Trace::disableSection_( sect )

/// Macro Wrapper
#define KIT_SYSTEM_TRACE_GET_SECTIONS( dst ) Kit::System::Trace::getSections_( dst )

/// Macro Wrapper
#define KIT_SYSTEM_TRACE_SET_THREAD_FILTER( t1 ) Kit::System::Trace::setThreadFilter_( t1 )

/// Macro Wrapper
#define KIT_SYSTEM_TRACE_SET_THREAD_2FILTERS( t1, t2 ) Kit::System::Trace::setThreadFilter_( t1, t2 )

/// Macro Wrapper
#define KIT_SYSTEM_TRACE_SET_THREAD_3FILTERS( t1, t2, t3 ) Kit::System::Trace::setThreadFilter_( t1, t2, t3 )

/// Macro Wrapper
#define KIT_SYSTEM_TRACE_SET_THREAD_4FILTERS( t1, t2, t3, t4 ) Kit::System::Trace::setThreadFilter_( t1, t2, t3, t4 )

/// Macro Wrapper
#define KIT_SYSTEM_TRACE_GET_THREAD_FILTERS( dst ) Kit::System::Trace::getThreadFilters_( dst )

/// Macro Wrapper
#define KIT_SYSTEM_TRACE_CLEAR_THREAD_FILTER() Kit::System::Trace::clearThreadFilter_()

/// Macro Wrapper
#define KIT_SYSTEM_TRACE_DISABLE_SECTION( sect ) Kit::System::Trace::disableSection_( sect )

/// Macro Wrapper
#define KIT_SYSTEM_TRACE_REDIRECT( newdst ) Kit::System::Trace::redirect_( newdst )

/// Macro Wrapper
#define KIT_SYSTEM_TRACE_REVERT() Kit::System::Trace::revert_()

/// Returns true if TRACE was activated/enabled at compile time
#define KIT_SYSTEM_TRACE_IS_COMPILED() true

/// Allocate a variable that is only 'used' when tracing is enabled
#define KIT_SYSTEM_TRACE_ALLOCATE( type, varname, initval ) type varname = initval

#else

/// Macro Wrapper
#define KIT_SYSTEM_TRACE_RESTRICTED_MSG( sect, ... )

/// Macro Wrapper
#define KIT_SYSTEM_TRACE_RESTRICTED_ALLOCATE( type, varname, initval )

/// Macro Wrapper
#define KIT_SYSTEM_TRACE_FUNC( sect )

/// Macro Wrapper
#define KIT_SYSTEM_TRACE_SCOPE( sect, label )

/// Macro Wrapper
#define KIT_SYSTEM_TRACE_MSG( sect, ... )

/// Macro Wrapper
#define KIT_SYSTEM_TRACE_ENABLE()

/// Macro Wrapper
#define KIT_SYSTEM_TRACE_DISABLE()

/// Macro Wrapper
#define KIT_SYSTEM_TRACE_IS_ENABLED()               false

/// Macro Wrapper
#define KIT_SYSTEM_TRACE_IS_SECTION_ENABLED( sect ) false

/// Macro Wrapper
#define KIT_SYSTEM_TRACE_SET_INFO_LEVEL( l )

/// Macro Wrapper
#define KIT_SYSTEM_TRACE_GET_INFO_LEVEL() ( (Kit::System::Trace::InfoLevel_T)0 )

/// Macro Wrapper
#define KIT_SYSTEM_TRACE_ENABLE_SECTION( sect )

/// Macro Wrapper
#define KIT_SYSTEM_TRACE_DISABLE_SECTION( sect )

/// Macro Wrapper
#define KIT_SYSTEM_TRACE_SET_THREAD_FILTER( t1 )

/// Macro Wrapper
#define KIT_SYSTEM_TRACE_SET_THREAD_2FILTERS( t1, t2 )

/// Macro Wrapper
#define KIT_SYSTEM_TRACE_SET_THREAD_3FILTERS( t1, t2, t3 )

/// Macro Wrapper
#define KIT_SYSTEM_TRACE_SET_THREAD_4FILTERS( t1, t2, t3, t4 )

/// Macro Wrapper
#define KIT_SYSTEM_TRACE_GET_THREAD_FILTERS( dst ) 0

/// Macro Wrapper
#define KIT_SYSTEM_TRACE_CLEAR_THREAD_FILTER()

/// Macro Wrapper
#define KIT_SYSTEM_TRACE_GET_SECTIONS( dst ) 0

/// Macro Wrapper
#define KIT_SYSTEM_TRACE_REDIRECT( newdst )

/// Macro Wrapper
#define KIT_SYSTEM_TRACE_REVERT()

/// Returns false if TRACE was NOT activated/enabled at compile time
#define KIT_SYSTEM_TRACE_IS_COMPILED() false

/// Macro Wrapper
#define KIT_SYSTEM_TRACE_ALLOCATE( type, varname, initval )

#endif  // end USE_KIT_SYSTEM_TRACE || USE_KIT_SYSTEM_RESTRICTED_TRACE


//////////////////////////////////////////////////////////////////////////////
/** The default maximum number of actively enabled 'sections' supported.
 */
#ifndef OPTION_KIT_SYSTEM_TRACE_MAX_SECTIONS
#define OPTION_KIT_SYSTEM_TRACE_MAX_SECTIONS 8
#endif

/** The default maximum size, in bytes, for a section name (not including
    the null terminator).
 */
#ifndef OPTION_KIT_SYSTEM_TRACE_MAX_SECTION_NAME_LEN
#define OPTION_KIT_SYSTEM_TRACE_MAX_SECTION_NAME_LEN 63
#endif

/** The size, in bytes (without the null terminator), of trace engine's internal
    String use to construct a complete (info + user msg) trace message.
 */
#ifndef OPTION_KIT_SYSTEM_TRACE_MAX_BUFFER
#define OPTION_KIT_SYSTEM_TRACE_MAX_BUFFER 511
#endif


/** String literal that is the prefix for all trace messages
 */
#ifndef OPTION_KIT_SYSTEM_TRACE_PREFIX_STRING
#define OPTION_KIT_SYSTEM_TRACE_PREFIX_STRING ">> "
#endif


/** String literal that is the final suffix for all trace messages
 */
#ifndef OPTION_KIT_SYSTEM_TRACE_SUFFIX_STRING
#define OPTION_KIT_SYSTEM_TRACE_SUFFIX_STRING "\n"
#endif

/** The compile time default setting for the info level
 */
#ifndef OPTION_KIT_SYSTEM_TRACE_DEFAULT_INFO_LEVEL
#define OPTION_KIT_SYSTEM_TRACE_DEFAULT_INFO_LEVEL Kit::System::Trace::eBRIEF
#endif

/** The compile time default setting what enable/disable state the trace
    starts in
 */
#ifndef OPTION_KIT_SYSTEM_TRACE_DEFAULT_ENABLE_STATE
#define OPTION_KIT_SYSTEM_TRACE_DEFAULT_ENABLE_STATE true
#endif


///
namespace Kit {
///
namespace System {

/// This concrete class provide a 'printf' tracing mechanism.
class Trace
{
public:
    /// Constructor
    Trace( const char* filename,
           int         linenum,
           const char* funcname,
           const char* section,
           const char* scope_name );

    /// Destructor
    ~Trace();

public:
    /** This enum is used to globally control the amount of 'Prologue' information
        that is prepended to the Trace log messages.  The specifics of what
        information is added is platform specific.
     */
    enum InfoLevel_T
    {
        eNONE = 0,  //!< Turns off ALL prologue info
        eBRIEF,     //!< Default setting
        eINFO,      //!< Additional prologue info
        eVERBOSE,   //!< more prologue info
        eMAX        //!< Maximum prologue info
    };

public:
    /** This function is used to trace the 'location' for general purpose trace
        messages.

        NOTE: NEVER call this method directly
     */
    static void traceLocation_( const char* section,
                                const char* filename,
                                int         linenum,
                                const char* funcname );

    /** This function is used to generate general purpose trace messages.

        NOTE: NEVER call this method directly
     */
    KIT_SYSTEM_PRINTF_CHECKER( 1, 2 )
    static void traceUserMsg_( const char* format, ... );

public:
    /** This method enables the output/logging of trace message at run-time.
        The default is start with tracing enabled.

        NOTE: NEVER call this method directly -->use the KIT_SYSTEM_TRACE_xxx()
              macros.
     */
    static void enable_();

    /** This method disables the output/logging of trace message at run-time.
        The default is start with tracing enabled.

        NOTE: NEVER call this method directly -->use the KIT_SYSTEM_TRACE_xxx()
              macros.
     */
    static void disable_();

    /** Returns true if tracing is enabled, i.e. enable_() has been called.,

        NOTE: NEVER call this method directly -->use the KIT_SYSTEM_TRACE_xxx()
              macros.
     */
    static bool isEnabled_();

    /** This method sets the information level for the trace messages. The method
        returns the previous level setting.

        NOTE: NEVER call this method directly -->use the KIT_SYSTEM_TRACE_xxx()
              macros.
     */
    static InfoLevel_T setInfoLevel_( InfoLevel_T newLevel );

    /** This method returns the current information level setting

        NOTE: NEVER call this method directly -->use the KIT_SYSTEM_TRACE_xxx()
              macros.
     */
    static InfoLevel_T getInfoLevel_() noexcept;

    /** This method enables the output/logging of trace message at run-time for
        the specified 'section'.  A 'section' name can NOT contain any white
        space.  The method returns true if the section was enabled; else false
        is return, i.e. exceeded max active enabled sections.

        NOTE: NEVER call this method directly -->use the KIT_SYSTEM_TRACE_xxx()
              macros.
     */
    static bool enableSection_( const char* sectionToEnable );

    /** This method disables the output/logging of trace message at run-time for
        the specified 'section'.  Setting  'sectionToDisable' to 0 will disable
        all sections.

        NOTE: NEVER call this method directly -->use the KIT_SYSTEM_TRACE_xxx()
              macros.
     */
    static void disableSection_( const char* sectionToDisable );

    /** Returns true if tracing is enabled AND the specified 'section' has been
        enabled.

        NOTE: NEVER call this method directly -->use the KIT_SYSTEM_TRACE_xxx()
              macros.
     */
    static bool isSectionEnabled_( const char* section );

    /** This method returns the number of enabled 'sections' and returns the
        actual section name(s) via the IString 'dst'.  It is the caller
        responsibility to ensure that 'dst' is large enough to hold all of the
        enable sections.  If 'dst' is to short, the results will be truncated.

        NOTE: NEVER call this method directly -->use the KIT_SYSTEM_TRACE_xxx()
              macros.
     */
    static unsigned getSections_( Kit::Text::IString& dst );

    /** This method applies a filter by thread name when generating output, i.e.
        for a message to be output, trace must be enabled, the message's section
        enabled, and must be running in the one of the threads specified
        by the thread filter.

        NOTE: NEVER call this method directly -->use the KIT_SYSTEM_TRACE_xxx()
              macros.
     */
    static void setThreadFilter_( const char* threadName1,
                                  const char* threadName2 = nullptr,
                                  const char* threadName3 = nullptr,
                                  const char* threadName4 = nullptr );

    /** This method returns the number of enabled 'thread filters' and returns
        the actual thread name(s) via the IString 'dst'.  It is the caller
        responsibility to ensure that 'dst' is large enough to hold all of the
        enable thread filters.  If 'dst' is to short, the results will be
        truncated.

        NOTE: NEVER call this method directly -->use the KIT_SYSTEM_TRACE_xxx()
              macros.
     */
    static unsigned getThreadFilters_( Kit::Text::IString& dst );

    /** This method removes  that thread filter (if there is one present).  If
        no filter has been applied when this method is called, then this
        method does nothing.

        NOTE: NEVER call this method directly -->use the KIT_SYSTEM_TRACE_xxx()
              macros.
     */
    static void clearThreadFilter_();

    /** This method returns true if the thread filter has been set and the
        current thread's name 'passes' the thread filter check, i.e. the specified
        name match one of the names in the filter list.

        NOTE: NEVER call this method directly -->use the KIT_SYSTEM_TRACE_xxx()
              macros.
     */
    static bool passedThreadFilter_();


public:
    /** This method is used to redirect the trace output to the provided Output
        stream. There is no guaranty on what happens to trace message(s) during
        the transition to the new output/media destination.

        NOTE: NEVER call this method directly -->use the KIT_SYSTEM_TRACE_xxx()
              macros.
     */
    static void redirect_( Kit::Io::IOutput& newMedia ) noexcept;

    /** This method is used to revert the trace output to its default output
        destination/stream/media. There is no guaranty on what happens to trace
        message(s) during the transition to the default output/media
        destination.

        NOTE: NEVER call this method directly -->use the KIT_SYSTEM_TRACE_xxx()
              macros.
     */
    static void revert_() noexcept;

public:
    /** This COMPONENT Scoped method provides the Output stream to the Trace
        Engine.  The Output stream instance MUST be created statically. However,
        the Output stream does not necessarily need to fully 'operational' when
        statically constructed - it just need to be  "operationally" by the time
        Kit::System::Api::initialize() is called.

        NOTE: The Application is RESPONSIBLE for implementing this method!
     */
    static Kit::Io::IOutput* getDefaultOutputStream_() noexcept;


protected:
    /// Caches trace info for exit message
    const char* m_filename;

    /// Caches trace info for exit message
    int m_linenum;

    /// Caches trace info for exit message
    const char* m_funcname;

    /// Caches trace info for exit message
    const char* m_section;

    /// Caches trace info for exit message
    const char* m_scope;
};

}
}  // end namespaces
#endif  // end header latch
