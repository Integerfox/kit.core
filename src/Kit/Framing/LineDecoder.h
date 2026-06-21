#ifndef KIT_FRAMING_LINE_DECODER_H_
#define KIT_FRAMING_LINE_DECODER_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Framing/DecoderReader.h"
#include "Kit/System/Assert.h"

///
namespace Kit {
///
namespace Framing {

/** This concrete template class extends the DecoderReader where a frame
    consider one line of PRINTABLE ASCII text followed by a newline character.

    ANY printable ASCII character is accepted as start-of-frame. And end-of-frame
    is a newline character (`\r` or `\n`).  There is NO escape sequence since the
    escape sequence is need to embedded EOF characters - but in this case EOF
    (newline) is NOT a printable ASCII character -->so an escape sequence is
    not meaningful.

    This class is NOT thread safe.

    NOTE: If a non-printable ASCII character is encounter within a frame, the
          current frame is aborted AND a newline character is required before
          accepting/detecting a new SOF character(s).

 */
class LineDecoder : public DecoderReader
{
public:
    /** Constructor.
        NOTES:
            - The size of the workBuffer determines how big of 'chunks' data is
              read from the "input source", i.e. it is a working buffer and
              does NOT have to be the size of the maximum possible input frame.

            - If the 'convertTabs' argument is set to a value OTHER than a tab
              character ('\t'), then any tab characters encounter will be
              converted to the value of 'convertTabs'. If 'convertTabs' is set
              to a tab character, then tabs are not converted - which means that
              if tabs are encountered, they will be treated as a non-printable
              character and cause the current frame to be aborted.
     */
    LineDecoder( ISource&           source,
                 uint8_t*           workBuffer,
                 Kit::Type::SSize_T sizeOfWorkBuffer,
                 char               convertTabs = '\t' )
        : DecoderReader( source, workBuffer, sizeOfWorkBuffer )
        , m_frameDetected( false )
        , m_convertTabs( convertTabs )
    {
        KIT_SYSTEM_ASSERT( convertTabs == '\t' || ( convertTabs >= ' ' && convertTabs <= '~' ) );
    }

protected:
    /// See Kit::Framing::IDecoder
    bool isStartOfFrame( uint8_t byte ) noexcept override;

    /// See Kit::Framing::IDecoder
    bool isEndOfFrame( uint8_t byte ) noexcept override;

    /// See Kit::Framing::IDecoder
    bool isEscapeByte( uint8_t byte ) noexcept override;

    /// See Kit::Framing::IDecoder
    bool isLegalByte( uint8_t byte ) noexcept override;

    /// See Kit::Framing::DecoderReader
    void initializeScan() noexcept override;

protected:
    /// Helper method to convert tabs when requested
    void convertTabs( uint8_t& incomingByte ) noexcept;

protected:
    /// Track if I have encountered SOF character(s) without an EOF character
    bool m_frameDetected;

    /// Remember my tabs option
    char m_convertTabs;
};


}  // end namespaces
}
#endif  // end header latch
