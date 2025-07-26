#ifndef CPL_SLIST_BASE_H_
#define CPL_SLIST_BASE_H_
/*-----------------------------------------------------------------------------
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

/** This protected base class implements a NON-TYPE-SAFE a singly linked list
    which maintains the ordering imposed on it by the application.  It is
    intended to be the base class for a type-safe template class. Having
    a non-templatized base class reduces the memory footprint when an
    application has more than one type specific SListBase_ class
 */
class SListBase_
{
protected:
    /** Points to the first item in the list.
     */
    Item* m_headPtr;

    /** Points to the last item in the list.
     */
    Item* m_tailPtr;

protected:
    /// Constructor initializes head and tail pointers.
    SListBase_() noexcept
        : m_headPtr( 0 ), m_tailPtr( 0 )

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
        SListBase_( const char* ignoreThisParameter_usedToCreateAUniqueConstructor ) noexcept
    {
        // intentially DO NOTHING
    }

protected:
    /// Moves the content of the this queue to the specified queue.
    void move( SListBase_& dst ) noexcept
    {
        // clear the destination list
        dst.clearTheList();

        // Copy each item (so the debug info is correct)
        Item* nextPtr;
        while ( ( nextPtr = get() ) )
        {
            dst.put( *nextPtr );
        }
    }

    /// Empties the list.  All references to the item(s) in the list are lost.
    void clearTheList() noexcept
    {
        // Drain list so the debug traps work correctly
        while ( get() )
        {
            ;
        }
    }


protected:
    /// Removes the first item in the list.  Returns nullptr if the list is empty.
    Item* getFirst( void ) noexcept
    {
        Item* nextPtr;
        if ( ( nextPtr = m_headPtr ) )
        {
            if ( !( m_headPtr = (Item*)( nextPtr->m_nextPtr_ ) ) )
            {
                m_tailPtr = nullptr;
            }
        }

        Item::remove_( nextPtr );
        return nextPtr;
    }

    /// Removes the last item in the list.  Returns nullptr if the list is empty.
    Item* getLast( void ) noexcept
    {
        Item* lastPtr = m_tailPtr;
        if ( lastPtr )
        {
            remove( *m_tailPtr );
        }
        return lastPtr;
    }

    /// Adds the item as the last item in the list
    void putFirst( Item& item ) noexcept
    {
        if ( item.insert_( this ) )
        {
            if ( m_headPtr )
            {
                item.m_nextPtr_ = m_headPtr;
                m_headPtr       = &item;
            }
            else
            {
                m_headPtr = m_tailPtr = &item;
                item.m_nextPtr_       = nullptr;
            }
        }
    }

    /// Adds the item as the last item in the list
    void putLast( Item& item ) noexcept
    {
        if ( item.insert_( this ) )
        {
            if ( m_headPtr )
            {
                m_tailPtr->m_nextPtr_ = &item;
            }
            else
            {
                m_headPtr = &item;
            }
            item.m_nextPtr_ = nullptr;
            m_tailPtr       = &item;
        }
    }

protected:
    /** Return a pointer to the first item in the list.
        The returned item remains in the list.  Returns nullptr
        if the list is empty.
     */
    Item* first( void ) const noexcept
    {
        return m_headPtr;
    }

    /** Return a pointer to the last item in the list.
        The returned item remains in the list.  Returns nullptr
        if the list is empty.
     */
    Item* last( void ) const noexcept
    {
        return m_tailPtr;
    }

protected:
    /** Remove specified Item element from the list.
        Returns true if the specified element was found and
        removed from the list, else false.
     */
    bool remove( Item& item ) noexcept
    {
        if ( item.isInContainer_( this ) )
        {
            Item* nxtPtr;
            Item* prvPtr;
            for ( nxtPtr = first(), prvPtr = nullptr; nxtPtr;
                  prvPtr = nxtPtr, nxtPtr = next( *nxtPtr ) )
            {
                if ( nxtPtr == &item )
                {
                    if ( prvPtr )
                    {
                        if ( !( prvPtr->m_nextPtr_ = nxtPtr->m_nextPtr_ ) )
                        {
                            m_tailPtr = prvPtr;
                        }
                    }
                    else
                    {
                        if ( !( m_headPtr = (Item*)nxtPtr->m_nextPtr_ ) )
                        {
                            m_tailPtr = nullptr;
                        }
                    }
                    Item::remove_( &item );
                    return true;
                }
            }
            // Note: I should never get here (i have already checked that I am in list
            // at the top of the method)
        }
        return false;
    }

    /** Insert the "item" Item into the list behind the
        "after" Item element.  If 'after' is nullptr, then 'item'
        is added to the head of the list.
     */
    void insertAfter( Item& after, Item& item ) noexcept
    {
        if ( item.insert_( this ) )
        {
            item.m_nextPtr_ = after.m_nextPtr_;
            if ( !item.m_nextPtr_ )
            {
                m_tailPtr = &item;
            }
            after.m_nextPtr_ = &item;
        }
    }

    /** Insert the "item" Item into the list ahead of the
        "before" Item element. If 'before' is nullptr, then 'item'
        is added to the tail of the list.  Note: This insert
        operation is more expensive than insertAfter() because
        a traversal of the list is required to find the
        'before' item
     */
    void insertBefore( Item& before, Item& item ) noexcept
    {
        if ( item.insert_( this ) )
        {
            Item* nxtPtr;
            Item* prvPtr;
            for ( nxtPtr = first(), prvPtr = nullptr; nxtPtr;
                  prvPtr = nxtPtr, nxtPtr = next( *nxtPtr ) )
            {
                if ( nxtPtr == &before )
                {
                    item.m_nextPtr_ = nxtPtr;
                    if ( prvPtr )
                    {
                        prvPtr->m_nextPtr_ = &item;
                    }
                    else
                    {
                        m_headPtr = &item;
                    }
                    break;
                }
            }
        }
    }

    /** Returns true if the specified item is already in the
        list, else false.
     */
    bool find( const Item& item ) const noexcept
    {
        return item.isInContainer_( this );
    }

    /** Return a pointer to the item after the item "item".
        Both items remain in the list.  Returns nullptr when the
        end-of-list is reached OR if 'item' is not in the list.
     */
    Item* next( const Item& item ) const noexcept
    {
        if ( isInContainer_( item ) )
        {
            return (Item*)( item.m_nextPtr_ );
        }
        return nullptr;
    }

protected:
    /// Prevent access to the copy constructor -->Containers can not be copied!
    SListBase_( const SListBase_& m ) = delete;

    /// Prevent access to the assignment operator -->Containers can not be
    /// copied!
    SListBase_& operator=( const SListBase_& m ) = delete;

    /// Prevent access to the move constructor -->Containers can not be moved!
    SListBase_( SListBase_&& m ) = delete;

    /// Prevent access to the move assignment operator -->Containers can not be
    /// moved!
    SListBase_& operator=( SListBase_&& m ) = delete;
};


}       // end namespaces
}       
#endif  // end header latch