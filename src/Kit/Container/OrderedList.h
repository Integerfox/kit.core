#ifndef KIT_CONTAINER_ORDEREDLIST_H_
#define KIT_CONTAINER_ORDEREDLIST_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Container/OrderedListBase.h"

///
namespace Kit {
///
namespace Container {

/** This template class implements a double linked list which maintains the
    ordering imposed on it by the application. It is type-safe wrapper around
    the OrderedListBase class.

    NOTE: KEYED_ITEM must be a subclass of Kit::Container::KeyedItem.
 */
template <class KEYED_ITEM>
class OrderedList : public OrderedListBase
{
public:
    /// Public constructor initializes head and tail pointers.
    OrderedList() noexcept
        : OrderedListBase() {}

    /** This is a special constructor for when the list is statically declared
        (i.e. it is initialized as part of C++ startup BEFORE main() is
        executed.  See Kit::Container::OrderedListBase for details.
     */
    OrderedList( const char* ignoreThisParameter_usedToCreateAUniqueConstructor ) noexcept
        : OrderedListBase( ignoreThisParameter_usedToCreateAUniqueConstructor ) {}

public:
    /// Moves the content of the this queue to the specified queue.
    void move( OrderedList<KEYED_ITEM>& dst ) noexcept { OrderedListBase::move( dst ); }

    /// Empties the list.  All references to the item(s) in the list are lost.
    void clearTheList() noexcept { OrderedListBase::clearTheList(); }

public:
    /** Removes the first item in the list.  Returns nullptr if the list
        is empty.
     */
    KEYED_ITEM* getFirst() noexcept { return static_cast<KEYED_ITEM*>(OrderedListBase::getFirst()); }

    /** Removes the last item in the list.  Returns nullptr if the list
        is empty.
     */
    KEYED_ITEM* getLast() noexcept { return static_cast<KEYED_ITEM*>(OrderedListBase::getLast()); }

    /** Remove specified KEYED_ITEM element from the list. Returns true if the
        specified element was found and removed from the list, else false.
     */
    bool remove( KEYED_ITEM& item ) noexcept { return OrderedListBase::remove( item ); }

    /** Insert the KEYED_ITEM "item" in 'order' into the list
     */
    void insert( KEYED_ITEM& item ) noexcept { OrderedListBase::insert( item ); }

    /// Returns true if the specified item is already in the list, else false.
    bool find( const KEYED_ITEM& item ) const noexcept { return OrderedListBase::find( item ); }

    /** Return a pointer to the first item in the list. The returned item
        remains in the list.  Returns nullptr if the list is empty.
     */
    KEYED_ITEM* first() const noexcept { return static_cast<KEYED_ITEM*>(OrderedListBase::first()); }

    /** Return a pointer to the last item in the list. The returned item remains
        in the list.  Returns nullptr if the list is empty.
     */
    KEYED_ITEM* last() const noexcept { return static_cast<KEYED_ITEM*>(OrderedListBase::last()); }

    /** Return a pointer to the item after the "item". Both items remain in
        the list.  Returns nullptr when the end-of-list is reached.
     */
    KEYED_ITEM* next( const KEYED_ITEM& item ) const noexcept { return static_cast<KEYED_ITEM*>(OrderedListBase::next( item )); }

    /** Return a pointer to the item before the "item". Both items remain in
        the list.  Returns nullptr when the front-of-list is reached.
     */
    KEYED_ITEM* previous( const KEYED_ITEM& item ) const noexcept { return static_cast<KEYED_ITEM*>(OrderedListBase::previous( item )); }

private:
    /// Prevent access to the copy constructor -->Containers can not be copied!
    OrderedList( const OrderedList<KEYED_ITEM>& m ) = delete;

    /// Prevent access to the assignment operator -->Containers can not be copied!
    OrderedList<KEYED_ITEM>& operator=( const OrderedList<KEYED_ITEM>& m ) = delete;

    /// Prevent access to the move constructor -->Containers can not be implicitly moved!
    OrderedList( OrderedList<KEYED_ITEM>&& m ) = delete;

    /// Prevent access to the move assignment operator -->Containers can not be implicitly moved!
    OrderedList<KEYED_ITEM>& operator=( OrderedList<KEYED_ITEM>&& m ) = delete;
};


};  // namespace Container
};  // namespace Kit
#endif  // end header latch