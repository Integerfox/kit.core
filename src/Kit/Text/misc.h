#ifndef Cpl_Text_misc_h_
#define Cpl_Text_misc_h_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */
/** @file

    This file contains a set of miscellaneous low-level text/string functions.

    NOTE: The application should be CAREFUL when using these method in that the
          provide little/to-none protection against memory errors/over-runs.
*/

#include <stdint.h>
#include <stdlib.h>


///
namespace Cpl {
///
namespace Text {


/** Returns the binary value for the 'ASCII HEX' character 'c'.  Returns -1
    if there is an error (e.g. 'c' is not valid 'hex' character)
 */
uint8_t unhexChar( char c );


/** This method converts up to 'numCharToScan' characters from the ASCII HEX
    text string.  The converted binary data is stored in 'outData'.

    The application is responsible for ENSURING that the 'outData' is large
    enough to hold the converted output!
 */
bool unhex( const char* inString, size_t numCharToScan, uint8_t* outData );


};      // end namespaces
};
#endif  // end header latch
