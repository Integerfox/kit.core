/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "LineDecoder.h"


//------------------------------------------------------------------------------
namespace Kit {
namespace Framing {
    
void LineDecoder::initializeScan() noexcept
{
    DecoderReader::initializeScan();
    m_frameDetected = false;
}

bool LineDecoder::isStartOfFrame( uint8_t byte ) noexcept
{
    // Reset my frame flag when encountering an newline
    if ( isEndOfFrame( byte ) )
    {
        m_frameDetected = false;
    }

    // Skip printable ASCII characters if the frame was aborted due to an non printable
    // character. Requires EOF to be encountered before accepting a new SOF character.
    if ( !m_frameDetected )
    {
        // Convert tabs
        convertTabs( byte );

        // Printable ASCII character?
        if ( byte >= ' ' && byte <= '~' )
        {
            // Adjust my internal data pointer/len since I am NOT discarding the SOF character
            m_dataPtr--;
            m_dataLen++;
            m_frameDetected = true;
            return true;
        }
    }

    return false;
}

bool LineDecoder::isEndOfFrame( uint8_t byte ) noexcept
{
    return byte == '\r' || byte == '\n';
}

bool LineDecoder::isEscapeByte( uint8_t byte ) noexcept
{
    return false;
}

bool LineDecoder::isLegalByte( uint8_t byte ) noexcept
{
    // Convert tabs to whitespace
    convertTabs( byte );

    // Newline is always 'valid'
    if ( isEndOfFrame( byte ) )
    {
        m_frameDetected = false;
        return true;
    }

    // Reject non-printable ASCII characters
    if ( byte < ' ' || byte > '~' )
    {
        return false;
    }

    // If I get here character is a printable ASCII character
    return true;
}

// Convert tab to space - when requested
void LineDecoder::convertTabs( uint8_t& incomingByte ) noexcept
{
    if ( m_convertTabs != '\t' && incomingByte == '\t' )
    {
        incomingByte = m_convertTabs;
        *m_dataPtr   = m_convertTabs;  // Tab was converted -->update the decoder's internal buffer with the converted value
    }
}

}  // end namespace
}
//------------------------------------------------------------------------------