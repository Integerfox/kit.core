/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "LogSink.h"
#include "Kit/Dm/IObserver.h"

using namespace Kit::Logging::Framework;

//////////////////////////////
LogSink::LogSink( Kit::EventQueue::IQueue&                                            myMbox,
                  Kit::Container::RingBufferMP<Kit::Logging::Framework::EntryData_T>& incomingEntriesBuffer ) noexcept
    : Kit::Itc::OpenCloseSync( myMbox )
    , m_obElementCount( myMbox )
    , m_logBuffer( incomingEntriesBuffer )
    , m_opened( false )
{
    m_obElementCount.setCallback<LogSink, &LogSink::elementCountChanged>( this );
}

void LogSink::request( OpenMsg& msg ) noexcept
{
    if ( !m_opened )
    {
        m_opened = true;
        m_logBuffer.m_mpElementCount.attach( m_obElementCount );
    }

    msg.returnToSender();
}

void LogSink::request( CloseMsg& msg ) noexcept
{
    if ( m_opened )
    {
        m_opened = false;
        m_logBuffer.m_mpElementCount.detach( m_obElementCount );
    }

    msg.returnToSender();
}

void LogSink::elementCountChanged( Kit::Dm::Mp::Uint32& mp, Kit::Dm::IObserver& clientObserver ) noexcept
{
    uint32_t count;
    if ( mp.readAndSync( count, clientObserver ) && count > 0 )
    {
        // Temporarily cancel change notifications to avoid redundant notifications while draining the buffer
        mp.detach( m_obElementCount );

        // Drain the buffer (but limit how many adds at one time) and write the entries to persistent storage
        unsigned                             iterations = 0;
        Kit::Logging::Framework::EntryData_T entry;
        uint16_t                             seqNum = mp.getSequenceNumber();
        while ( iterations < OPTION_KIT_LOGGING_FRAMEWORK_MAX_BATCH_WRITE && m_logBuffer.remove( entry, seqNum ) )
        {
            dispatchLogEntry( entry );
            iterations++;
        }

        // Reattach the observer after draining the buffer
        mp.attach( m_obElementCount, seqNum );
    }
}
