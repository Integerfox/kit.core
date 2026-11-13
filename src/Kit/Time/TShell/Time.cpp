/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Time.h"
#include "Kit/Text/Tokenizer/TextBlock.h"
#include "Kit/Text/Format.h"
#include "Kit/System/ElapsedTime.h"
#include "Kit/Time/AbsoluteTime.h"
#include "Kit/Time/BootTime.h"
#include <inttypes.h>

//------------------------------------------------------------------------------
namespace Kit {
namespace Time {
namespace TShell {


Kit::TShell::Result_T Time::execute( Kit::TShell::IContext& context, char* cmdString ) noexcept
{
    Kit::Text::Tokenizer::TextBlock tokens( cmdString );
    Kit::Text::IString&             outtext = context.getOutputBuffer();

    // Error checking
    if ( tokens.numParameters() > 1 )
    {
        return Kit::TShell::Result_T::CMD_ERR_BAD_SYNTAX;
    }

    bool io = true;

    // Prefix a newline so command output always starts on its own line below the prompt.
    outtext.format( "\nElapsed Time (ms):               %" PRIu64, Kit::System::ElapsedTime::millisecondsEx() );
    io &= context.writeFrame( outtext );

    uint64_t nowMs = Kit::Time::getAbsoluteTimeMilliseconds();
    outtext.format( "Wall Clock Time (UTC):           " );
    Kit::Text::Format::utcTimestamp( outtext, nowMs, true, true );
    io &= context.writeFrame( outtext );

    outtext.format( "Wall Clock Time (ms):            %" PRIu64, nowMs );
    io &= context.writeFrame( outtext );

    KitTimeBootCount_T bootCounter;
    uint64_t           bootElapsedMs;
    Kit::Time::parseBootTime( Kit::Time::getBootTime(), bootCounter, bootElapsedMs );
    outtext.format( "Boot Time (counter, elapsed ms): %" PRIu64 " %" PRIu64, static_cast<uint64_t>( bootCounter ), bootElapsedMs );
    return writeLastFrame( context, outtext, io );
}

}  // end namespaces
}
}
//------------------------------------------------------------------------------
