#ifndef KIT_IO_FILE_OUTPUT_API_H_
#define KIT_IO_FILE_OUTPUT_API_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Io/IOutput.h"
#include "Kit/Io/File/IPosition.h"


///
namespace Kit {
///
namespace Io {
///
namespace File {


/** This abstract class defines the interface for a Random Access Input File.
    NOTE: All the write operations return 'false' if an error occurred, this
          INCLUDES the end-of-file condition (which is error when dealing
          with streams). To differentiate between a true error and EOF, the
          client must call isEof().
 */
class IOutput : public Kit::Io::IOutput, virtual public IPosition
{
};


};      // end namespaces
};
};
#endif  // end header latch


