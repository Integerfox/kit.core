#ifndef KIT_PERSISTENCE_RECORD_IRECORD_H
#define KIT_PERSISTENCE_RECORD_IRECORD_H
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

/** This abstract class defines the public interface for a Record instance.
    A Record is the atomic-unit that is managed in Persistent storage media.
*/
class IRecord
{
public:
    /** This method is used to start/initialize the Record.  It is typically only
        called once at the startup of the application.  However, start() can be
        called after a previous call to the stop() method.

        This method is called when the Record Server is 'opened'.

        The 'myEventQueue' is a reference to the Record Server's Event Queue, i.e.
        the event loop for the thread that the Record Server executes in. Example 
        usage of 'myEventQueue' is if the IRecord needs/uses a software timer

        The method returns true on success; else false is returned when an error
        occurred during the start.
     */
    virtual bool start( Kit::EventQueue::IQueue& myEventQueue ) noexcept = 0;

    /** This method is used to stop/shutdown the Record.  It is typically only
        called once during an orderly shutdown of the application. However,
        start() can be called after a previous call to the stop() method.
     */
    virtual void stop() noexcept = 0;

public:
    /** This method returns the total number of bytes of Record when stored in 
        persistent storage. This includes both the payload and any metadata.
     */
    virtual Size_T getSize() const noexcept = 0;

public:
    /// Virtual destructor
    virtual ~IRecord() = default;
};

}  // end namespaces
}
}
#endif  // end header latch
