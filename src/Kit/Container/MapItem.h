#ifndef KIT_CONTAINER_MAP_ITEM_H_
#define KIT_CONTAINER_MAP_ITEM_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Container/KeyedItem.h"


///
namespace Kit {
///
namespace Container {


/** This abstract class represents a item that can be contained in
    an Map (aka a sorted list implemented using an AVL tree).  The client
    sub-class is required to implement the 'getKey()' method (from the
    parent class KeyedItem).

    NOTE: Map Items CAN be used with SList or DList containers
 */
class MapItem : public KeyedItem
{
protected:
    /// Magic values for balance status
    enum Balance_T : int8_t
    {
        eLEFT_  = -1,  //!< Left child is deeper than right child
        eEVEN_  = 0,   //!< Left and right children are the same depth
        eRIGHT_ = 1    //!< Right child is deeper than left child
    };

protected:
    /// Constructor
    MapItem() noexcept;

    /** Constructor -->special constructor to allow a Map to be
        statically allocated.  Only the Sorted List itself should ever
        use this constructor -->not intended for Items in the list
     */
    MapItem( const char* ignoreThisParameter_usedToCreateAUniqueConstructor ) noexcept
        : KeyedItem( ignoreThisParameter_usedToCreateAUniqueConstructor ) {}

    /// Initialize the node when being inserted in the tree
    void initialize( MapItem* parent ) noexcept;

protected:  // Helper methods
    /// Get tree connection/pointer
    MapItem* getParent() const { return static_cast<MapItem*>( m_parentPtr_ ); }

    /// Set tree connection/pointer
    void setParent( MapItem* n ) { m_parentPtr_ = n; }

    /// Get tree connection/pointer
    MapItem* getLeft() const { return static_cast<MapItem*>( m_prevPtr_ ); };

    /// Set tree connection/pointer
    void setLeft( MapItem* n ) { m_prevPtr_ = n; }

    /// Get tree connection/pointer
    MapItem* getRight() const { return static_cast<MapItem*>( m_nextPtr_ ); };

    /// Set tree connection/pointer
    void setRight( MapItem* n ) { m_nextPtr_ = n; }


protected:
    /// Link field to the parent node.
    void* m_parentPtr_;

    /// Balance status
    Balance_T m_balance_;

    /// I am the root node
    bool m_is_NOT_root_;


    // Allow the AVL Tree class access to my data members
    friend class AvlTree_;
};

}  // end namespaces
}
#endif  // end header latch
