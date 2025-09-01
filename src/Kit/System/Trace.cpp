/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Trace.h"
#include "Private.h"
#include "Kit/Text/FString.h"


//------------------------------------------------------------------------------
namespace Kit {
namespace System {

#define NUM_THREAD_FILTERS_ 4

typedef Kit::Text::FString<OPTION_KIT_SYSTEM_TRACE_MAX_SECTION_NAME_LEN> Section_T;

// TODO: Implement thread awareness
// static bool               threadFilterEnabled_ = false;
static bool               enabled_   = OPTION_KIT_SYSTEM_TRACE_DEFAULT_ENABLE_STATE;
static Trace::InfoLevel_T infoLevel_ = OPTION_KIT_SYSTEM_TRACE_DEFAULT_INFO_LEVEL;
static Section_T          activeSections_[OPTION_KIT_SYSTEM_TRACE_MAX_SECTIONS];
// TODO: Implement thread awareness
// static const char*        threadFilters_[NUM_THREAD_FILTERS_];

static Kit::Text::FString<OPTION_KIT_SYSTEM_TRACE_MAX_BUFFER> buffer_;


////////////////////////////////////////////////////////////////////////
Trace::Trace( const char* filename,
              int         linenum,
              const char* funcname,
              const char* section,
              const char* scope )
    : m_filename( filename )
    , m_linenum( linenum )
    , m_funcname( funcname )
    , m_section( section )
    , m_scope( scope )
{
    if ( isSectionEnabled_( section ) && passedThreadFilter_() )
    {
        traceLocation_( section, filename, linenum, funcname );
        traceUserMsg_( "->ENTER: %s", scope );
    }
}

Trace::~Trace()
{
    if ( isSectionEnabled_( m_section ) && passedThreadFilter_() )
    {
        traceLocation_( m_section, m_filename, m_linenum, m_funcname );
        traceUserMsg_( "->EXIT:  %s", m_scope );
    }
}


////////////////////////////////////////////////////////////////////////
// NOTE The following two methods MUST be called in order AND always
//      as pair!
//
void Trace::traceLocation_( const char* section, const char* filename, int linenum, const char* funcname )
{
    // Get the current tracing level parameter
    PrivateLocks::tracing().lock();
    Trace::InfoLevel_T infoLevel = infoLevel_;
    PrivateLocks::tracing().unlock();

    // Serialize the output
    PrivateLocks::tracingOutput().lock();
    buffer_ = OPTION_KIT_SYSTEM_TRACE_PREFIX_STRING;
    PrivateTracePlatform::appendInfo( buffer_, infoLevel, section, filename, linenum, funcname );
}


void Trace::traceUserMsg_( const char* format, ... )
{
    va_list ap;
    va_start( ap, format );
    buffer_.vformatAppend( format, ap );
    va_end( ap );

    buffer_ += OPTION_KIT_SYSTEM_TRACE_SUFFIX_STRING;

    // Ensure that the suffix is ALWAYS valid when appended (at the expense of truncating the user msg)
    if ( buffer_.truncated() )
    {
        buffer_.trimRight( strlen( OPTION_KIT_SYSTEM_TRACE_SUFFIX_STRING ) );
        buffer_ += OPTION_KIT_SYSTEM_TRACE_SUFFIX_STRING;
    }

    PrivateTracePlatform::output( buffer_ );
    PrivateLocks::tracingOutput().unlock();
}


////////////////////////////////////////////////////////////////////////
void Trace::enable_( void )
{
    Mutex::ScopeLock criticalSection( PrivateLocks::tracing() );
    enabled_ = true;
}


void Trace::disable_( void )
{
    Mutex::ScopeLock criticalSection( PrivateLocks::tracing() );
    enabled_ = false;
}


bool Trace::isEnabled_( void )
{
    Mutex::ScopeLock criticalSection( PrivateLocks::tracing() );
    bool result = enabled_;
    return result;
}


Trace::InfoLevel_T Trace::setInfoLevel_( Trace::InfoLevel_T newLevel )
{
    Mutex::ScopeLock criticalSection( PrivateLocks::tracing() );
    InfoLevel_T previous = infoLevel_;
    infoLevel_           = newLevel;
    return previous;
}

Trace::InfoLevel_T Trace::getInfoLevel_( void ) noexcept
{
    Mutex::ScopeLock criticalSection( PrivateLocks::tracing() );
    InfoLevel_T current = infoLevel_;
    return current;
}


bool Trace::enableSection_( const char* sectionToEnable )
{
    bool result = false;
    if ( sectionToEnable )
    {
        Mutex::ScopeLock criticalSection( PrivateLocks::tracing() );

        int i;
        for ( i = 0; i < OPTION_KIT_SYSTEM_TRACE_MAX_SECTIONS; i++ )
        {
            if ( activeSections_[i].isEmpty() )
            {
                activeSections_[i] = sectionToEnable;
                result             = true;
                break;
            }
        }
    }

    return result;
}


void Trace::disableSection_( const char* sectionToDisable )
{
    Mutex::ScopeLock criticalSection( PrivateLocks::tracing() );

    // Disable all sections
    if ( sectionToDisable == 0 )
    {
        int i;
        for ( i = 0; i < OPTION_KIT_SYSTEM_TRACE_MAX_SECTIONS; i++ )
        {
            activeSections_[i].clear();
        }
    }

    // Disable a single section
    else
    {
        int i;
        for ( i = 0; i < OPTION_KIT_SYSTEM_TRACE_MAX_SECTIONS; i++ )
        {
            if ( activeSections_[i] == sectionToDisable )
            {
                activeSections_[i].clear();
                break;
            }
        }
    }
}


bool Trace::isSectionEnabled_( const char* section )
{
    Mutex::ScopeLock criticalSection( PrivateLocks::tracing() );

    bool result = false;
    if ( enabled_ )
    {
        int i;
        for ( i = 0; i < OPTION_KIT_SYSTEM_TRACE_MAX_SECTIONS; i++ )
        {
            if ( activeSections_[i][0] != '*' )
            {
                if ( activeSections_[i] == section )
                {
                    result = true;
                    break;
                }
            }
            else
            {
                if ( activeSections_[i].isEqualSubstring( 1, activeSections_[i].length() - 1, section ) )
                {
                    result = true;
                    break;
                }
            }
        }
    }

    return result;
}

unsigned Trace::getSections_( Kit::Text::IString& dst )
{
    bool     first = true;
    unsigned count = 0;
    dst.clear();

    Mutex::ScopeLock criticalSection( PrivateLocks::tracing() );
    int i;
    for ( i = 0; i < OPTION_KIT_SYSTEM_TRACE_MAX_SECTIONS; i++ )
    {
        if ( activeSections_[i].isEmpty() == false )
        {
            if ( !first )
            {
                dst += ' ';
            }

            dst   += activeSections_[i];
            first  = false;
            count++;
        }
    }

    return count;
}

// TODO: Implement thread awareness
#if 0
void Trace::setThreadFilter_( const char* threadName1,
                              const char* threadName2,
                              const char* threadName3,
                              const char* threadName4 )
{
    Mutex::ScopeLock criticalSection( PrivateLocks::tracing() );

    // Set filter to: ACTIVE
    threadFilterEnabled_ = true;

    // Update filter table
    threadFilters_[0] = threadName1;
    threadFilters_[1] = threadName2;
    threadFilters_[2] = threadName3;
    threadFilters_[3] = threadName4;
}

void Trace::clearThreadFilter_( void )
{
    Mutex::ScopeLock criticalSection( PrivateLocks::tracing() );
    threadFilterEnabled_ = false;

    // Clear  filter table
    threadFilters_[0] = 0;
    threadFilters_[1] = 0;
    threadFilters_[2] = 0;
    threadFilters_[3] = 0;
}


unsigned Trace::getThreadFilters_( Kit::Text::IString& dst )
{
    bool     first = true;
    unsigned count = 0;
    dst.clear();

    Mutex::ScopeLock criticalSection( PrivateLocks::tracing() );
    int i;
    for ( i = 0; i < NUM_THREAD_FILTERS_; i++ )
    {
        if ( threadFilters_[i] != 0 )
        {
            if ( !first )
            {
                dst += ' ';
            }

            dst   += threadFilters_[i];
            first  = false;
            count++;
        }
    }

    return count;
}

#endif

bool Trace::passedThreadFilter_()
{
    return true;
    // TODO: Implement thread awareness
#if 0    
    // Safely get the current's thread name (i.e. works with non-KIT threads)
    const char* threadNameToTest = nullptr;
    Thread*     curThread        = Thread::tryGetCurrent();
    if ( curThread != nullptr )
    {
        threadNameToTest = curThread->getName();
    }

    bool result = true;
    Mutex::ScopeLock criticalSection( PrivateLocks::tracing() );
    if ( threadFilterEnabled_ && threadNameToTest )
    {
        int i;
        for ( i = 0, result = false; i < NUM_THREAD_FILTERS_; i++ )
        {
            if ( threadFilters_[i] && strcmp( threadNameToTest, threadFilters_[i] ) == 0 )
            {
                result = true;
                break;
            }
        }
    }

    return result;
#endif
}

}  // end namespaces
}
//------------------------------------------------------------------------------