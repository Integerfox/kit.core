#ifndef KIT_TEXT_FSTRING_H_
#define KIT_TEXT_FSTRING_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Text/FStringBase.h"
#include <string.h>

///
namespace Kit {
namespace Text {


/** This template class represents a NULL terminated string of a
    specific length.  The size (aka in the internal storage) of string cannot
    be changed. Any 'write' operations that exceed the length of the internal
    storage - the results are silently truncated and the string is ALWAYS
    left in null terminated state.

    Template Args:  S:=  Max Size of the String WITHOUT the null
                         terminator!

    NOTE: See base class - IString - for a complete listing/description of
          the class's methods.
 */
template <int S>
class FString : public FStringBase
{
public:
    /// Constructor
    FString( const FString<S>& string ) noexcept
        : FStringBase( string.getString(), m_strMem, S ) {}

    /// Constructor
    FString( const IString& string ) noexcept
        : FStringBase( string.getString(), m_strMem, S ) {}

    /// Constructor
    FString( const char* string = "" ) noexcept
        : FStringBase( string, m_strMem, S ) {}

    /// Constructor
    FString( char c ) noexcept
        : FStringBase( c, m_strMem, S ) {}

    /// Constructor
    FString( int num ) noexcept
        : FStringBase( num, m_strMem, S ) {}

    /// Constructor
    FString( unsigned num ) noexcept
        : FStringBase( num, m_strMem, S ) {}

    /// Constructor
    FString( long num ) noexcept
        : FStringBase( num, m_strMem, S ) {}

    /// Constructor
    FString( long long num ) noexcept
        : FStringBase( num, m_strMem, S ) {}

    /// Constructor
    FString( unsigned long num ) noexcept
        : FStringBase( num, m_strMem, S ) {}

    /// Constructor
    FString( unsigned long long num ) noexcept
        : FStringBase( num, m_strMem, S ) {}

public:
    // Add this using declaration to make base class operators visible
    using FStringBase::operator=;

private:
    /// Storage for the string
    char m_strMem[S + 1];
};


}  // end namespaces
}
#endif  // end header latch
