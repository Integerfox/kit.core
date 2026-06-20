#ifndef KIT_IO_RAM_INPUTOUTPUTALLOCATE_H_
#define KIT_IO_RAM_INPUTOUTPUTALLOCATE_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Io/Ram/InputOutput.h"
#include "Kit/Container/RingBufferAllocate.h"

///
namespace Kit {
///
namespace Io {
///
namespace Ram {

/** This concrete template convience class extends the RAM InputOutput class
    to include allocating the Ring Buffer memory.  The template argument 
    'NUM_BYTES' is the number of 'useable' bytes.
 */
template <int NUM_BYTES>
class InputOutputAllocate : public InputOutput
{
public:
    /// Constructor.
    InputOutputAllocate() noexcept
        : InputOutput( m_memory, sizeof(m_memory) )
    {
    }

protected:
    /// Memory for the Ring buffer (+1 because the Ring Buffer can only store N-1 bytes when the buffer size is N)
    uint8_t m_memory[NUM_BYTES+1];   
};

}  // end namespaces
}
}
#endif  // end header latch
