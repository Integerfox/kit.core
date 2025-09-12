/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "TeeOutput.h"
#include "Kit/Io/IOutput.h"

//------------------------------------------------------------------------------
namespace Kit {
namespace Io {

///////////////////////////////
TeeOutput::TeeOutput() noexcept
    : m_opened( true )
{
}

TeeOutput::TeeOutput( IOutput& streamA ) noexcept
    : m_opened( true )
{
    m_streams.put( streamA );
}

TeeOutput::TeeOutput( IOutput& streamA, IOutput& streamB ) noexcept
    : m_opened( true )
{
    m_streams.put( streamA );
    m_streams.put( streamB );
}

///////////////////////////////
void TeeOutput::add( IOutput& stream ) noexcept
{
    m_streams.put( stream );
}

bool TeeOutput::remove( IOutput& stream ) noexcept
{
    if ( !m_streams.remove( stream ) )
    {
        return m_failed.remove( stream );
    }

    return true;
}

///////////////////////////////
IOutput* TeeOutput::firstFailed() noexcept
{
    return m_failed.first();
}

IOutput* TeeOutput::nextFailed( IOutput& currentFailedStream ) noexcept
{
    return m_failed.next( currentFailedStream );
}


IOutput* TeeOutput::removeAndGetNextFailed( IOutput& currentFailedStream ) noexcept
{
    IOutput* next = m_failed.next( currentFailedStream );
    if ( !m_failed.remove( currentFailedStream ) )
    {
        return nullptr;
    }

    return next;
}


///////////////////////////////
bool TeeOutput::write( const void* buffer, int maxBytes, int& bytesWritten ) noexcept
{
    bool     rc     = true;
    IOutput* stream = m_streams.first();

    // Handle the special case of NO output streams
    if ( stream == nullptr )
    {
        // In this case -->act like the Null IOutput stream
        bytesWritten = maxBytes;
        return m_opened;  // Normally I return true, but If I have been closed -->I need to return false
    }

    // Ensure 'bytesWritten' starts at zero (so the trap-max-value logic works)
    bytesWritten = 0;

    // Loop through all active streams
    while ( stream )
    {
        // Cache the next stream in the 'active list'
        IOutput* next = m_streams.next( *stream );

        // IOutput to stream -->and trap any error
        int tempWritten;
        if ( !stream->write( buffer, maxBytes, tempWritten ) )
        {
            // move the stream with an error to the failed list
            m_streams.remove( *stream );
            m_failed.put( *stream );
            rc = false;
        }

        // Write succeeded -->trap max number of bytes written.
        else
        {
            if ( tempWritten > bytesWritten )
            {
                bytesWritten = tempWritten;
            }
        }

        // Continue with the next stream in the active list
        stream = next;
    }

    return rc;
}


void TeeOutput::flush() noexcept
{
    IOutput* stream = m_streams.first();
    while ( stream )
    {
        stream->flush();
        stream = m_streams.next( *stream );
    }
}

bool TeeOutput::isEos() const noexcept
{
    bool     rc     = false;
    IOutput* stream = m_streams.first();
    while ( stream )
    {
        IOutput* next = m_streams.next( *stream );
        if ( stream->isEos() )
        {
            // move the stream with an error to the failed list
            m_streams.remove( *stream );
            m_failed.put( *stream );
            rc = true;
        }
        stream = next;
    }

    return rc;
}

void TeeOutput::close() noexcept
{
    if ( m_opened )
    {
        // Remember that I am closed
        m_opened = false;

        // Close my active streams
        IOutput* stream = m_streams.first();
        while ( stream )
        {
            stream->close();
            stream = m_streams.next( *stream );
        }

        // Close any failed streams
        stream = m_failed.first();
        while ( stream )
        {
            stream->close();
            stream = m_failed.next( *stream );
        }
    }
}

}  // end namespace
}
//------------------------------------------------------------------------------