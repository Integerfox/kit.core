#ifndef CONTAINER_RINGBUFFER_LED_SERVER_H_
#define CONTAINER_RINGBUFFER_LED_SERVER_H_
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
#include "Kit/System/Trace.h"
#include "Kit/System/Api.h"
#include "Kit/Bsp/Api.h"
#include <inttypes.h>

///
namespace Container {
///
namespace RingBuffer {

/** This concrete class monitors the a Queue of LED Flash codes and the
    consumes-and-executes the Flash codes from the Queue.

    This class is thread safe
 */
class LedServer : public Kit::Itc::OpenCloseSync
{
public:
    /// Constructor
    LedServer( Kit::EventQueue::IQueue&                   myEventQueue,
               Kit::Container::RingBufferMP<FlashCode_T>& flashCodeQueue )
        : Kit::Itc::OpenCloseSync( myEventQueue )
        , m_flashCodeQueue( flashCodeQueue )
        , m_obFlashCodeQueue( myEventQueue )
        , m_opened( false )
    {
        m_obFlashCodeQueue.setCallback<LedServer, &LedServer::queueElementCountChanged>( this );
    }

public:
    /// See Kit::Itc::OpenCloseSync
    void request( OpenMsg& msg ) noexcept override
    {
        // Only 'open' when not already opened
        if ( !m_opened )
        {
            // Housekeeping
            m_opened = true;

            // Subscribe for the Flash Code Queue change notifications
            m_flashCodeQueue.m_mpElementCount.attach( m_obFlashCodeQueue );
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
        if ( mpElementCount.readAndSync( numElementsInQueue, observer ) && numElementsInQueue > 0 )
        {
            // Temporarily cancel the change notification to skip receiving 'removed' notifications
            mpElementCount.detach( m_obFlashCodeQueue );

            // Drain ALL flash code in the Queue and execute them.
            FlashCode_T flashCode;
            uint16_t    seqNum;
            while ( m_flashCodeQueue.remove( flashCode, seqNum ) )
            {
                executeFlashCode( flashCode );
            }

            // Re-subscribe for the Flash Code Queue change notifications.
            // Use the sequence number of the last 'used-by-the-server-state'
            // so as to NOT get an immediate callback when no new item has been added
            // while NOT missing a change notification that could have occurred
            // between when the server called remove() and and server re-subscribing
            mpElementCount.attach( m_obFlashCodeQueue, seqNum );
        }
    }

protected:
    /// Helper method to execute a flash code
    void executeFlashCode( const FlashCode_T& flashCode ) noexcept
    {
        KIT_SYSTEM_TRACE_MSG( "main",
                              "SERVER: Executing a flash code. on=%" PRIu32 " ms, off=%" PRIu32 " ms, numFlashes=%u, totalTime=%" PRIu32 " ms",
                              flashCode.onPeriodMs,
                              flashCode.offPeriodMs,
                              flashCode.numFlashes,
                              ( flashCode.onPeriodMs + flashCode.offPeriodMs ) * flashCode.numFlashes );

        for ( unsigned i = 0; i < flashCode.numFlashes; ++i )
        {
            Bsp_turn_on_debug1();
            Kit::System::sleep( flashCode.onPeriodMs );
            Bsp_turn_off_debug1();
            Kit::System::sleep( flashCode.offPeriodMs );
        }
    }

protected:
    /// Flash codes Queue
    Kit::Container::RingBufferMP<FlashCode_T>& m_flashCodeQueue;

    /// Observer callback for when flash code(s) are added to the Queue
    Kit::Dm::ObserverCallback<Kit::Dm::Mp::Uint32> m_obFlashCodeQueue;

    /// Track my open/closed
    bool m_opened;
};

}  // end namespaces
}
#endif  // end header latch
