#ifndef KIT_FRAMING_DECODER_READER_H_
#define KIT_FRAMING_DECODER_READER_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Framing/IDecoder.h"


///
namespace Kit {
///
namespace Framing {

/** This partially concrete class implements the basic scanning/reading logic
    for a Decoder
 */
class DecoderReader : public IDecoder
{
protected:
    /** Constructor. The size of the workBuffer determines how big of
        'chunks' data is read from the "input source", i.e. it is a working
        buffer and does NOT have to be the size of the maximum possible input
        frame.
     */
    DecoderReader( uint8_t* workBuffer, Kit::Type::SSize_T sizeOfWorkBuffer )
    : m_dataPtr( nullptr )
    , m_buffer( workBuffer )
    , m_framePtr( nullptr )
    , m_dataLen( 0 )
    , m_frameSize( 0 )
    , m_bufSize( sizeOfWorkBuffer )
    , m_inFrame( false )
    , m_escaping( false )
    {
        initializeScan();
    }

public:
    /// See Kit::Framing::IDecoder
    virtual bool scan( Kit::Type::SSize_T  maxSizeOfFrameBuffer,
                       uint8_t*            frameBuffer,
                       Kit::Type::SSize_T& frameSize ) noexcept override;

    /// See Kit::Framing::IDecoder
    virtual bool scan( Kit::Type::SSize_T  maxSizeOfFrameBuffer,
                       uint8_t*            frameBuffer,
                       Kit::Type::SSize_T& frameSize,
                       bool&               isEof ) noexcept override;

    /// See Kit::Framing::IDecoder
    virtual bool oobRead( uint8_t*            dstBuffer,
                          Kit::Type::SSize_T  numBytes,
                          Kit::Type::SSize_T& bytesRead ) noexcept override;

protected:
    /// Returns true if at start-of-frame
    virtual bool isStartOfFrame( uint8_t byte ) noexcept = 0;

    /// Returns true if at end-of-frame
    virtual bool isEndOfFrame( uint8_t byte ) noexcept = 0;

    /// Returns true if the start of the start of a escape sequence has been detected
    virtual bool isEscapeByte( uint8_t byte ) noexcept = 0;

    /// Returns true if the current byte is a legal/valid within a frame
    virtual bool isLegalByte( uint8_t byte ) noexcept = 0;

    /** Attempts to read the specified number of bytes from the "input source"
        into the supplied buffer.  The actual number of bytes read is returned via
        'bytesRead'. Returns true if successful, or false if End-of-Input
        was encountered.
     */
    virtual bool read( void*               dstBuffer,
                       Kit::Type::SSize_T  numBytes,
                       Kit::Type::SSize_T& bytesRead ) noexcept = 0;

    /** Returns the un-encoded value for the specified escaped byte.  The
        default implementation simply returns 'escapedByte'
     */
    virtual uint8_t decodeEscapedByte( uint8_t escapedByte ) noexcept;

protected:
    /// Helper method to initialize frame processing
    virtual void initializeScan() noexcept;

protected:
    /// Pointer to the next unprocessed character in my raw input buffer
    uint8_t* m_dataPtr;

    /// Work buffer for reading characters in 'chunks' from my Input source (i.e. minimize the calls to read())
    uint8_t* m_buffer;

    /// Pointer to the next decoded frame character
    uint8_t* m_framePtr;

    /// Current number of characters remaining in my work buffer
    Kit::Type::SSize_T m_dataLen;

    /// Number of bytes current decoded for the frame
    Kit::Type::SSize_T m_frameSize;

    /// Size of my work buffer
    Kit::Type::SSize_T m_bufSize;

    /// Flag: I am currently in a Frame
    bool m_inFrame;

    /// Flag: the next character is an escape character
    bool m_escaping;
};


}  // end namespaces
}
#endif  // end header latch
