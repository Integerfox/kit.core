/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Key.h"
#include "Kit/System/Assert.h"

//------------------------------------------------------------------------------
namespace Kit {
namespace Container {

KeyStringBuffer::KeyStringBuffer( const char* startOfString, size_t lenOfStringInBytes ) noexcept
    : m_stringKeyPtr( startOfString )
    , m_len( lenOfStringInBytes )
{
    KIT_SYSTEM_ASSERT( m_stringKeyPtr != nullptr );
}

int KeyStringBuffer::compareKey( const Key& key ) const noexcept
{
    unsigned    otherLen = 0;
    const char* otherPtr = static_cast<const char*>(key.getRawKey( &otherLen ));
    return compare( m_stringKeyPtr, m_len, otherPtr, otherLen );
}


const void* KeyStringBuffer::getRawKey( unsigned* returnRawKeyLenPtr ) const noexcept
{
    if ( returnRawKeyLenPtr )
    {
        *returnRawKeyLenPtr = m_stringKeyPtr ? m_len : 0;
    }

    return m_stringKeyPtr;
}

int KeyStringBuffer::compare( const char* myString, unsigned myLen, const char* otherString, unsigned otherLen ) noexcept
{
    if ( otherString )
    {
        if ( myString )
        {
            unsigned cmpLen      = myLen > otherLen ? otherLen : myLen;
            int      comparision = strncmp( myString, otherString, cmpLen );

            if ( comparision == 0 && myLen != otherLen )
            {
                return myLen - static_cast<int>(otherLen);
            }

            return comparision;
        }
    }

    // Not a valid key
    return INT_MIN;
}

}   // end namespaces
}  
//------------------------------------------------------------------------------