#ifndef KIT_CONTAINER_DLIST_H_
#define KIT_CONTAINER_DLIST_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Container/DListBase.h"

///
namespace Kit {
///
namespace Container {

/** This template class implements a singly linked list which maintains the
    ordering imposed on it by the application. It is type-safe wrapper around
    the DListBase class.

    NOTE: ITEM must be a subclass of Kit::Container::ExtendedItem.
 */
template <class ITEM>
class DList : public DListBase
{
public:
    /// Public constructor initializes head and tail pointers.
    DList() noexcept
        : DListBase() {}

    /** This is a special constructor for when the list is statically declared
        (i.e. it is initialized as part of C++ startup BEFORE main() is
        executed.  See Kit::Container::DListBase for details.
     */
    DList( const char* ignoreThisParameter_usedToCreateAUniqueConstructor ) noexcept
        : DListBase( ignoreThisParameter_usedToCreateAUniqueConstructor ) {}

public:
    /// Moves the content of the this queue to the specified queue.
    void move( DList<ITEM>& dst ) noexcept { DListBase::move( dst ); }

    /// Empties the list.  All references to the item(s) in the list are lost.
    void clearTheList() noexcept { DListBase::clearTheList(); }

public:
    /// Removes the first item in the list.  Returns nullptr if the list is empty.
    ITEM* get() noexcept { return static_cast<ITEM*>(DListBase::getFirst()); }

    /// Adds the item as the last item in the list
    void put( ITEM& item ) noexcept { DListBase::putLast( item ); }

    /** Return a pointer to the first item in the list. The returned item
        remains in the list.  Returns nullptr if the list is empty.
     */
    ITEM* head() const noexcept { return static_cast<ITEM*>(DListBase::first()); }

    /** Return a pointer to the last item in the list. The returned item
        remains in the list.  Returns nullptr if the list is empty.
     */
    ITEM* tail() const noexcept { return static_cast<ITEM*>(DListBase::last()); }

public:
    /** Removes the top element from stack and return a pointer to it as a
        result. Returns nullptr, if the stack is empty
     */
    ITEM* pop() noexcept { return static_cast<ITEM*>(DListBase::getFirst()); }

    /// Adds the ITEM item to top of the stack.
    void push( ITEM& item ) noexcept { DListBase::putFirst( item ); }

    /** Return a pointer to the top ITEM item in the stack. The returned item
        remains in the queue.  Returns nullptr if the stack is empty.
     */
    ITEM* top() const noexcept { return static_cast<ITEM*>(DListBase::first()); }

public:
    /** Removes the first item in the list.  Returns nullptr if the list
        is empty.
     */
    ITEM* getFirst() noexcept { return static_cast<ITEM*>(DListBase::getFirst()); }

    /** Removes the last item in the list.  Returns nullptr if the list
        is empty.
     */
    ITEM* getLast() noexcept { return static_cast<ITEM*>(DListBase::getLast()); }

    /// Adds the item as the first item in the list.
    void putFirst( ITEM& item ) noexcept { DListBase::putFirst( item ); }

    /// Adds the item as the last item in the list.
    void putLast( ITEM& item ) noexcept { DListBase::putLast( item ); }

    /** Remove specified ITEM element from the list. Returns true if the
        specified element was found and removed from the list, else false.
     */
    bool remove( ITEM& item ) noexcept { return DListBase::remove( item ); }

    /** Insert the "item" ITEM into the list behind the "after" ITEM element.
        If 'after' is nullptr, then 'item' is added to the head of the list.
     */
    void insertAfter( ITEM& after, ITEM& item ) noexcept { DListBase::insertAfter( after, item ); }

    /** Insert the "item" ITEM into the list ahead of the "before" ITEM element.
        If 'before' is nullptr, then 'item' is added to the tail of the list.

        Note: This insert operation is more expensive than insertAfter() because
        a traversal of the list is required to find the 'before' item
     */
    void insertBefore( ITEM& before, ITEM& item ) noexcept { DListBase::insertBefore( before, item ); }

    /// Returns true if the specified item is already in the list, else false.
    bool find( const ITEM& item ) const noexcept { return DListBase::find( item ); }

    /** Return a pointer to the first item in the list. The returned item
        remains in the list.  Returns nullptr if the list is empty.
     */
    ITEM* first() const noexcept { return static_cast<ITEM*>(DListBase::first()); }

    /** Return a pointer to the last item in the list. The returned item remains
        in the list.  Returns nullptr if the list is empty.
     */
    ITEM* last() const noexcept { return static_cast<ITEM*>(DListBase::last()); }

    /** Return a pointer to the item after the "item". Both items remain in
        the list.  Returns nullptr when the end-of-list is reached.
     */
    ITEM* next( const ITEM& item ) const noexcept { return static_cast<ITEM*>(DListBase::next( item )); }

    /** Return a pointer to the item before the "item". Both items remain in
        the list.  Returns nullptr when the front-of-list is reached.
     */
    ITEM* previous( const ITEM& item ) const noexcept { return static_cast<ITEM*>(DListBase::previous( item )); }

private:
    /// Prevent access to the copy constructor -->Containers can not be copied!
    DList( const DList<ITEM>& m ) = delete;

    /// Prevent access to the assignment operator -->Containers can not be copied!
    DList<ITEM>& operator=( const DList<ITEM>& m ) = delete;

    /// Prevent access to the move constructor -->Containers can not be implicitly moved!
    DList( DList<ITEM>&& m ) = delete;

    /// Prevent access to the move assignment operator -->Containers can not be implicitly moved!
    DList<ITEM>& operator=( DList<ITEM>&& m ) = delete;
};


};  // namespace Container
};  // namespace Kit
#endif  // end header latch