#ifndef KIT_FRAMING_LINE_DECODER_ALLOCATE_H_
#define KIT_FRAMING_LINE_DECODER_ALLOCATE_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Framing/LineDecoder.h"

///
namespace Kit {
///
namespace Framing {

/** This concrete convenience template class extends the LineDecoder to include
    allocating the work buffer for the LineDecoder.
 */
template <int SIZE_OF_WORK_BUFFER>
class LineDecoderAllocate : public LineDecoder
{
public:
    /** Constructor. See LineDecoder for details about the arguments.
     */
    LineDecoderAllocate( ISource& source,
                         char     convertTabs = '\t' )
        : LineDecoder( source, m_workBuffer, sizeof( m_workBuffer ), convertTabs )
    {
    }

protected:
    /// Work buffer for the LineDecoder
    uint8_t m_workBuffer[SIZE_OF_WORK_BUFFER];
};


}  // end namespaces
}
#endif  // end header latch
