#ifndef KIT_IO_SOCKET_IINPUT_OUTPUT_H_
#define KIT_IO_SOCKET_IINPUT_OUTPUT_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Io/IInputOutput.h"

///
namespace Kit {
///
namespace Io {
///
namespace Socket {


/** This abstract class defines the interface for a BSD socket.
 */
class IInputOutput : public Kit::Io::IInputOutput
{
public:
    /** This method is used to 'activate' socket.  This method MUST be called
        after the socket connection is established and BEFORE any Input/Output
        'stream' methods are called.

        NOTE: If the InputOutput instance is already 'active' (i.e. a previous
              call to activate() was made), then the existing socket is closed
              before activating the new socket.
     */
    virtual void activate( KitIoSocketHandle_T newSocketHandle ) noexcept = 0;
};


}      // end namespaces
}
}
#endif  // end header latch
