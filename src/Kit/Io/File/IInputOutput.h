#ifndef KIT_IO_FILE_IINPUT_OUTPUT_H_
#define KIT_IO_FILE_IINPUT_OUTPUT_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Io/File/IInput.h"
#include "Kit/Io/File/IOutput.h"

///
namespace Kit {
///
namespace Io {
///
namespace File {


/** This abstract class defines the interface for a Random Access Input
    Output File.

    NOTE: All the read/write operations return 'false' if an error occurred,
          this INCLUDES the end-of-file condition (which is error when dealing
          with streams). To differentiate between a true error and EOF, the
          client must call isEof().

    NOTE: A File::IInputOutput object does not inherit from Kit::Io::IInputOutput,
          i.e. a File::IInputOutput object can be substituted for a 
          Kit::Io::IInputOutput object. 
 */
class IInputOutput : public IInput, public IOutput
{
};


}      // end namespaces
}
}
#endif  // end header latch
