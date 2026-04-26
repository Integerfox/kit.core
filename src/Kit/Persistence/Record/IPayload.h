#ifndef KIT_PERSISTENCE_RECORD_IPAYLOAD_H
#define KIT_PERSISTENCE_RECORD_IPAYLOAD_H
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Persistence/Types.h"

///
namespace Kit {
///
namespace Persistence {
///
namespace Record {

/** This abstract class defines the interface accessing the data payload of an
    individual Record instance.

    NOTE: The IPayload interface does not define any semantics regarding fixed
          length vs. variable length records.  However, the overall payload size 
          plus the concrete IRecord's overhead plus its the IChunk's overhead can
          NOT exceed the size of its IMedia's storage capacity.
 */
class IPayload
{
public:
    /** This method retrieves a Record's data payload contents.  It is used when
        transferring the Record's data TO persistent storage.  The method returns
        the number of bytes copied to 'dst' on success; else KIT_PERSISTENCE_SIZE_MAX
        is returned when there is failure.
     */
    virtual Size_T copyTo( void* dst, Size_T maxDstLen ) noexcept = 0;

    /** This method updates the Record's data payload contents.  It is used when
        transferring data FROM persistent storage to the Record.  The method
        returns true if successful; else false is returned.
     */
    virtual bool copyFrom( const void* src, Size_T srcLen ) noexcept = 0;


public:
    /// Virtual destructor
    virtual ~IPayload() = default;
};

}  // end namespaces
}
}
#endif  // end header latch
