#ifndef KIT_FRAMING_ISOURCE_H_
#define KIT_FRAMING_ISOURCE_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Type/SSize.h"


///
namespace Kit {
///
namespace Framing {

/** This abstract class defines an interface of the input source for a Decoder
    instance. The concrete instance determines the 'source', e.g. a
    Stream, RAM buffer, etc.
 */
class ISource
{


public:
    /** Attempts to read the specified number of bytes from the "input source"
        into the supplied buffer.  The actual number of bytes read is returned via
        'bytesRead'. Returns true if successful, false if no more data is currently
        available or an error was encountered.
     */
    virtual bool read( void*               dstBuffer,
                       Kit::Type::SSize_T  numBytes,
                       Kit::Type::SSize_T& bytesRead ) noexcept = 0;

public:
    /// Virtual Destructor
    virtual ~ISource() = default;
};


}  // end namespaces
}
#endif  // end header latch
