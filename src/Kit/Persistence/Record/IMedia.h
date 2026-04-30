#ifndef KIT_PERSISTENCE_RECORD_IMEDIA_H
#define KIT_PERSISTENCE_RECORD_IMEDIA_H
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/EventQueue/IQueue.h"
#include "Kit/Persistence/Types.h"


///
namespace Kit {
///
namespace Persistence {
///
namespace Record {

/** This abstract class defines the operations that can be performed on a
    persistent media.  The application is responsible for managing the
    physical storage provided by each concrete IMedia instance.

    There is a one-to-one relationship between IChunk and IMedia instances.
 */
class IMedia
{
public:
    /** This method is used to start/initialize the IMedia instance.  It is typically
        only called once at the startup of the application.  However, start()
        can be called after a previous call to the stop() method.

        This method is called when the corresponding IChunk instance is 'started'

        The 'myEventQueue' is a reference to the Record Server's Event Queue,
        i.e. the event loop for the thread that the Record Server executes in.
        Example usage of 'myEventQueue' is if the IMedia needs/uses a software
        timer
     */
    virtual void start( Kit::EventQueue::IQueue& myEventQueue ) noexcept = 0;

    /** This method is used to stop/shutdown the IMedia instance.  It is typically only
        called once during an orderly shutdown of the application. However,
        start() can be called after a previous call to the stop() method.
     */
    virtual void stop() noexcept = 0;


public:
    /** This method writes 'srcLen' bytes to the media at the specified offset.

        The method return true of the write operation was successful, else false
        is returned.
     */
    virtual bool write( Size_T      offset,
                        const void* srcData,
                        Size_T      srcLen ) noexcept = 0;

    /** This method reads 'bytesToRead' bytes from the media at the specified
        offset.  The method does not return until:

        1) All requested bytes have been read,
        2) OR all available bytes have been read (i.e. end of media reached),
        3) OR an error occurs.

        The method returns the number of bytes read.  A return value of KIT_PERSISTENCE_SIZE_MAX
        indicates that an error occurred.
     */
    virtual Size_T read( Size_T offset,
                         void*  dstBuffer,
                         Size_T bytesToRead ) noexcept = 0;


public:
    /// Returns the maximum amount of storage, in bytes, the IMedia instance supports
    virtual Size_T getMaxSize() const noexcept = 0;


public:
    /// Virtual destructor
    virtual ~IMedia() = default;
};

}  // end namespaces
}
}
#endif  // end header latch
