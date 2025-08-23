/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/

// #include "Kit/System/Thread.h"
#include "Kit/System/ElapsedTime.h"
#include "Kit/System/api.h"
// #include "Kit/System/SimTick.h"
#include "Kit/System/Private.h"
#include "Kit/Text/Format.h"
#include <cstdint>


//------------------------------------------------------------------------------
namespace Kit {
namespace System {


////////////////////////////////////////////////////////////////////////////////
// TODO: Implement tracing via Kit::Io::Output streams
#if 0
//static Kit::Io::Output* activePtr_ = Trace::getDefaultOutputStream_();
void Trace::redirect_( Kit::Io::Output& newMedia )
{
    Locks_::tracing().lock();
    activePtr_ = &newMedia;
    Locks_::tracing().unlock();
}


void Trace::revert_( void )
{
    Locks_::tracing().lock();
    activePtr_ = getDefaultOutputStream_();
    Locks_::tracing().unlock();
}
#endif


////////////////////////////////////////////////////////////////////////////////
void PrivateTracePlatform::output( Kit::Text::IString& src )
{
    printf( "%s", src.getString() );

    // TODO: Implement tracing via Kit::Io::Output streams
    // activePtr_->write( src.getString() );
}


/* Info settings:

    eNONE     -->  ">> <user message>"
    eBRIEF    -->  ">> DD HH:MM:SS.MMM <user message>"
    eINFO     -->  ">> DD HH:MM:SS.MMM (<section name>) <user message>"
    eVERBOSE  -->  ">> DD HH:MM:SS.MMM (<section name>) [<thread name>] <user message>"
    eMAX      -->  ">> DD HH:MM:SS.MMM (<section name>) [<thread name>] {<filename>,<line#>[,<funcname>]} <user message>"

*/
void PrivateTracePlatform::appendInfo( Kit::Text::IString& dst, Trace::InfoLevel_T info, const char* section, const char* filename, int linenum, const char* funcname )
{
    // TODO: Implement thread awareness
    const char* threadName = "n/a";
    // if ( Kit::System::Thread::tryGetCurrent() != nullptr )
    // {
    //     threadName = Kit::System::Thread::myName();
    // }

    // Level: eBRIEF
    if ( info > Trace::eNONE )
    {
        // TODO: Implement simulated time aware
        // Indent "simulated time" time stamps
        // if ( CPL_SYSTEM_SIM_TICK_USING_SIM_TICKS() )
        // {
        //     dst += "  ";
        // }

        // Add time stamp (Note: Elapsed time may not be valid/working when
        // the scheduler has not been started - so use 'zero' instead)
        uint64_t now = 0;
        if ( isSchedulingEnabled() )
        {
            now = ElapsedTime::millisecondsEx();
        }
        Kit::Text::Format::timestamp( dst, now, true, true );
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
