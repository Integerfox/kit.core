#ifndef KIT_FRAMING_ENCODER_WRITER_H_
#define KIT_FRAMING_ENCODER_WRITER_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Framing/IEncoder.h"
#include "Kit/Framing/IDestination.h"
#include "Kit/System/Assert.h"

///
namespace Kit {
///
namespace Framing {

/** This concrete class implements the basic framing/output logic
    for an Encoder.  The provided IDestination instances determines the "output
    destination"
 */
class EncoderWriter : public IEncoder
{
public:
    /** Constructor. The size of the workBuffer determines how big of
        'chunks' data is read from the "input source", i.e. it is a working
        buffer and does NOT have to be the size of the maximum possible input
        frame.
     */
    EncoderWriter( IDestination& destination,
                   uint8_t       startOfFrame,
                   uint8_t       endOfFrame,
                   uint8_t       escapeByte,
                   bool          skipSendingSof = false )
        : m_dst( destination )
        , m_sof( startOfFrame )
        , m_eof( endOfFrame )
        , m_esc( escapeByte )
        , m_inFrame( false )
        , m_skipSendingSof( skipSendingSof )
    {
        KIT_SYSTEM_ASSERT( m_sof != m_eof );
        KIT_SYSTEM_ASSERT( m_sof != m_esc );
        KIT_SYSTEM_ASSERT( m_eof != m_esc );
    }

public:
    /// See Kit::Framing::IEncoder
    bool startFrame( void ) noexcept override;

    /// See Kit::Framing::IEncoder
    bool output( const void* srcBuffer, Kit::Type::SSize_T numBytes ) noexcept override;

    /// See Kit::Framing::IEncoder
    bool endFrame( void ) noexcept override;

protected:
    /** Returns the encoded/escaped value for the specified special character.
        The default implementation simply returns 'byteToBeEscaped'
     */
    virtual uint8_t encodeEscapedByte( uint8_t byteToBeEscaped ) noexcept;

protected:
    /// Reference to the output destination
    IDestination& m_dst;

    /// Start-of-Frame character
    uint8_t m_sof;

    /// End-of-Frame character
    uint8_t m_eof;

    /// Escape character
    uint8_t m_esc;

    /// Flag: I am currently in a Frame
    bool m_inFrame;

    /** Flag: Skip sending Start-of-Frame character. This flag should ONLY be
        set when the child class's framing protocol supports multiple SOF characters.
        See TBD as example of such a protocol.
     */
    bool m_skipSendingSof;
};


}  // end namespaces
}
#endif  // end header latch
