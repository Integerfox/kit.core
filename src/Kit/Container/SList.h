#ifndef KIT_CONTAINER_SLIST_H_
#define KIT_CONTAINER_SLIST_H_
/*-----------------------------------------------------------------------------
 * This file is part of the Colony.Core Project.  The Colony.Core Project is an
 * open source project with a BSD type of licensing agreement.  See the license
 * agreement (license.txt) in the top/ directory or on the Internet at
 * http://integerfox.com/colony.core/license.txt
 *
 * Copyright (c) 2014-2025  John T. Taylor
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Container/SListBase_.h"

///
namespace Kit {
///
namespace Container {

/** This template class implements a singly linked list which maintains the
    ordering imposed on it by the application. It is type-safe wrapper around
    the SListBase_ class.

    NOTE: ITEM must be a subclass of Item.
 */
template <class ITEM>
class SList : public SListBase_
{
public:
    /// Public constructor initializes head and tail pointers.
    SList() noexcept
        : SListBase_() {}

    /** This is a special constructor for when the list is statically declared
        (i.e. it is initialized as part of C++ startup BEFORE main() is
        executed.  See Kit::Container::SListBase_ for details.
     */
    SList( const char* ignoreThisParameter_usedToCreateAUniqueConstructor ) noexcept
        : SListBase_( ignoreThisParameter_usedToCreateAUniqueConstructor ) {}

public:
    /// Moves the content of the this queue to the specified queue.
    void move( SList<ITEM>& dst ) noexcept { SListBase_::move( dst ); }

    /// Empties the list.  All references to the item(s) in the list are lost.
    void clearTheList() noexcept { SListBase_::clearTheList(); }

public:
    /// Removes the first item in the list.  Returns nullptr if the list is empty.
    ITEM* get( void ) noexcept { return (ITEM*)SListBase_::getFirst(); }

    /// Adds the item as the last item in the list
    void put( ITEM& item ) noexcept { SListBase_::putLast( item ); }

    /** Return a pointer to the first item in the list. The returned item
        remains in the list.  Returns nullptr if the list is empty.
     */
    ITEM* head( void ) const noexcept { return (ITEM*)SListBase_::first(); }

    /** Return a pointer to the last item in the list. The returned item
        remains in the list.  Returns nullptr if the list is empty.
     */
    ITEM* tail( void ) const noexcept { return (ITEM*)SListBase_::last(); }

public:
    /** Removes the top element from stack and return a pointer to it as a
        result. Returns nullptr, if the stack is empty
     */
    ITEM* pop( void ) noexcept { return (ITEM*)SListBase_::getFirst(); }

    /// Adds the ITEM item to top of the stack.
    void push( ITEM& item ) noexcept { SListBase_::putFirst( item ); }

    /** Return a pointer to the top ITEM item in the stack. The returned item
        remains in the queue.  Returns nullptr if the stack is empty.
     */
    ITEM* top( void ) const noexcept { return (ITEM*)SListBase_::first(); }

public:
    /** Removes the first item in the list.  Returns nullptr if the list
        is empty.
     */
    ITEM* getFirst( void ) noexcept { return (ITEM*)SListBase_::getFirst(); }

    /** Removes the last item in the list.  Returns nullptr if the list
        is empty.
     */
    ITEM* getLast( void ) noexcept { return (ITEM*)SListBase_::getLast(); }

    /// Adds the item as the first item in the list.
    void putFirst( ITEM& item ) noexcept { SListBase_::putFirst( item ); }

    /// Adds the item as the last item in the list.
    void putLast( ITEM& item ) noexcept { SListBase_::putLast( item ); }

    /** Remove specified ITEM element from the list. Returns true if the
        specified element was found and removed from the list, else false.
     */
    bool remove( ITEM& item ) noexcept { return SListBase_::remove( item ); }

    /** Insert the "item" ITEM into the list behind the "after" ITEM element.
        If 'after' is nullptr, then 'item' is added to the head of the list.
     */
    void insertAfter( ITEM& after, ITEM& item ) noexcept { SListBase_::insertAfter( after, item ); }

    /** Insert the "item" ITEM into the list ahead of the "before" ITEM element.
        If 'before' is nullptr, then 'item' is added to the tail of the list.

        Note: This insert operation is more expensive than insertAfter() because
        a traversal of the list is required to find the 'before' item
     */
    void insertBefore( ITEM& before, ITEM& item ) noexcept { SListBase_::insertBefore( before, item ); }

    /// Returns true if the specified item is already in the list, else false.
    bool find( const ITEM& item ) const noexcept { return SListBase_::find( item ); }

    /** Return a pointer to the first item in the list. The returned item
        remains in the list.  Returns nullptr if the list is empty.
     */
    ITEM* first( void ) const noexcept { return (ITEM*)SListBase_::first(); }

    /** Return a pointer to the last item in the list. The returned item remains
        in the list.  Returns nullptr if the list is empty.
     */
    ITEM* last( void ) const noexcept { return (ITEM*)SListBase_::last(); }

    /** Return a pointer to the item after the item "item". Both items remain in
        the list.  Returns nullptr when the end-of-list is reached.
     */
    ITEM* next( const ITEM& item ) const noexcept { return (ITEM*)SListBase_::next( item ); }

protected:
    /// Prevent access to the copy constructor -->Containers can not be copied!
    SList<ITEM>( const SList<ITEM>& m ) = delete;

    /// Prevent access to the assignment operator -->Containers can not be
    /// copied!
    SList<ITEM>& operator=( const SList<ITEM>& m ) = delete;

    /// Prevent access to the move constructor -->Containers can not be moved!
    SList<ITEM>( SList<ITEM>&& m ) = delete;

    /// Prevent access to the move assignment operator -->Containers can not be
    /// moved!
    SList<ITEM>& operator=( SList<ITEM>&& m ) = delete;
};


};  // namespace Container
};  // namespace Kit
#endif  // end header latch