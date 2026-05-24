#ifndef KIT_PERSISTENCE_RECORD_MEDIA_FILEADAPTER_H
#define KIT_PERSISTENCE_RECORD_MEDIA_FILEADAPTER_H
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

/** This concrete class implements the IMedia interface using the
    Kit::Io::File interfaces. Each instance of this class uses a single file
    as the storage media.  It is the responsibility of the application to ensure
    that each instance has a unique file name
 */
class FileAdapter : public Kit::Persistence::Record::IMedia
{
public:
    /// Constructor
    FileAdapter( const char* fileName, Size_T allocatedLen ) noexcept
        : m_fname( fileName )
        , m_maxLen( allocatedLen )
    {
    }


public:
    /// See Kit::Persistence::Record::IMedia
    bool start( Kit::EventQueue::IQueue& myEventQueue ) noexcept override;

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

protected:
    /// Name of the file used for storage
    const char* m_fname;

    /// Maximum support storage size
    Size_T m_maxLen;
};

}  // end namespaces
}
}
}
#endif  // end header latch
