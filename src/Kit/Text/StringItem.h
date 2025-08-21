#ifndef KIT_TEXT_STRING_ITEM_H_
#define KIT_TEXT_STRING_ITEM_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Container/MapItem.h"
#include "Kit/Text/IString.h"


///
namespace Kit {
///
namespace Text {



/** This partially concrete provides a wrapper class that allows a IString
    object to be stored directly in one the KIT Containers.  A StringItem
    can be stored in a SList, DList, and Map.

    Notes:

        o The allocation of the actual IString is deferred to child classes.

        o When storing a StringItem in a 'keyed' container (i.e. Map) the 
          application MUST NOT change the value of the internal string since it
          is the 'key' value that is used to determine its placement in the Map. 
          Modifying the internal IString while the StringItem is in a Map will 
          CORRUPT the Container.

        o StringItem instances CANNOT be copied.  This is to avoid potential
          Container corruption since the individual Items contain the memory
          for the Containers' linkage, i.e. a copied container Item is
          analogous to storing a single Item in more than one Container at the
          same time.
 */
class StringItem : public Kit::Container::MapItem
{
protected:
    /// Constructor
    StringItem( IString& string ) noexcept :m_myString( string ) {}

public:
    /// Returns a reference to the actual IString
    inline IString& get() const noexcept { return m_myString; }

    /// Returns a pointer to the internal C string
    inline const char* getString() const noexcept { return m_myString.getString(); }

    /// Cast to a IString reference
    inline operator IString& ( ) const noexcept { return m_myString; }

    /// Cast to read-only character string pointer.
    inline operator const char* ( ) const noexcept { return m_myString.getString(); }

    /// Returns a Read-only pointer to the "raw" (short-hand for getString())
    inline const char* operator()() const noexcept { return m_myString.getString(); }


public:
    /// Return Kit::Container::Key (A IString is-a Key)
    const Kit::Container::Key& getKey() const noexcept { return m_myString; }


protected:
    /// Reference to the actual IString/storage
    IString & m_myString;


private:
    /// Prevent access to the copy constructor -->Container Items should not be copied!
    StringItem( const StringItem& m ) noexcept = delete;

    /// Prevent access to the assignment operator -->Container Items should not be copied!
    const StringItem& operator=( const StringItem& m ) noexcept = delete;

    /// Prevent access to the move constructor -->Container Items should not be moved!
    StringItem( StringItem&& m ) noexcept = delete;

    /// Prevent access to the move assignment operator -->Container Items should not be moved!
    const StringItem& operator=( StringItem&& m ) noexcept = delete; 
};


}       // end namespaces
} 
#endif  // end header latch
