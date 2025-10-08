#ifndef KIT_CONTAINER_SLIST_BASE_H_
#define KIT_CONTAINER_SLIST_BASE_H_
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

///
namespace Kit {
///
namespace Container {

/** This protected base class implements a NON-TYPE-SAFE a singly linked list
    which maintains the ordering imposed on it by the application.  It is
    intended to be the base class for a type-safe template class. Having
    a non-templatized base class reduces the memory footprint when an
    application has more than one type specific SListBase class
 */
class SListBase
{
protected:
    /// Constructor initializes head and tail pointers.
    SListBase() noexcept
        : m_headPtr( nullptr ), m_tailPtr( nullptr ) {}

    /** This is a special constructor for when the list is
        statically declared (i.e. it is initialized as part of
        C++ startup BEFORE main() is executed.  C/C++ guarantees
        that all statically declared data will be initialized
        to zero by default (see r.8.4 in C++ Programming Language,
        Second Edition).  Since the head & tail pointers are
        initialized to zero - then the C/C++ default is OK.  Why
        do I care about all this?  Because if you attempt to build
        static list(s), then the order of when the list is
        constructed vs. when the static elements are added to the
        list is a problem!  To avoid this problem, a alternate
        constructor that does NOT initialize the head & tail pointers
        is provided.  It assumes that the pointers are already set
        zero because the list is "static". Whew!
     */
    SListBase( const char* ignoreThisParameter_usedToCreateAUniqueConstructor ) noexcept
    {
        // intentially DO NOTHING
    }

protected:
    /// Moves the content of the this queue to the specified queue.
    void move( SListBase& dst ) noexcept;

    /// Empties the list.  All references to the item(s) in the list are lost.
    void clearTheList() noexcept;


protected:
    /// Removes the first item in the list.  Returns nullptr if the list is empty.
    ListItem* getFirst() noexcept;

    /// Removes the last item in the list.  Returns nullptr if the list is empty.
    ListItem* getLast() noexcept;

    /// Adds the item as the last item in the list
    void putFirst( ListItem& item ) noexcept;
    
    /// Adds the item as the last item in the list
    void putLast( ListItem& item ) noexcept;

protected:
    /** Return a pointer to the first item in the list.
        The returned item remains in the list.  Returns nullptr
        if the list is empty.
     */
    ListItem* first() const noexcept
    {
        return m_headPtr;
    }

    /** Return a pointer to the last item in the list.
        The returned item remains in the list.  Returns nullptr
        if the list is empty.
     */
    ListItem* last() const noexcept
    {
        return m_tailPtr;
    }

protected:
    /** Remove specified ListItem element from the list.
        Returns true if the specified element was found and
        removed from the list, else false.
     */
    bool remove( ListItem& item ) noexcept;

    /** Insert the "item" ListItem into the list behind the "after" ListItem element.  
        If 'after' is nullptr, then 'item' is added to the head of the list.
     */
    void insertAfter( ListItem& after, ListItem& item ) noexcept;

    /** Insert the "item" ListItem into the list ahead of the "before" ListItem element. 
        If 'before' is nullptr, then 'item' is added to the tail of the list.  
        Note: This insert operation is more expensive than insertAfter() because
        a traversal of the list is required to find the 'before' item
     */
    void insertBefore( ListItem& before, ListItem& item ) noexcept;

    /// Returns true if the specified item is already in the list, else false.
    bool find( const ListItem& item ) const noexcept
    {
        return item.isInContainer_( this );
    }

    /** Return a pointer to the item after the item "item". Both items remain
        in the list.  Returns nullptr when the end-of-list is reached.

        NOTE: If 'item' is not in the list, then a fatal error is generated.
     */
    ListItem* next( const ListItem& item ) const noexcept;

protected:
    /// Points to the first item in the list.
    ListItem* m_headPtr;

    /// Points to the last item in the list.
    ListItem* m_tailPtr;

private:
    /// Prevent access to the copy constructor -->Containers can not be copied!
    SListBase( const SListBase& m ) = delete;

    /// Prevent access to the assignment operator -->Containers can not be
    /// copied!
    SListBase& operator=( const SListBase& m ) = delete;

    /// Prevent access to the move constructor -->Containers can not be implicitly moved!
    SListBase( SListBase&& m ) = delete;

    /// Prevent access to the move assignment operator -->Containers can not be implicitly moved!
    SListBase& operator=( SListBase&& m ) = delete;
};


}  // end namespaces
}
#endif  // end header latch