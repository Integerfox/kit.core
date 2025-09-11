#ifndef KIT_TEXT_DSTRING_H
#define KIT_TEXT_DSTRING_H
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "kit_config.h"
#include "Kit/Text/StringBase.h"

///
namespace Kit {
///
namespace Text {


/** This magic constant defines the default block size that the DString class
    uses when allocating memory, i.e. the size of all chunks of memory allocated
    is a multiple of the block size.
 */
#ifndef OPTION_KIT_TEXT_DSTRING_ALLOC_BLOCK_SIZE
#define OPTION_KIT_TEXT_DSTRING_ALLOC_BLOCK_SIZE 16
#endif


/** This concrete class implements a simple "dynamic storage" IString Type.
    All memory is allocated from the heap.  For memory allocation errors,
    the following happens:
        1) The _truncated flag is set to true.

        2) If the error occurred in the constructor, then the internal string
           is set an empty string. If the error occurred because of a requested
           size increase, the internal string is simply truncated.
 */
class DString : public StringBase
{
public:
    /** Constructor.  The amount of storage initially allocated for the string is
        the maximum of the size of the source string AND the value of initialSize.
        Whatever value is chosen, it is then rounded up to the nearest block size
        multiple.  There are two main reasons to specify an initialSize.
        1) To reduce the number of malloc/free that occur over the life of the
           IString. If you supply a large enough initialSize - no additional 
           mallocs will be needed.  
        2) If you plan to use the format(..) methods.  The format() methods will
           NOT allocate additional storage.  To prevent the format() methods from 
           truncating, you must start with a large enough 'buffer'.
           
        The block size parameter can be used to control the size of the 'chunks'
        memory is allocated in.  This 'blocking' paradigm helps to reduce
        fragmentation and number of internal malloc/free operations.

        NOTE: The space reserved for the trailing null terminator is part of the
              block calculation, i.e. if your initial size is 16, and your
              block size is 16, then number of bytes allocated is 32 to allow
              space for a 16 byte string and a one byte terminator and then
              rounded up to the next block size.
     */
    DString( const IString& string, int initialSize = 0, int blocksize = OPTION_KIT_TEXT_DSTRING_ALLOC_BLOCK_SIZE ) noexcept;

    /// Constructor.  See above constructor for details
    DString( const DString& string, int initialSize = 0, int blocksize = OPTION_KIT_TEXT_DSTRING_ALLOC_BLOCK_SIZE ) noexcept;

    /// Constructor.  See above constructor for details
    DString( const char* string = "", int initialSize = 0, int blocksize = OPTION_KIT_TEXT_DSTRING_ALLOC_BLOCK_SIZE ) noexcept;

    /// Constructor.  See above constructor for details
    DString( char c, int initialSize = 0, int blocksize = OPTION_KIT_TEXT_DSTRING_ALLOC_BLOCK_SIZE ) noexcept;

    /// Constructor.  See above constructor for details
    DString( int num, int initialSize = 0, int blocksize = OPTION_KIT_TEXT_DSTRING_ALLOC_BLOCK_SIZE ) noexcept;

    /// Constructor.  See above constructor for details
    DString( unsigned num, int initialSize = 0, int blocksize = OPTION_KIT_TEXT_DSTRING_ALLOC_BLOCK_SIZE ) noexcept;

    /// Constructor.  See above constructor for details
    DString( long num, int initialSize = 0, int blocksize = OPTION_KIT_TEXT_DSTRING_ALLOC_BLOCK_SIZE ) noexcept;

    /// Constructor.  See above constructor for details
    DString( long long num, int initialSize = 0, int blocksize = OPTION_KIT_TEXT_DSTRING_ALLOC_BLOCK_SIZE ) noexcept;

    /// Constructor.  See above constructor for details
    DString( unsigned long num, int initialSize = 0, int blocksize = OPTION_KIT_TEXT_DSTRING_ALLOC_BLOCK_SIZE ) noexcept;

    /// Constructor.  See above constructor for details
    DString( unsigned long long num, int initialSize = 0, int blocksize = OPTION_KIT_TEXT_DSTRING_ALLOC_BLOCK_SIZE ) noexcept;

    /// Destructor
    ~DString() noexcept;


public:
    /// Make parent method visible
    using IString::operator=;

    /// Assignment
    IString& operator=( const DString& string ) noexcept;

    /// Assignment
    IString& operator=( int num ) noexcept override;

    /// Assignment
    IString& operator=( unsigned num ) noexcept override;

    /// Assignment
    IString& operator=( long num ) noexcept override;

    /// Assignment
    IString& operator=( unsigned long num ) noexcept override;

public:
    /// Make parent method visible
    using IString::operator+=;

    /// Append
    IString& operator+=( const DString& string ) noexcept;

    /// Append
    IString& operator+=( int num ) noexcept override;

    /// Append
    IString& operator+=( unsigned num ) noexcept override;

    /// Append
    IString& operator+=( long num ) noexcept override;

    /// Append
    IString& operator+=( unsigned long num ) noexcept override;


public:
    /// Override base class
    void copyIn( const char* string, int n ) noexcept override;

    /// Override base class
    void appendTo( const char* string, int n ) noexcept override;

    /// Override base class
    void insertAt( int insertOffset, const char* stringToInsert ) noexcept override;

    /// Override base class
    int maxLength() const noexcept override;


protected:  // Helper methods
    /** Returns the need memory size in "block units".  Note: The size calculation
        includes the memory for the trailing '\0' string terminator.
     */
    inline int calcMemSize( int len ) noexcept { return ( ( len + m_blockSize ) / m_blockSize ) * m_blockSize; }

    /// Frees the current string memory - IF it was previously allocated
    void freeCurrentString( void ) noexcept;

    /// Returns the max length of internal WITHOUT the '\0' string terminator
    inline int maxStrLen( void ) const noexcept { return m_storageLen - 1; }

    /// Validates the just created string is 'valid'
    void validateAndCopy( const char* string, int len ) noexcept;

protected:
    /// Block size in bytes
    int m_blockSize;

    /// Size, in bytes, of allocated storage
    int m_storageLen;
};


}       // end namespaces
}
#endif  // end header latch
