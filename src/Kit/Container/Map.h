#ifndef KIT_CONTAINER_MAP_H_
#define KIT_CONTAINER_MAP_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Container/AvlTree_.h"


///
namespace Kit {
///
namespace Container {

/** This template class implements an Map using an AVL Binary tree
    (i.e. a sorted list with fast searches, inserts, and deletes).

    NOTE: The MAP does not allow entries with duplicate key value (i.e. the
          insert() method will fail when attempting to add an item with a
          duplicate key value as existing item in the MAP).

    Template ARGS:
        ITEM    - Data type of the object stored in the Map.  'ITEM' must
                  be a sub-class of the Cpl::Container::MapItem base
                  class.
 */

template <class ITEM>
class Map
{
private:
    /// Delegate operations to the generic tree implementation
    AvlTree_ m_tree;

public:
    /// Constructor.
    Map() noexcept {}

    /** This is a special constructor for when the Map is
        statically declared (i.e. it is initialized as part of
        C++ startup BEFORE main() is executed.  C/C++ guarantees
        that all statically declared data will be initialized
        to zero by default (see r.8.4 in C++ Programming Language,
        Second Edition).  Since the head & tail pointers are
        initialized to zero - then the C/C++ default is OK.  Why
        do I care about all this?  Because if you attempt to build
        static list(s), then the order of when the Map is
        constructed vs. when the static elements are added to the
        list is a problem!  To avoid this problem, a alternate
        constructor that does NOT initialize the head & tail pointers
        is provided.  It assumes that the pointers are already set
        zero because the list is "static". Whew!
     */
    Map( const char* ignoreThisParameter_usedToCreateAUniqueConstructor ) noexcept
        : m_tree( ignoreThisParameter_usedToCreateAUniqueConstructor )
    {
        // intentially DO NOTHING
    }

public:  ///@name Operations to manage items in the Map
    ///@{
    /** Inserts an item into the tree.  If the node is successfully inserted,
        then the method return true.  If the tree already contains a node
        with the same key, then the method returns false.
     */
    bool insert( ITEM& node ) noexcept
    {
        return m_tree.insert( node );
    }

    /** Removes the node (if it exists) that has the matching key. Returns
        the node removed from the tree or 0 if no key match was found.
     */
    ITEM* remove( const Key& keyOfItemToDelete ) noexcept
    {
        ITEM* nodePtr = find( keyOfItemToDelete );
        if ( nodePtr )
        {
            return (ITEM*)m_tree.removeItem( *nodePtr );
        }
        return nullptr;
    }

    /** Removes the specified item from the tree.  Returns true
        when the node was found and removed; else false is
        returned (i.e. node not exists in the tree).
     */
    bool removeItem( ITEM& node ) noexcept
    {
        return m_tree.removeItem( node ) != nullptr;
    }

    /** Searches for a item with a matching key.  Returns the node that
        matches, else 0.
     */
    ITEM* find( const Key& keyToFind ) const noexcept
    {
        return (ITEM*)m_tree.find( keyToFind );
    }

    /** Returns true if the specified item is in the map; else false
        is returned.
     */
    bool isInMap( ITEM& node ) const noexcept
    {
        return node.isInContainer_( this );
    }


    /// Returns the first item in the tree. Returns 0 if tree is empty
    ITEM* first() const noexcept
    {
        return (ITEM*)m_tree.first();
    }


    /// Returns the last item in the tree. Returns 0 if tree is empty
    ITEM* last() const noexcept
    {
        return (ITEM*)m_tree.last();
    }


    /// Returns the next item in the tree.  Returns 0 if at the end-of-tree
    ITEM* next( ITEM& current ) const noexcept
    {
        return (ITEM*)m_tree.next( current );
    }


    /// Returns the previous item in the tree.  Returns 0 if at the start-of-tree
    ITEM* previous( ITEM& current ) const noexcept
    {
        return (ITEM*)m_tree.previous( current );
    }

    /** Removes the first item in the list.  Returns 0 if the list
        is empty.
     */
    ITEM* getFirst() noexcept
    {
        ITEM* nodePtr = first();
        if ( nodePtr )
        {
            return (ITEM*)m_tree.removeItem( *nodePtr );
        }
        return nullptr;
    }


    /** Removes the last item in the list.  Returns 0 if the list
        is empty.
     */
    ITEM* getLast() noexcept
    {
        ITEM* nodePtr = last();
        if ( nodePtr )
        {
            return (ITEM*)m_tree.removeItem( *nodePtr );
        }
        return nullptr;
    }
    ///@}


public:  ///@name Operations on the Map itself
    ///@{
    /// Moves the content of the this Map to the specified Map.
    void move( Map<ITEM>& dst )
    {
        // clear the destination map
        dst.clearTheMap();

        // Copy each item (so the debug info is correct)
        ITEM* nextPtr;
        while ( ( nextPtr = getFirst() ) )
        {
            dst.insert( *nextPtr );
        }
    }

    /// Empties the Map.  All references to the item(s) in the Map are lost.
    void clearTheMap()
    {
        // Drain map so the debug traps work correctly
        while ( getFirst() )
        {
            ;
        }
    }
    ///@}


protected:  ///@name Prevent the container from being copied
    ///@{
    /// Prevent access to the copy constructor -->Containers can not be copied!
    Map( const Map<ITEM>& m ) = delete;

    /// Prevent access to the assignment operator -->Containers can not be copied!
    Map<ITEM>& operator=( const Map<ITEM>& m ) = delete;

    /// Prevent access to the move constructor -->Containers can not be implicitly moved!
    Map( Map<ITEM>&& m ) = delete;

    /// Prevent access to the move assignment operator -->Containers can not be implicitly moved!
    Map<ITEM>& operator=( Map<ITEM>&& m ) = delete;
    ///@}
};



}       // end namespaces
}
#endif  // end header latch
