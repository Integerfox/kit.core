#ifndef KIT_PERSISTENCE_RECORD_ICHUNK_H
#define KIT_PERSISTENCE_RECORD_ICHUNK_H
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
#include "Kit/Persistence/Record/IPayload.h"
#include <stdlib.h>

///
namespace Kit {
///
namespace Persistence {
///
namespace Record {

/** This abstract class defines the interface for managing a 'chunk'. A chunk is
    responsible for managing the meta-data (e.g. CRC) associated with a Record's
    data when stored in persistent storage. A chunk is also responsible for
    reading and writing the Record's data from/to persistent storage

    There is a one-to-one relationship between a IChunk and IRecord instances.
 */
class IChunk
{
public:
    /** This method is to start/initialize the chunk.  It is typically only
        called once at the startup of the application.  However, start() can be
        called after a previous call to the stop() method.

        This method is called when the corresponding Record instance is 'started'

        The 'myEventQueue' is a reference to the RecordServer's Event Queue, i.e. the
        event loop for the thread that the Chunk executes in.
     */
    virtual void start( Kit::EventQueue::IQueue& myEventQueue ) noexcept = 0;

    /** This method is to stop/shutdown the chunk.  It is typically only
        called once during an orderly shutdown of the application. However,
        start() can be after a previous call to the stop() method.
     */
    virtual void stop() noexcept = 0;


public:
    /** This method is used to initiate the sequence to retrieve a Record's
        data from persistent storage. 

        The 'index' offset can be used by the Chunk client to 'index into' the
        Chunk's region instead of starting at offset zero.  This argument should
        only be used when multiple instances of a Record is being stored in
        a single region.

        The method returns true if the read operation was successful and that
        the data is 'valid'; else false is returned
     */
    virtual bool loadData( IPayload& destination,
                           size_t    index = 0 ) noexcept = 0;

    /** This method is used to update persistent storage with new data for the
        Record.  The entire record is written/updated when this call is made.
        The method does not return until the write operation has completed.

        If the 'invalidate' argument is true, the instead of writing the
        new data to persistent storage, all binary zero's are written AND all
        of the chunk's metadata is invalidated.  This effectively erases the
        Record, i.e. the next time the record is loaded, it will fail because
        the chunk data is NOT valid.

        The 'index' offset can be used by the Chunk client to 'index into' the
        Chunk's region instead of starting at offset zero.  This argument should
        only be used when multiple instances of a Record is being stored in
        a single region.

        The method returns true if successful; else false is returned.  It is
        the responsibility of the Record/Application to decided what to do when
        there is error (e.g. ignored, a log entry generated, etc.)
    */
    virtual bool updateData( IPayload& source,
                             size_t    index      = 0,
                             bool      invalidate = false ) noexcept = 0;


public:
    /** This method returns the size, in bytes, of any/all metadata that is
        included with the record when it is stored in persistent storage.
     */
    virtual size_t getMetadataLength() const noexcept = 0;

public:
    /// Virtual destructor
    virtual ~IChunk() = default;
};

}  // end namespaces
}
}
#endif  // end header latch
