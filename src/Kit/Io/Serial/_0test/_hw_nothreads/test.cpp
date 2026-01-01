/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Io/Types.h"
#include "kit_config.h"
#include "test.h"
#include "Kit/Bsp/Api.h"
#include "Kit/Text/FString.h"



////////////////////////////////////////////////////////////////////////////////

#ifndef MAX_INPUT
#define MAX_INPUT       1024
#endif

static char  inbuffer[MAX_INPUT + 1];
static Kit::Text::FString<128> tmpString;

void test( Kit::Io::IInput& infd, Kit::Io::IOutput& outfd )
{
    outfd.write( "\n== Testing: Kit::Io stream via a serial device..." );
    outfd.write( "\n== Board will echo back input character once a newline\nhas been received." );
    uint32_t startTime = elapsedTimeMs();
    Bsp_turn_on_debug1();

    while ( true )
    {
        // Toggle my LED
        uint32_t now = elapsedTimeMs();
        if ( now - startTime > 1000 )
        {
            startTime = now;
            Bsp_toggle_debug1();
        }

        Kit::Io::ByteCount_T bytesRead  = 0;
        if ( !infd.read( inbuffer, MAX_INPUT, bytesRead ) )
        {
            tmpString.format( "\n**** ERROR occurred while reading input stream (requested bytes=%d)\n", MAX_INPUT );
            outfd.write( tmpString );
        }
        else
        {
            if ( bytesRead > 0 )
            {
                // Ensure the input data is null terminated
                inbuffer[bytesRead] = '\0';
                tmpString.format( "\nECHO: len=%d [", (int) bytesRead );
                outfd.write( tmpString );
                outfd.write( inbuffer );
                outfd.write( "]\n" );
            }
        }
    }
}
