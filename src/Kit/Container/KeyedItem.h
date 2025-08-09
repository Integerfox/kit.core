#ifndef KIT_CONTAINER_KEYEDITEM_H_
#define KIT_CONTAINER_KEYEDITEM_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Container/ListItem.h"
#include "Kit/Container/Key.h"


///
namespace Kit {
///
namespace Container {


/** This abstract class represents a item that can be used with containers that
    have a concept of Key-Value-Pair (e.g. a sorted map).  All elements in
    'KVP' container must have the same 'Key Type'.

    NOTE: Keyed Items CAN be used with SList or DList containers
 */
class KeyedItem: public ExtendedListItem
{
protected:
    /// Constructor
    KeyedItem() noexcept {}

    /** Constructor used ONLY with the child class MapItem: -->special
        constructor to allow a Map to be statically allocated.  Only the Map
        itself should ever use this constructor -->not intended for Items in a
        Map
     */
    KeyedItem( const char* /* ignoreThisParameter_usedToCreateAUniqueConstructor */ ) noexcept {}


public:
    /// This method returns a reference to the Item's Key instance. 
    virtual const Key& getKey() const noexcept = 0;
};

}       // end namespaces
} 
#endif  // end header latch
