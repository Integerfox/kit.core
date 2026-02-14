#ifndef KIT_MEMORY_ALIGNEDCLASS_H_
#define KIT_MEMORY_ALIGNEDCLASS_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include <stdint.h>    // for uint8_t

///
namespace Kit {
///
namespace Memory {

/** This is a convenience type used to create a memory block that is large 
    enough to hold the memory footprint of ONE instance of 'class T' with proper
    alignment.
 */
template <class T>
struct AlignedClass
{
    /** This member defines the memory as a properly aligned character array.
     */
    alignas(alignof(T)) uint8_t m_byteMem[sizeof(T)];
};


}      // end namespaces
}
#endif  // end header latch
