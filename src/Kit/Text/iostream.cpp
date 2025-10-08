/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "IString.h"
#include <string.h>


////////////////////////////////////
namespace std {

ostream& operator<< ( ostream &out, const Kit::Text::IString& outstring )
{
    out << outstring.getString();
    return out;
}

istream& operator>> ( istream &in, Kit::Text::IString& instring )
{
    int        maxlen  = 0;
    char*      dstPtr  = instring.getBuffer( maxlen );
    memset( dstPtr, 0, maxlen+1 );  // Ensure there is null terminator.  Note: maxlen does NOT include the NULL terminator
    return in.read( dstPtr, maxlen );
}

};