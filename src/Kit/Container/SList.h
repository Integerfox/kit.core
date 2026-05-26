#ifndef KIT_CONTAINER_SLIST_H_
#define KIT_CONTAINER_SLIST_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Container/SListBase.h"

///
namespace Kit {
///
namespace Container {

/** This template class implements a singly linked list which maintains the
    ordering imposed on it by the application. It is type-safe wrapper around
    the SListBase class.

    NOTE: LIST_ITEM must be a subclass of Kit::Container::ListItem.
 */
template <class LIST_ITEM>
class SList : public SListBase
{
public:
    /// Public constructor initializes head and tail pointers.
    SList() noexcept
        : SListBase() {}

    /** This is a special constructor for when the list is statically declared
        (i.e. it is initialized as part of C++ startup BEFORE main() is
        executed.  See Kit::Container::SListBase for details.
     */
    SList( const char* ignoreThisParameter_usedToCreateAUniqueConstructor ) noexcept
        : SListBase( ignoreThisParameter_usedToCreateAUniqueConstructor ) {}

public:
    /// Moves the content of the this queue to the specified queue.
    void move( SList<LIST_ITEM>& dst ) noexcept { SListBase::move( dst ); }

    /// Empties the list.  All references to the item(s) in the list are lost.
    void clearTheList() noexcept { SListBase::clearTheList(); }

public:
    /// Removes the first item in the list.  Returns nullptr if the list is empty.
    LIST_ITEM* get() noexcept { return static_cast<LIST_ITEM*>( SListBase::getFirst() ); }

    /// Adds the item as the last item in the list
    void put( LIST_ITEM& item ) noexcept { SListBase::putLast( item ); }

    /** Return a pointer to the first item in the list. The returned item
        remains in the list.  Returns nullptr if the list is empty.
     */
    LIST_ITEM* head() const noexcept { return static_cast<LIST_ITEM*>( SListBase::first() ); }

    /** Return a pointer to the last item in the list. The returned item
        remains in the list.  Returns nullptr if the list is empty.
     */
    LIST_ITEM* tail() const noexcept { return static_cast<LIST_ITEM*>( SListBase::last() ); }

public:
    /** Removes the top element from stack and return a pointer to it as a
        result. Returns nullptr, if the stack is empty
     */
    LIST_ITEM* pop() noexcept { return static_cast<LIST_ITEM*>( SListBase::getFirst() ); }

    /// Adds the LIST_ITEM item to top of the stack.
    void push( LIST_ITEM& item ) noexcept { SListBase::putFirst( item ); }

    /** Return a pointer to the top LIST_ITEM item in the stack. The returned item
        remains in the queue.  Returns nullptr if the stack is empty.
     */
    LIST_ITEM* top() const noexcept { return static_cast<LIST_ITEM*>( SListBase::first() ); }

public:
    /** Removes the first item in the list.  Returns nullptr if the list
        is empty.
     */
    LIST_ITEM* getFirst() noexcept { return static_cast<LIST_ITEM*>( SListBase::getFirst() ); }

    /** Removes the last item in the list.  Returns nullptr if the list
        is empty.
     */
    LIST_ITEM* getLast() noexcept { return static_cast<LIST_ITEM*>( SListBase::getLast() ); }

    /// Adds the item as the first item in the list.
    void putFirst( LIST_ITEM& item ) noexcept { SListBase::putFirst( item ); }

    /// Adds the item as the last item in the list.
    void putLast( LIST_ITEM& item ) noexcept { SListBase::putLast( item ); }

    /** Remove specified LIST_ITEM element from the list. Returns true if the
        specified element was found and removed from the list, else false.
     */
    bool remove( LIST_ITEM& item ) noexcept { return SListBase::remove( item ); }

    /** Insert the "item" LIST_ITEM into the list behind the "after" LIST_ITEM element.
        If 'after' is nullptr, then 'item' is added to the head of the list.
     */
    void insertAfter( LIST_ITEM& after, LIST_ITEM& item ) noexcept { SListBase::insertAfter( after, item ); }

    /** Insert the "item" LIST_ITEM into the list ahead of the "before" LIST_ITEM element.
        If 'before' is nullptr, then 'item' is added to the tail of the list.

        Note: This insert operation is more expensive than insertAfter() because
        a traversal of the list is required to find the 'before' item
     */
    void insertBefore( LIST_ITEM& before, LIST_ITEM& item ) noexcept { SListBase::insertBefore( before, item ); }

    /// Returns true if the specified item is already in the list, else false.
    bool find( const LIST_ITEM& item ) const noexcept { return SListBase::find( item ); }

    /** Return a pointer to the first item in the list. The returned item
        remains in the list.  Returns nullptr if the list is empty.
     */
    LIST_ITEM* first() const noexcept { return static_cast<LIST_ITEM*>( SListBase::first() ); }

    /** Return a pointer to the last item in the list. The returned item remains
        in the list.  Returns nullptr if the list is empty.
     */
    LIST_ITEM* last() const noexcept { return static_cast<LIST_ITEM*>( SListBase::last() ); }

    /** Return a pointer to the item after the item "item". Both items remain in
        the list.  Returns nullptr when the end-of-list is reached.
     */
    LIST_ITEM* next( const LIST_ITEM& item ) const noexcept { return static_cast<LIST_ITEM*>( SListBase::next( item ) ); }

private:
    /// Prevent access to the copy constructor -->Containers can not be copied!
    SList( const SList<LIST_ITEM>& m ) = delete;

    /// Prevent access to the assignment operator -->Containers can not be copied!
    SList<LIST_ITEM>& operator=( const SList<LIST_ITEM>& m ) = delete;

    /// Prevent access to the move constructor -->Containers can not be implicitly moved!
    SList( SList<LIST_ITEM>&& m ) = delete;

    /// Prevent access to the move assignment operator -->Containers can not be implicitly moved!
    SList<LIST_ITEM>& operator=( SList<LIST_ITEM>&& m ) = delete;
};


};  // namespace Container
};  // namespace Kit
#endif  // end header latch