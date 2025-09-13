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

//------------------------------------------------------------------------------
namespace Kit {
namespace Io {

///////////////////////////////
TeeOutput::TeeOutput() noexcept
    : m_opened( true )
{
    memset( m_streams, 0, sizeof( m_streams ) );
    memset( m_failed, 0, sizeof( m_failed ) );
}

TeeOutput::TeeOutput( IOutput& streamA ) noexcept
    : TeeOutput()
{
    m_streams[0] = &streamA;
}

TeeOutput::TeeOutput( IOutput& streamA, IOutput& streamB ) noexcept
    : TeeOutput()
{
    m_streams[0] = &streamA;
    m_streams[1] = &streamB;
}

///////////////////////////////
bool TeeOutput::remove( IOutput& stream ) noexcept
{
    if ( !remove( m_streams, stream ) )
    {
        return remove( m_failed, stream );
    }
    return true;
}

bool TeeOutput::add( IOutput** streamList, IOutput& stream ) noexcept
{
    // Find an empty slot
    for ( unsigned i = 0; i < MAX_STREAMS; ++i )
    {
        if ( streamList[i] == nullptr )
        {
            streamList[i] = &stream;
            return true;
        }
    }

    // No empty slot found
    return false;
}

bool TeeOutput::remove( IOutput** streamList, IOutput& stream ) noexcept
{
    for ( unsigned i = 0; i < MAX_STREAMS; ++i )
    {
        if ( streamList[i] == &stream )
        {
            streamList[i] = nullptr;
            return true;
        }
    }

    return false;
}

///////////////////////////////
IOutput* TeeOutput::first( IOutput** streamList ) const noexcept
{
    for ( unsigned i = 0; i < MAX_STREAMS; ++i )
    {
        if ( streamList[i] != nullptr )
        {
            return streamList[i];
        }
    }

    return nullptr;
}

IOutput* TeeOutput::next( IOutput** streamList, IOutput& currentStream ) const noexcept
{
    bool startIdxFound = false;
    for ( unsigned i = 0; i < MAX_STREAMS; ++i )
    {
        if ( !startIdxFound )
        {
            if ( streamList[i] == &currentStream )
            {
                startIdxFound = true;
            }
            continue;
        }

        if ( m_failed[i] != nullptr )
        {
            return m_failed[i];
        }
    }

    return nullptr;
}

///////////////////////////////
bool TeeOutput::write( const void* buffer, ByteCount_T maxBytes, ByteCount_T& bytesWritten ) noexcept
{
    bool     rc     = true;
    IOutput* stream = first( m_streams );

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
    for( unsigned i=0; i<MAX_STREAMS; ++i )
    {
        IOutput* stream = m_streams[i];
        if ( stream == nullptr )
        {
            continue;
        }

        // IOutput to stream -->and trap any error
        ByteCount_T tempWritten;
        if ( !stream->write( buffer, maxBytes, tempWritten ) )
        {
            // move the stream with an error to the failed list
            remove( m_streams, *stream );
            add( m_failed, *stream );
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
    }

    return rc;
}


void TeeOutput::flush() noexcept
{
    for ( unsigned i = 0; i < MAX_STREAMS; ++i )
    {
        if ( m_streams[i] )
        {
            m_streams[i]->flush();
        }
    }
}

bool TeeOutput::isEos()  noexcept
{
    bool rc = false;
    for ( unsigned i = 0; i < MAX_STREAMS; ++i )
    {
        auto* stream = m_streams[i];
        if ( stream && !stream->isEos() )
        {
            // move the stream with an error to the failed list
            remove( m_streams, *stream );
            add( m_failed, *stream );
            rc = true;
        }
    }

    return rc;
}

void TeeOutput::close() noexcept
{
    if ( m_opened )
    {
        // Remember that I am closed
        m_opened = false;

        // Close all the streams (both good and failed)
        for(unsigned i=0; i<MAX_STREAMS; ++i)
        {
            if ( m_streams[i] != nullptr )
            {
                m_streams[i]->close();
            }
            if ( m_failed[i] != nullptr )
            {
                m_failed[i]->close();
            }
        }
    }
}

}  // end namespace
}
//------------------------------------------------------------------------------