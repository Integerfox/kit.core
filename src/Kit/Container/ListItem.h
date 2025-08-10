#ifndef KIT_CONTAINER_LIST_ITEM_H_
#define KIT_CONTAINER_LIST_ITEM_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Container/Item.h"

///
namespace Kit {
///
namespace Container {


/** This class is used by the Container classes to implement a various types
    of linked lists containers.
 */
class ListItem : public Item
{
protected:
    /// Constructor
    ListItem() noexcept
        : m_nextPtr_( nullptr ) {}

    /** Constructor used ONLY with the child class MapItem: -->special
        constructor to allow a Map to be statically allocated.  Only the Map
        itself should ever use this constructor -->not intended for Items in a
        Map
     */
    ListItem( const char* ignoreThisParameter_usedToCreateAUniqueConstructor ) noexcept
        : Item( ignoreThisParameter_usedToCreateAUniqueConstructor ) {}

public:
    /// The link field.
    ListItem* m_nextPtr_;
};

/** This class is used by the Container classes to implement a various types of
    DOUBLY linked containers.
 */

class ExtendedListItem : public ListItem
{
protected:
    /// Constructor
    ExtendedListItem() noexcept
        : m_prevPtr_( nullptr ) {}

    /** Constructor used ONLY with the child class MapItem: -->special
        constructor to allow a Map to be statically allocated.  Only the Map
        itself should ever use this constructor -->not intended for Items in a
        Map
     */
    ExtendedListItem( const char* ignoreThisParameter_usedToCreateAUniqueConstructor ) noexcept
        : ListItem( ignoreThisParameter_usedToCreateAUniqueConstructor ) {}

public:
    /// The previous link field.
    ExtendedListItem* m_prevPtr_;
};


/** This template class defines wrapper class - that is makes a reference
    'listable'. This class is useful when the Application needs to put a single
    entity into multiple containers.

    Template Arguments:
        REFITEM    - The type of the Reference being wrapped.
        ITEMTYPE   - The Item/Container type, e.g. ListItem, ExtendedListItem, MapItem, etc.
 */
template <class REFITEM, class ITEMTYPE>
class ReferenceItem : public ITEMTYPE
{
public:
    /// Reference to the item that is being 'containerized'
    REFITEM& m_reference;

    /// Constructor
    ReferenceItem( REFITEM& item ) noexcept
        : ITEMTYPE(), m_reference( item ) {}

    /** Constructor used ONLY with the child class MapItem: -->special
        constructor to allow a Map to be statically allocated.  Only the Map
        itself should ever use this constructor -->not intended for Items in a
        Map
     */
    ReferenceItem( REFITEM& item, const char* ignoreThisParameter_usedToCreateAUniqueConstructor ) noexcept
        : ITEMTYPE( ignoreThisParameter_usedToCreateAUniqueConstructor ), m_reference( item ) {}
};

}  // end namespaces
}
#endif  // end header latch
