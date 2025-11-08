/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/

#include "Kit/System/Mutex.h"
#include "Kit/System/Thread.h"
#include "Kit/System/ElapsedTime.h"
#include "Kit/System/Api.h"
#include "Kit/System/SimTick.h"
#include "Kit/System/Private.h"
#include "Kit/Text/Format.h"


//------------------------------------------------------------------------------
namespace Kit {
namespace System {


////////////////////////////////////////////////////////////////////////////////
static Kit::Io::IOutput* activePtr_ = Trace::getDefaultOutputStream_();

void Trace::redirect_( Kit::Io::IOutput& newMedia ) noexcept
{
    Mutex::ScopeLock criticalSection( PrivateLocks::tracing() );
    activePtr_ = &newMedia;
}


void Trace::revert_() noexcept
{
    Mutex::ScopeLock criticalSection( PrivateLocks::tracing() );
    activePtr_ = getDefaultOutputStream_();
}


////////////////////////////////////////////////////////////////////////////////
void PrivateTracePlatform::output( Kit::Text::IString& src ) noexcept
{
    activePtr_->write( src.getString() );
}


/* Info settings:

    eNONE     -->  ">> <user message>"
    eBRIEF    -->  ">> DD HH:MM:SS.MMM <user message>"
    eINFO     -->  ">> DD HH:MM:SS.MMM (<section name>) <user message>"
    eVERBOSE  -->  ">> DD HH:MM:SS.MMM (<section name>) [<thread name>] <user message>"
    eMAX      -->  ">> DD HH:MM:SS.MMM (<section name>) [<thread name>] {<filename>,<line#>[,<funcname>]} <user message>"

*/
void PrivateTracePlatform::appendInfo( Kit::Text::IString& dst,
                                       Trace::InfoLevel_T  info,
                                       const char*         section,
                                       const char*         filename,
                                       int                 linenum,
                                       const char*         funcname ) noexcept
{
    const char* threadName = "n/a";
    if ( Kit::System::Thread::tryGetCurrent() != nullptr )
    {
        threadName = Kit::System::Thread::myName();
    }

    // Level: eBRIEF
    if ( info > Trace::eNONE )
    {
        dst.clear();

        // Indent "simulated time" time stamps
        if ( KIT_SYSTEM_SIM_TICK_USING_SIM_TICKS() )
        {
            dst += "  ";
        }

        // Add time stamp (Note: Elapsed time may not be valid/working when
        // the scheduler has not been started - so use 'zero' instead)
        uint64_t now = 0;
        if ( isSchedulingEnabled() )
        {
            now = ElapsedTime::millisecondsEx();
        }
        Kit::Text::Format::timestamp( dst, now, true, true, true );
        dst += ' ';

        // LEVEL: eINFO
        if ( info > Trace::eBRIEF )
        {
            // Add section name
            dst.formatAppend( "(%s) ", section );

            // LEVEL: eVERBOSE
            if ( info > Trace::eINFO )
            {
                // Add Thread name
                dst.formatAppend( "[%s] ", threadName );

                // LEVEL: eMAX
                if ( info > Trace::eVERBOSE )
                {
                    dst.formatAppend( "{%s,%d", filename, linenum );

                    // Skip funcname if the compiler does not support it
                    if ( funcname && *funcname != '\0' )
                    {
                        dst.formatAppend( ",%s", funcname );
                    }
                    dst += "} ";
                }
            }
        }
    }
}


}  // end namespaces
}
//------------------------------------------------------------------------------
