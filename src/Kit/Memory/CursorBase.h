#ifndef KIT_MEMORY_CURSOR_BASE_H_
#define KIT_MEMORY_CURSOR_BASE_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include <cstdint>
#include <stdint.h>
#include <stdlib.h>

///
namespace Kit {
///
namespace Memory {

/** This internal concrete class provides common methods that are used for
    extracting and inserting data from/to a Memory Buffer.  Because there are no
    guaranties on the address alignment of data in the Memory buffer - this class
    uses memcpy() operations to read/write data from/to the Memory buffer to avoid
    memory address alignment errors when operating on multibyte data.

    A 'public' child class is required to provide the 'Endian' semantics for the
    memory buffer - i.e. BIG ENDIAN or LITTLE ENDIAN - when reading/writing. It
    is the public child class that provides the methods to read/write multi-byte
    data types (e.g. uint16_t, uint32_t, float, double, etc).

    Usage:
        1. Create an cursor instance specifying a memory buffer and optionally a
           starting offset into the buffer.
        2. Use the various methods to read/write data from/to the
           memory Buffer.  The cursor maintains a SINGLE internal buffer pointer
           that is advanced by the size of each read/write operation.
        3. The cursor will not 'overrun' the maximum memory buffer size.  If an
           overrun is attempted - the cursor enters an error state and all
           subsequent read/write operations will fail.  The error state can
           be cleared by calling setPosition() with a valid offset.

  */
class CursorBase
{
protected:
    /** Constructor. 'srcBuffer' must be a valid pointer to a memory buffer and
        'srcBufferSize' must be less than SIZE_MAX
     */
    CursorBase( void* srcBuffer, size_t srcBufferSize, size_t startingOffset );


public:
    /** Returns true if the cursor is in an error state (e.g. an 'overrun' was
        attempted).  Once the cursor is in an error state - it can only be
        'clear' by calling setPosition() with a valid 'newOffset'
     */
    bool errorOccurred() const noexcept;

    /// Sets the internal pointer to a new offset.  Note: this WILL clear an existing error condition.
    bool setPosition( size_t newOffset ) noexcept;

    /// Returns the current position.  Note: 'position' is a ZERO based index/value
    inline size_t getPosition() const noexcept
    {
        return m_bufPtr - m_baseBufPtr;
    }

    /// Returns the 'remaining byte' count
    inline size_t getRemainingCount() const noexcept
    {
        return m_bytesRemaining;
    }

    /** Advances the internal offset 'numBytes'. Does NOT clear any existing
        error condition. However if the advance would cause an overrun - the
        cursor enters an error state and return false.
     */
    bool skip( size_t numBytes );

    /// Returns the maximum size of the internal buffer, i.e. the original value provided in the Constructor
    inline size_t getBufferSize() const noexcept
    {
        return m_bufferSize;
    }

public:
    /// Read a single byte. Returns false if there was an error
    inline bool readU8( uint8_t& dstData ) noexcept
    {
        return readAndAdvance( &dstData, sizeof( dstData ) );
    }

    /// Read a single byte. Returns false if there was an error
    inline bool readI8( int8_t& dstData ) noexcept
    {
        return readAndAdvance( &dstData, sizeof( dstData ) );
    }

    /// Read N bytes. Returns false if there was an error
    inline bool read( void* dstBuffer, size_t numBytesToRead ) noexcept
    {
        return readAndAdvance( dstBuffer, numBytesToRead );
    }

public:
    /// Write a single byte. Returns false if there was an error
    inline bool writeU8( uint8_t srcData ) noexcept
    {
        return writeAndAdvance( &srcData, sizeof( srcData ) );
    }

    /// Write a single byte. Returns false if there was an error
    inline bool writeI8( int8_t srcData ) noexcept
    {
        return writeAndAdvance( &srcData, sizeof( srcData ) );
    }

    /// Write N bytes. Returns false if there was an error
    inline bool write( const void* srcBuffer, size_t numBytesToWrite ) noexcept
    {
        return writeAndAdvance( srcBuffer, numBytesToWrite );
    }

    /// Write 'fillByte' N times (where N='numBytes').  Returns false if there was an error
    bool fill( size_t numBytes, uint8_t fillByte = 0 ) noexcept;

protected:
    /// Helper method
    bool readAndAdvance( void* dstPtr, size_t dstSize ) noexcept;

    /// Helper method
    bool writeAndAdvance( const void* srcPtr, size_t srcSize ) noexcept;

protected:
    /// Start of the physical buffer
    uint8_t* m_baseBufPtr;

    /// Current pointer/offset
    uint8_t* m_bufPtr;

    /// Absolute size of the buffer
    size_t m_bufferSize;

    /// Bytes remaining
    size_t m_bytesRemaining;
};

}  // end namespaces
}
#endif  // end header latch