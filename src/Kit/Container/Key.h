#ifndef KIT_CONTAINER_KEY_H_
#define KIT_CONTAINER_KEY_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include <stdint.h>
#include <stdlib.h>
#include <limits.h>

///
namespace Kit {
///
namespace Container {


/** This abstract class defines the interface that a contained object must
    support if it has comparable key associated with it.
 */
class Key
{
public:
    /** Key Compare function.  Returns <0, 0, >0 if this key is less than,
        equal, or greater than respectively to the specified key!
        The actual type of the 'key' is up to the client sub-class
        that implements this interface.  It is the responsibility of
        the sub-class to correctly define/interpret the data type of the key.

        CAUTION: If the key being compared is NOT a compatible type or is not
                 a valid key, then INT_MIN is returned. The caller of the method
                 is responsible for checking for INT_MIN

     */
    virtual int compareKey( const Key& key ) const noexcept = 0;


    /** Returns the object's length (in bytes) and a pointer to the start of
        key data.  If 'returnRawKeyLenPtr' is null, then no length is
        returned.
     */
    virtual const void* getRawKey( unsigned* returnRawKeyLenPtr = 0 ) const noexcept = 0;

public:
    /// Ensure a Virtual destructor
    virtual ~Key() {}
};


/////////////////////////////////////////////////////////////////////////////
/** This template class is used to generate Key classes for most of the C/C++
    primitive data types.  This class is NOT intended for general use - it is
    used create the typedefs that follow.
 */
template <class DATATYPE>
class KeyPlainType : public Key
{
public:
    /// Constructor
    KeyPlainType( DATATYPE initialValue = 0 ) noexcept
        : m_keyData( initialValue )
    {
    }


public:
    /// Updates the Key's content value
    void setValue( DATATYPE newValue ) noexcept
    {
        m_keyData = newValue;
    }

    /// Returns the Key's content value
    DATATYPE getKeyValue() const noexcept
    {
        return m_keyData;
    }

public:
    /// See Kit::Container::Key
    int compareKey( const Key& key ) const noexcept override
    {
        unsigned        len = 0;
        const DATATYPE* ptr = static_cast<const DATATYPE*>( key.getRawKey( &len ) );
        if ( len != sizeof( DATATYPE ) )
        {
            return INT_MIN;  // Not a valid key
        }

        if ( m_keyData < *ptr )
        {
            return -1;
        }
        else if ( m_keyData > *ptr )
        {
            return 1;
        }

        return 0;
    }

    /// See Kit::Container::Key
    const void* getRawKey( unsigned* returnRawKeyLenPtr = 0 ) const noexcept override
    {
        if ( returnRawKeyLenPtr != 0 )
        {
            *returnRawKeyLenPtr = sizeof( DATATYPE );
        }

        return &m_keyData;
    }


protected:
    /// Storage for the key
    DATATYPE m_keyData;
};


//
// Pre-defined types to some of the primitive C/C++ data types
//

/// Pre-defined key
typedef KeyPlainType<int> KeyInteger_T;

/// Pre-defined key
typedef KeyPlainType<unsigned> KeyUnsigned_T;

/// Pre-defined key
typedef KeyPlainType<long> KeyLong_T;

/// Pre-defined key
typedef KeyPlainType<unsigned long> KeyULong_T;

/// Pre-defined key
typedef KeyPlainType<size_t> KeySizet_T;

/// Pre-defined key
typedef KeyPlainType<int8_t> KeyInteger8_T;

/// Pre-defined key
typedef KeyPlainType<uint8_t> KeyUinteger8_T;

/// Pre-defined key
typedef KeyPlainType<int16_t> KeyInteger16_T;

/// Pre-defined key
typedef KeyPlainType<uint16_t> KeyUinteger16_T;

/// Pre-defined key
typedef KeyPlainType<int32_t> KeyInteger32_T;

/// Pre-defined key
typedef KeyPlainType<uint32_t> KeyUinteger32_T;

/// Pre-defined key
typedef KeyPlainType<int64_t> KeyInteger64_T;

/// Pre-defined key
typedef KeyPlainType<uint64_t> KeyUinteger64_T;


/////////////////////////////////////////////////////////////////////////////
/** This class provides a 'Key' wrapper for a array of Character of length N,
    i.e. a string that is NOT null terminated.  Keys of this type can used to
    compare against other KeyStringBuffer, KeyLiteralString, or Items that use
    a Kit::Text::String as their key
 */
class KeyStringBuffer : public Key
{
public:
    /// Constructor
    KeyStringBuffer( const char* startOfString, size_t lenOfStringInBytes ) noexcept;

public:
    /** Returns the Key's content value. Note: The returned values is NOT
        a null terminated string!
     */
    const char* getKeyValue( size_t& lenOfStringInBytes ) const noexcept
    {
        lenOfStringInBytes = m_len;
        return m_stringKeyPtr;
    }

public:
    /// Generic compare function for strings and string buffers
    static int compare( const char* myString, unsigned myLen, const char* otherString, unsigned otherLen ) noexcept;


public:
    ///  See Kit::Container::key
    int compareKey( const Key& key ) const noexcept override;

    ///  See Kit::Container::key
    const void* getRawKey( unsigned* returnRawKeyLenPtr = 0 ) const noexcept override;

public:
    /// Pointer to the key's storage. Note: Making the member variable public because it makes things simpler (and yes it technically a hack)
    const char* m_stringKeyPtr;

    /// Length of the string (does NOT include the null terminator!).  See previous comment about being a public member variable.
    size_t m_len;
};


/** This class provides a 'Key' wrapper for a C string literal.  Keys
    of this type can used to compare against other KeyLiteralString,
    KeyStringBuffer, or Items that use a Kit::Text::String as their key.
 */
class KeyLiteralString : public KeyStringBuffer
{
public:
    /// Constructor
    KeyLiteralString( const char* string ) noexcept;

public:
    /// Returns the Key's content value
    const char* getKeyValue() const noexcept { return m_stringKeyPtr; }

    /// Cast to read-only character string pointer.
    operator const char*() const { return m_stringKeyPtr; }

    /// Returns a Read-only pointer to the "raw" (short-hand for getKeyValue())
    inline const char* operator()() const { return m_stringKeyPtr; }
};


}  // end namespaces
}
#endif  // end header latch
