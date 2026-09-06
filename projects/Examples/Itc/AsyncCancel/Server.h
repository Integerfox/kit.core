#ifndef EXAMPLES_ITC_ASYNCCANCEL_FLASHSERVER_H_
#define EXAMPLES_ITC_ASYNCCANCEL_FLASHSERVER_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Examples/Itc/AsyncCancel/IFlashRequest.h"
#include "Kit/EventQueue/IQueue.h"
#include "Kit/Itc/OpenCloseSync.h"
#include "Kit/System/Trace.h"
#include "Kit/Bsp/Api.h"
#include "Kit/System/Timer.h"
#include "Kit/Container/SList.h"
#include <inttypes.h>

/// Minimum flash rate in milliseconds
#ifndef OPTION_EXAMPLES_ITC_ASYNCCANCEL_SERVER_MIN_FLASH_RATE_MS
#define OPTION_EXAMPLES_ITC_ASYNCCANCEL_SERVER_MIN_FLASH_RATE_MS 50  // 10Hz
#endif

/// Maximum flash rate in milliseconds
#ifndef OPTION_EXAMPLES_ITC_ASYNCCANCEL_SERVER_MAX_FLASH_RATE_MS
#define OPTION_EXAMPLES_ITC_ASYNCCANCEL_SERVER_MAX_FLASH_RATE_MS 5000  // 0.1Hz
#endif


///
namespace Examples {
///
namespace Itc {
///
namespace AsyncCancel {

/** This concrete class is a 'server' that produces the LED flash patterns. The
    server accepts flash requests from many clients.  The server responses to
    a client AFTER it has completed the client's requested duty cycles. Flash
    requests are queued in FIFO order. A software timer is used to manage the
    LED flashing.
 */
class Server : public Kit::Itc::OpenCloseSync, public IFlashRequest, public Kit::System::Timer
{
public:
    /// Constructor
    Server( Kit::EventQueue::IQueue& myEventQueue ) noexcept
        : Kit::Itc::OpenCloseSync( myEventQueue )
        , Timer( myEventQueue )
        , m_currentFlashMsg( nullptr )
        , m_opened( false )
    {
    }

public:
    /// See Kit::Itc::OpenCloseSync
    void request( OpenMsg& msg ) noexcept override
    {
        // Only 'open' when not already opened
        if ( !m_opened )
        {
            // Housekeeping
            m_opened          = true;
            m_currentFlashMsg = nullptr;
            m_onCycle         = false;
            m_repeatCount     = UINT32_MAX;
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
            Timer::stop();

            // Return the current flash message to the sender if it exists
            if ( m_currentFlashMsg != nullptr )
            {
                m_currentFlashMsg->getPayload().success = false;
                m_currentFlashMsg->returnToSender();
            }

            // Drain the request queue
            auto* msg = m_flashRequestList.get();
            while ( msg != nullptr )
            {
                msg->getPayload().success = false;
                msg->returnToSender();
                msg = m_flashRequestList.get();
            }
        }

        // Return the message to the sender since we are done processing the request
        msg.returnToSender();
    }


public:
    /// See IFlashRequest
    void request( FlashMsg& msg ) noexcept override
    {
        // NOTE: Make sure to have a *reference* to the payload (instead of a copy)
        //       since we need to update the 'success' field in the payload to
        //       indicate pass/fail of the request
        auto& payload = msg.getPayload();

        // Validate the flash rate request
        if ( ( payload.flashPattern.onTimeMs < OPTION_EXAMPLES_ITC_ASYNCCANCEL_SERVER_MIN_FLASH_RATE_MS ) ||
             ( payload.flashPattern.onTimeMs > OPTION_EXAMPLES_ITC_ASYNCCANCEL_SERVER_MAX_FLASH_RATE_MS ) ||
             ( payload.flashPattern.offTimeMs < OPTION_EXAMPLES_ITC_ASYNCCANCEL_SERVER_MIN_FLASH_RATE_MS ) ||
             ( payload.flashPattern.offTimeMs > OPTION_EXAMPLES_ITC_ASYNCCANCEL_SERVER_MAX_FLASH_RATE_MS ) ||
             ( payload.flashPattern.onTimeMs == 0 && payload.flashPattern.offTimeMs == 0 ) ||
             ( payload.flashPattern.repeatCount == 0 ) )
        {
            // Invalid flash rate request.  Set the response value to false to indicate failure and return
            payload.success = false;
            msg.returnToSender();  // Respond to the client immediately
            return;
        }

        // Queue the request if currently processing another flash request
        if ( m_currentFlashMsg != nullptr )
        {
            m_flashRequestList.put( msg );
            return;
        }
        m_currentFlashMsg = &msg;

        // Begin the duty cycle(s)
        // NOTE: The client's message is returned AFTER the duty cycle(s) have completed
        beginDutyCycleSequence();
    }

    /// See IFlashRequest
    void request( FlashCancelMsg& msg ) noexcept override
    {
        KIT_SYSTEM_TRACE_MSG( "main", "Received flash cancel request. %p, %p", m_currentFlashMsg, msg.getPayload().flashMsgToCancel );

        // Default the results to: failed/not-cancelled
        auto& payload    = msg.getPayload();
        payload.canceled = false;

        // Is the request in progress
        if ( m_currentFlashMsg != nullptr && m_currentFlashMsg == payload.flashMsgToCancel )
        {
            KIT_SYSTEM_TRACE_MSG( "main", "Cancelling current request..." );
            Timer::stop();
            Bsp_turn_off_debug1();
            payload.canceled                        = true;
            m_currentFlashMsg->getPayload().success = false;  // Mark the transaction as failed due to cancellation
            m_currentFlashMsg->returnToSender();
            startNextRequest();
        }

        // Search the request queue for the message being canceled
        else
        {
            auto* queuedMsgPtr = m_flashRequestList.first();
            while ( queuedMsgPtr != nullptr )
            {
                if ( queuedMsgPtr == payload.flashMsgToCancel )
                {
                    KIT_SYSTEM_TRACE_MSG( "main", "Cancelling queued request..." );
                    m_flashRequestList.remove( *queuedMsgPtr );
                    payload.canceled                   = true;
                    queuedMsgPtr->getPayload().success = false;  // Mark the queued transaction as failed due to cancellation
                    queuedMsgPtr->returnToSender();
                    break;  // Exit the loop once the Requestmessage is found and removed
                }
                queuedMsgPtr = m_flashRequestList.next( *queuedMsgPtr );  // Move to the next message in the queue
            }
        }

        // Return the 'Cancel' message (NOTE: Must be returned AFTER returning the canceled request message)
        msg.returnToSender();
    }

protected:
    /// See Kit::System::ICounter, i.e. software timer expired callback
    void expired() noexcept override
    {
        // ON Cycle in progress
        if ( m_onCycle )
        {
            m_onCycle = false;
            Bsp_turn_off_debug1();
            Timer::start( m_currentFlashMsg->getPayload().flashPattern.offTimeMs );
        }

        // OFF Cycle in progress
        else
        {
            m_repeatCount++;
            m_onCycle = true;
            KIT_SYSTEM_TRACE_MSG( "main", "LED off (%" PRIu32 " ms, cycleCount=%" PRIu32 ")", m_currentFlashMsg->getPayload().flashPattern.offTimeMs, m_repeatCount );

            if ( m_repeatCount >= m_currentFlashMsg->getPayload().flashPattern.repeatCount )
            {
                m_currentFlashMsg->getPayload().success = true;
                m_currentFlashMsg->returnToSender();
                startNextRequest();
            }
            else
            {
                startOnCycle();
            }
        }
    }

protected:
    /// Helper
    void beginDutyCycleSequence() noexcept
    {
        m_repeatCount = 0;
        startOnCycle();
    }

    /// Helper
    void startOnCycle() noexcept
    {
        m_onCycle = true;
        Bsp_turn_on_debug1();
        auto onTime = m_currentFlashMsg->getPayload().flashPattern.onTimeMs;
        KIT_SYSTEM_TRACE_MSG( "main", "LED ON  (%" PRIu32 " ms, cycleCount=%" PRIu32 ")", onTime, m_repeatCount + 1 );
        Timer::start( onTime );
    }

    /// Helper:
    void startNextRequest() noexcept
    {
        m_currentFlashMsg = m_flashRequestList.get();
        if ( m_currentFlashMsg != nullptr )
        {
            beginDutyCycleSequence();
        }
    }

protected:
    /// Current flash request message
    FlashMsg* m_currentFlashMsg;

    /// List of Flash requests
    Kit::Container::SList<FlashMsg> m_flashRequestList;

    /// Track the duty cycle count
    uint32_t m_repeatCount;

    /// Track the duty cycle state
    bool m_onCycle;

    /// Track my open/closed
    bool m_opened;
};

}  // end namespaces
}
}
#endif  // end header latch
