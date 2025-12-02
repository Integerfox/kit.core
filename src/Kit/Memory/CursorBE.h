#ifndef KIT_MEMORY_CURSOR_BE_H_
#define KIT_MEMORY_CURSOR_BE_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Memory/CursorBase.h"
#include "Kit/Type/endian.h"

///
namespace Kit {
///
namespace Memory {

/** This 'public' concrete class provides BIG ENDIAN byte ordering semantics
    for a Cursor. The Cursor class properly handles multi-byte alignment and
    Endianess issues when copying multi-byte data to/from a memory buffer. See
    the CursorBase class for additional details
  */
class CursorBE : public CursorBase
{
public:
    /// Constructor
    CursorBE( void* srcBuffer, size_t srcBufferSize, size_t startingOffset = 0 )
        : CursorBase( srcBuffer, srcBufferSize, startingOffset )
    {
    }


public:
    /// Read a single word. Returns false if there was an error
    inline bool readU16( uint16_t& dstData ) noexcept
    {
        bool result = readAndAdvance( &dstData, sizeof( dstData ) );
        dstData     = KIT_TYPE_BE16TOH( dstData );
        return result;
    }

    /// Read a single word. Returns false if there was an error
    inline bool readI16( int16_t& dstData ) noexcept
    {
        bool result = readAndAdvance( &dstData, sizeof( dstData ) );
        dstData     = KIT_TYPE_BE16TOH( dstData );
        return result;
    }

    /// Read a double word. Returns false if there was an error
    inline bool readU32( uint32_t& dstData ) noexcept
    {
        bool result = readAndAdvance( &dstData, sizeof( dstData ) );
        dstData     = KIT_TYPE_BE32TOH( dstData );
        return result;
    }

    /// Read a double word. Returns false if there was an error
    inline bool readI32( int32_t& dstData ) noexcept
    {
        bool result = readAndAdvance( &dstData, sizeof( dstData ) );
        dstData     = KIT_TYPE_BE32TOH( dstData );
        return result;
    }

    /// Read a quad word. Returns false if there was an error
    inline bool readU64( uint64_t& dstData ) noexcept
    {
        bool result = readAndAdvance( &dstData, sizeof( dstData ) );
        dstData     = KIT_TYPE_BE64TOH( dstData );
        return result;
    }

    /// Read a quad word. Returns false if there was an error
    inline bool readI64( int64_t& dstData ) noexcept
    {
        bool result = readAndAdvance( &dstData, sizeof( dstData ) );
        dstData     = KIT_TYPE_BE64TOH( dstData );
        return result;
    }

    /// Read a Real32. Returns false if there was an error
    inline bool readF32( float& dstData ) noexcept
    {
        bool result = readAndAdvance( &dstData, sizeof( dstData ) );
        dstData     = KIT_TYPE_BEF32TOH( dstData );
        return result;
    }

    /// Read a Real64. Returns false if there was an error
    inline bool readF64( double& dstData ) noexcept
    {
        bool result = readAndAdvance( &dstData, sizeof( dstData ) );
        dstData     = KIT_TYPE_BEF64TOH( dstData );
        return result;
    }

public:
    /// Write a single word. Returns false if there was an error
    inline bool writeU16( uint16_t srcData ) noexcept
    {
        auto tempData = KIT_TYPE_HTOBE16( srcData );
        return writeAndAdvance( &tempData, sizeof( tempData ) );
    }

    /// Write a single word. Returns false if there was an error
    inline bool writeI16( int16_t srcData ) noexcept
    {
        auto tempData = KIT_TYPE_HTOBE16( srcData );
        return writeAndAdvance( &tempData, sizeof( tempData ) );
    }

    /// Write a double word. Returns false if there was an error
    inline bool writeU32( uint32_t srcData ) noexcept
    {
        auto tempData = KIT_TYPE_HTOBE32( srcData );
        return writeAndAdvance( &tempData, sizeof( tempData ) );
    }

    /// Write a double word. Returns false if there was an error
    inline bool writeI32( int32_t srcData ) noexcept
    {
        auto tempData = KIT_TYPE_HTOBE32( srcData );
        return writeAndAdvance( &tempData, sizeof( tempData ) );
    }

    /// Write a quad word. Returns false if there was an error
    inline bool writeU64( uint64_t srcData ) noexcept
    {
        auto tempData = KIT_TYPE_HTOBE64( srcData );
        return writeAndAdvance( &tempData, sizeof( tempData ) );
    }

    /// Write a quad word. Returns false if there was an error
    inline bool writeI64( int64_t srcData ) noexcept
    {
        auto tempData = KIT_TYPE_HTOBE64( srcData );
        return writeAndAdvance( &tempData, sizeof( tempData ) );
    }

    /// Write a Real32. Returns false if there was an error
    inline bool writeF32( float srcData ) noexcept
    {
        auto tempData = KIT_TYPE_HTOBEF32( srcData );
        return writeAndAdvance( &tempData, sizeof( tempData ) );
    }

    /// Write a Real64. Returns false if there was an error
    inline bool writeF64( double srcData ) noexcept
    {
        auto tempData = KIT_TYPE_HTOBEF64( srcData );
        return writeAndAdvance( &tempData, sizeof( tempData ) );
    }
};

}  // end namespaces
}
#endif  // end header latch