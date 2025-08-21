#ifndef KIT_TEXT_FSTRING_ITEM_H_
#define KIT_TEXT_FSTRING_ITEM_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Text/StringItem.h"
#include "Kit/Text/FString.h"


///
namespace Kit {
///
namespace Text {



/** This template class implements the StringItem class for a 'String' using
    a 'FString' as the internal storage for String instance.

    Template Args:  S:=  Max Size of the String WITHOUT the null
                         terminator!
 */
template <int S>
class FStringItem : public StringItem
{
protected:
    /// Underlying String
    FString<S> m_internalString;

public:
    /// Constructor
    FStringItem( const IString& string ) noexcept :StringItem( m_internalString ), m_internalString( string ) {}

    /// Constructor
    FStringItem( const char* string="" ) noexcept :StringItem( m_internalString ), m_internalString( string ) {}


private:
    /// Prevent access to the copy constructor -->Container Items should not be copied!
    FStringItem( const FStringItem& m ) noexcept;

    /// Prevent access to the assignment operator -->Container Items should not be copied!
    const FStringItem& operator=( const FStringItem& m ) noexcept;

    /// Prevent access to the move constructor -->Container Items should not be moved!
    FStringItem( FStringItem&& m ) noexcept = delete;

    /// Prevent access to the move assignment operator -->Container Items should not be moved!
    const FStringItem& operator=( FStringItem&& m ) noexcept = delete;
};


}       // end namespaces
}
#endif  // end header latch
