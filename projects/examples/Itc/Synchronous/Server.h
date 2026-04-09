#ifndef ITC_SYNCHRONOUS_RATESERVER_H_
#define ITC_SYNCHRONOUS_RATESERVER_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Itc/Synchronous/RateSync.h"
#include "Kit/Itc/OpenCloseSync.h"
#include "Kit/EventQueue/IQueue.h"
#include "Kit/System/Trace.h"
#include "Kit/Bsp/Api.h"
#include "Kit/System/Timer.h"
#include "Kit/System/ElapsedTime.h"
#include <inttypes.h>

/// Minimum flash rate in milliseconds
#ifndef OPTION_ITC_SYNCHRONOUS_SERVER_MIN_FLASH_RATE_MS
#define OPTION_ITC_SYNCHRONOUS_SERVER_MIN_FLASH_RATE_MS 50  // 10Hz
#endif

/// Maximum flash rate in milliseconds
#ifndef OPTION_ITC_SYNCHRONOUS_SERVER_MAX_FLASH_RATE_MS
#define OPTION_ITC_SYNCHRONOUS_SERVER_MAX_FLASH_RATE_MS 5000  // 0.1Hz
#endif


///
namespace Itc {
///
namespace Synchronous {

/** This concrete class is a 'server' that flashes the LED.  It responds to
    rate requests from a 'client'.  A software timer is used to manage the LED
    flashing.
 */
class Server : public RateSync, public Kit::System::Timer, public Kit::Itc::OpenCloseSync
{
public:
    /// Constructor
    Server( Kit::EventQueue::IQueue& myMbox,
            uint32_t                 initialFlashRateMs = 500 ) noexcept  // default to 1Hz
        : RateSync( myMbox )
        , Timer( myMbox )
        , Kit::Itc::OpenCloseSync( myMbox )
        , m_flashRateMs( initialFlashRateMs )
        , m_opened( false )
    {
    }

public:
    /// See Itc::Synchronous::IRateRequest
    void request( RateMsg& msg ) noexcept override
    {
        // NOTE: Make sure to have a *reference* to the payload (instead of a copy)
        //       since we need to update the 'success' field in the payload to
        //       indicate pass/fail of the request
        auto& payload = msg.getPayload();

        // Validate the flash rate request
        if ( ( payload.flashRateMs < OPTION_ITC_SYNCHRONOUS_SERVER_MIN_FLASH_RATE_MS ) ||
             ( payload.flashRateMs > OPTION_ITC_SYNCHRONOUS_SERVER_MAX_FLASH_RATE_MS ) )
        {
            // Invalid flash rate request.  Set the response value to false to indicate failure and return
            payload.success = false;
        }

        // Update the Flash rate
        {
            payload.success = true;
            m_flashRateMs   = payload.flashRateMs;
            expired();  // Expire the timer immediately to update the flash rate without waiting for the current flash cycle to complete
        }

        // Return the message to the sender since we are done processing the request
        msg.returnToSender();
    }

public:
    /// See Kit::Itc::OpenCloseSync
    void request( OpenMsg& msg ) noexcept override
    {
        // Only 'open' when not already opened
        if ( !m_opened )
        {
            // Housekeeping
            m_opened     = true;
            m_timeMarker = Kit::System::ElapsedTime::milliseconds();

            // Start the timer to begin flashing the LED
            // Call the software timer expired function to start the flashing
            expired();
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
        }

        // Return the message to the sender since we are done processing the request
        msg.returnToSender();
    }

public:
    /// See Kit::System::ICounter, i.e. software timer expired callback
    void expired() noexcept override
    {
        // Toggle the LED
        Bsp_toggle_debug1();
        uint32_t now = Kit::System::ElapsedTime::milliseconds();
        KIT_SYSTEM_TRACE_MSG( "main", "LED Flash. delay=%" PRIu32 " ms", m_flashRateMs );

        // Restart the timer for the next flash cycle.  Account for the trace output latency
        // by adjusting the next duration to the next m_flashRateMs boundary
        uint32_t nextDuration = m_flashRateMs;
        uint32_t elapsed      = Kit::System::ElapsedTime::deltaMilliseconds( m_timeMarker, now );
        if ( elapsed >= m_flashRateMs )
        {
            nextDuration = m_flashRateMs - (elapsed - m_flashRateMs);
        }
        Timer::start( nextDuration );
        m_timeMarker = now;
    }


protected:
    /// Time marker used to account for 'printf' latency in the timer expiration callback.
    uint32_t m_timeMarker;

    /// Current flash rate
    uint32_t m_flashRateMs;

    /// Track my open/closed
    bool m_opened;
};

}  // end namespaces
}
#endif  // end header latch
