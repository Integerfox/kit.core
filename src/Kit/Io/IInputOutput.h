#ifndef KIT_IO_IINPUTOUTPUT_H_ 
#define KIT_IO_IINPUTOUTPUT_H_ 
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Io/IInput.h"
#include "Kit/Io/IOutput.h"

///
namespace Kit {
///
namespace Io {


/** This abstract class defines a interface for operating on an input-output
    stream. Example of input-output streams: socket connection, UART.
 */
class IInputOutput : public IInput,
    public IOutput
{
};


}       // end namespaces
}
#endif  // end header latch
