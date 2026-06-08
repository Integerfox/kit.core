/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "EncoderWriter.h"

//------------------------------------------------------------------------------
namespace Kit {
namespace Framing {

/////////////
bool EncoderWriter::startFrame( void ) noexcept
{
    // End the current frame before starting a new one
    if ( m_inFrame )
    {
        if ( !endFrame() )
        {
            return false;  // Failed to end the current frame
        }
    }

    if ( !m_dst.startOutput() )
    {
        return false;  // Failed to start the new frame
    }

    // NOT skipping the SOF byte
    if ( !m_skipSendingSof )
    {
        if ( !m_dst.appendOutput( m_sof ) )
        {
            return false;  // Failed to output the SOF byte
        }
    }
    m_inFrame = true;
    return true;
}

bool EncoderWriter::output( const void* srcBuffer, Kit::Type::SSize_T numBytes ) noexcept
{
    if ( !m_inFrame || srcBuffer == nullptr || numBytes < 0 )
    {
        return false;  // Frame has not been started or source buffer is null
    }

    const uint8_t* srcBytePtr = static_cast<const uint8_t*>( srcBuffer );
    for ( Kit::Type::SSize_T i = 0; i < numBytes; ++i )
    {
        uint8_t srcByte = *srcBytePtr++;

        // Escape the byte if it is a special character
        if ( srcByte == m_sof || srcByte == m_eof || srcByte == m_esc )
        {
            if ( !m_dst.appendOutput( m_esc ) )
            {
                m_inFrame = false;
                return false;  // Failed to output the escape character
            }
            srcByte = encodeEscapedByte( srcByte );
        }

        // Write the next byte
        if ( !m_dst.appendOutput( srcByte ) )
        {
            m_inFrame = false;
            return false;  // Failed to output the byte
        }
    }

    return true;
}

bool EncoderWriter::endFrame( void ) noexcept
{
    // Frame has not been started
    if ( !m_inFrame )
    {
        return false;
    }

    // Success or Error - the frame sequence is ended
    m_inFrame = false;

    // Output the End-of-Frame character
    if ( !m_dst.appendOutput( m_eof ) )
    {
        return false;  // Failed to output the EOF byte
    }

    if ( !m_dst.endOutput() )
    {
        return false;  // Failed to end the frame
    }

    return true;
}

uint8_t EncoderWriter::encodeEscapedByte( uint8_t byteToBeEscaped ) noexcept
{
    return byteToBeEscaped;
}

}  // end namespace
}
//------------------------------------------------------------------------------