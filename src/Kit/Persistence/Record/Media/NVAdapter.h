#ifndef KIT_PERSISTENCE_RECORD_MEDIA_NVADAPTER_H
#define KIT_PERSISTENCE_RECORD_MEDIA_NVADAPTER_H
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Persistence/Record/IMedia.h"

///
namespace Kit {
///
namespace Persistence {
///
namespace Record {
///
namespace Media {

/** This abstract class defines the operations that can be performed on a
    persistent media.  The application is responsible for managing the
    physical storage provided by each concrete IMedia instance.

    There is a one-to-one relationship between IChunk and IMedia instances.
 */
class NVAdapter : public Kit::Persistence::Record::IMedia
{
public:
    /// See Kit::Persistence::Record::IMedia
    void start( Kit::EventQueue::IQueue& myEventQueue ) noexcept override;

    /// See Kit::Persistence::Record::IMedia
    void stop() noexcept override;

public:
    /// See Kit::Persistence::Record::IMedia
    bool write( Size_T offset, const void* srcData, Size_T srcLen ) noexcept override;

    /// See Kit::Persistence::Record::IMedia
    Size_T read( Size_T offset, void* dstBuffer, Size_T bytesToRead ) noexcept override;


public:
    /// See Kit::Persistence::Record::IMedia
    Size_T getMaxSize() const noexcept override;
};

}  // end namespaces
}
}
}
#endif  // end header latch
