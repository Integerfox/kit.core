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

using namespace Cpl::Logging;

//////////////////////////////
LogSink::LogSink( Cpl::Dm::MailboxServer&                                    myMbox,
                  Cpl::Container::RingBufferMP<Cpl::Logging::EntryData_T>&   incomingEntriesBuffer ) noexcept
    : Cpl::Itc::CloseSync( myMbox )
    , m_observerElementCount( *((Cpl::Dm::EventLoop*) &myMbox), *this, &LogSink::elementCountChanged )
    , m_logBuffer( incomingEntriesBuffer )
    , m_opened( false )
{
}

void LogSink::request( OpenMsg& msg )
{
    if ( !m_opened )
    {
        m_opened = true;
        m_logBuffer.m_mpElementCount.attach( m_observerElementCount );
    }

    msg.returnToSender();
}

void LogSink::request( CloseMsg& msg )
{
    if ( m_opened )
    {
        m_opened = false;
        m_logBuffer.m_mpElementCount.detach( m_observerElementCount );
    }

    msg.returnToSender();
}

void LogSink::elementCountChanged( Cpl::Dm::Mp::Uint32& mp, Cpl::Dm::SubscriberApi& clientObserver ) noexcept
{
    uint32_t count;
    if ( mp.readAndSync( count, clientObserver ) && count > 0 )
    {
        // Drain the buffer (but limit how many adds at one time) and write the entries to persistent storage
        unsigned                   iterations = 0;
        Cpl::Logging::EntryData_T  entry;
        while ( iterations < OPTION_CPL_LOGGING_LOGSINK_MAX_BATCH_WRITE && m_logBuffer.remove( entry ) )
        {
            dispatchLogEntry( entry );
            iterations++;
        }
    }
}
