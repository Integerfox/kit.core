#ifndef CONTAINER_RINGBUFFER_PRODUCER_H_
#define CONTAINER_RINGBUFFER_PRODUCER_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Container/RingBuffer/FlashCode.h"
#include "Kit/Container/RingBufferMP.h"
#include "Kit/Dm/ObserverCallback.h"
#include "Kit/Itc/OpenCloseSync.h"
#include "Kit/EventQueue/IQueue.h"
#include "Kit/System/Shutdown.h"
#include "Kit/System/Trace.h"
#include "Kit/Bsp/Api.h"
#include <climits>
#include <inttypes.h>

///
namespace Container {
///
namespace RingBuffer {

/** This concrete class is responsible for generating flash codes (i.e. adding
    to the Flash Code RingBuffer.  The class fills the entire Ring Buffer and
    then waits for the Ring Buffer to be empty before repeating the fill process
 */
class Producer : public Kit::Itc::OpenCloseSync
{
public:
    /// Constructor
    Producer( Kit::EventQueue::IQueue&                   myEventQueue,
              Kit::Container::RingBufferMP<FlashCode_T>& flashCodeQueue,
              unsigned                                   maxRefills = UINT_MAX )
        : Kit::Itc::OpenCloseSync( myEventQueue )
        , m_flashCodeQueue( flashCodeQueue )
        , m_obFlashCodeQueue( myEventQueue )
        , m_maxRefills( maxRefills )
        , m_opened( false )
    {
        m_obFlashCodeQueue.setCallback<Producer, &Producer::queueElementCountChanged>( this );
    }

public:
    /// See Kit::Itc::OpenCloseSync
    void request( OpenMsg& msg ) noexcept override
    {
        // Only 'open' when not already opened
        if ( !m_opened )
        {
            // Housekeeping
            m_opened    = true;
            m_fillCount = 0;

            // Subscribe for the Queue change notifications
            m_flashCodeQueue.m_mpElementCount.attach( m_obFlashCodeQueue );

            // Fill the Flash Code Queue with flash codes to be executed by the Server
            fillFlashCodeQueue();
        }

        // Return the message to the sender since we are done processing the request
        msg.returnToSender();
    }

    /// See Kit::Itc::OpenCloseSync
    void request( CloseMsg& msg ) noexcept override
    {
        // Only 'close' when already opened
        if ( m_opened )
        {
            // Housekeeping
            m_opened = false;

            // Make sure the all of the potential callbacks get cancelled
            m_flashCodeQueue.m_mpElementCount.detach( m_obFlashCodeQueue );
        }

        // Return the message to the sender since we are done processing the request
        msg.returnToSender();
    }


protected:
    /// Callback when the number of items in the Flash Code Queue changes
    void queueElementCountChanged( Kit::Dm::Mp::Uint32& mpElementCount, Kit::Dm::IObserver& observer ) noexcept
    {
        uint32_t numElementsInQueue;
        if ( mpElementCount.readAndSync( numElementsInQueue, observer ) && numElementsInQueue == 0 )
        {
            // Temporarily cancel the change notification to skip receiving 'adding' notifications
            mpElementCount.detach( m_obFlashCodeQueue );

            // Re-fill the Flash Code Queue with flash codes to be executed by the LedServer
            fillFlashCodeQueue();

            // Shutdown after N 'fill cycles'
            if ( m_fillCount >= m_maxRefills )
            {
                Kit::System::Shutdown::success();
                return;
            }

            // Re-subscribe for the Flash Code Queue change notifications.
            mpElementCount.attach( m_obFlashCodeQueue );
        }
    }

protected:
    /// Helper method that produces flash codes and adds them to the Flash Code Queue
    void fillFlashCodeQueue() noexcept
    {
        m_fillCount++;
        unsigned idx      = 0;
        unsigned numAdded = 0;
        while ( m_flashCodeQueue.add( g_fcodes[idx] ) )
        {
            idx = ( idx + 1 ) % ( sizeof( g_fcodes ) / sizeof( g_fcodes[0] ) );
            numAdded++;
        }
        KIT_SYSTEM_TRACE_MSG( "main", "Producer[%u]: Added %u Flash Codes to the Queue", m_fillCount, numAdded );
    }

protected:
    /// Flash codes Queue
    Kit::Container::RingBufferMP<FlashCode_T>& m_flashCodeQueue;

    /// Observer callback for when flash code(s) are removed from the Queue
    Kit::Dm::ObserverCallback<Kit::Dm::Mp::Uint32> m_obFlashCodeQueue;

    /// Maximum number of times to fill/refill the Flash Code Queue.
    unsigned m_maxRefills;

    /// Number of fill/refill attempts
    unsigned m_fillCount;

    /// Track my open/closed
    bool m_opened;

    /// Array of flash codes
    static FlashCode_T g_fcodes[4];
};

}  // end namespaces
}
#endif  // end header latch
