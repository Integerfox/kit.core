#ifndef KIT_CONTAINER_DLIST_BASE_H_
#define KIT_CONTAINER_DLIST_BASE_H_
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

/** This protected base class implements a NON-TYPE-SAFE a doubly linked list
    which maintains the ordering imposed on it by the application.  It is
    intended to be the base class for a type-safe template class. Having
    a non-templatized base class reduces the memory footprint when an
    application has more than one type specific DListBase_ class
 */
class DListBase_
{
protected:
    /// Constructor initializes head and tail pointers.
    DListBase_() noexcept
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
    DListBase_( const char* ignoreThisParameter_usedToCreateAUniqueConstructor ) noexcept
    {
        // intentially DO NOTHING
    }

protected:
    /// Moves the content of the this queue to the specified queue.
    void move( DListBase_& dst ) noexcept
    {
        // clear the destination list
        dst.clearTheList();

        // Copy each item (so the debug info is correct)
        ExtendedItem* nextPtr;
        while ( ( nextPtr = getFirst() ) )
        {
            dst.putLast( *nextPtr );
        }
    }

    /// Empties the list.  All references to the item(s) in the list are lost.
    void clearTheList() noexcept
    {
        // Drain list so the debug traps work correctly
        while ( getFirst() )
        {
            ;
        }
    }


protected:
    /// Removes the first item in the list.  Returns nullptr if the list is empty.
    ExtendedItem* getFirst() noexcept
    {
        ExtendedItem* firstPtr = m_headPtr;
        if ( firstPtr )
        {
            remove( *firstPtr );
        }
        return firstPtr;
    }

    /// Removes the last item in the list.  Returns nullptr if the list is empty.
    ExtendedItem* getLast() noexcept
    {
        ExtendedItem* lastPtr = m_tailPtr;
        if ( lastPtr )
        {
            remove( *lastPtr );
        }
        return lastPtr;
    }

    /// Adds the 'item' as the last item in the list
    void putFirst( ExtendedItem& item ) noexcept
    {
        if ( item.insert_( this ) )
        {
            if ( m_headPtr )
            {
                item.m_nextPtr_       = m_headPtr;
                m_headPtr->m_prevPtr_ = &item;
                m_headPtr             = &item;
            }
            else
            {
                m_headPtr = m_tailPtr = &item;
                item.m_nextPtr_       = 0;
            }
            item.m_prevPtr_ = 0;
        }
    }

    /// Adds the 'item' as the last item in the list
    void putLast( ExtendedItem& item ) noexcept
    {
        if ( item.insert_( this ) )
        {
            if ( m_headPtr )
            {
                m_tailPtr->m_nextPtr_ = &item;
                item.m_prevPtr_       = m_tailPtr;
            }
            else
            {
                m_headPtr       = &item;
                item.m_prevPtr_ = 0;
            }
            item.m_nextPtr_ = 0;
            m_tailPtr       = &item;
        }
    }

protected:
    /** Return a pointer to the first item in the list. The returned item 
        remains in the list.  Returns nullptr if the list is empty.
     */
    ExtendedItem* first() const noexcept
    {
        return m_headPtr;
    }

    /** Return a pointer to the last item in the list. The returned item
        remains in the list.  Returns nullptr if the list is empty.
     */
    ExtendedItem* last() const noexcept
    {
        return m_tailPtr;
    }

protected:
    /** Remove the specified 'item' element from the list. Returns true if the 
        specified element was found and removed from the list, else false.
     */
    bool remove( ExtendedItem& item ) noexcept
    {
        if ( item.isInContainer_( this ) )
        {
            ExtendedItem* prvPtr = item.m_prevPtr_;
            ExtendedItem* nxtPtr = (ExtendedItem*)item.m_nextPtr_;
            if ( prvPtr )
            {
                if ( !( prvPtr->m_nextPtr_ = nxtPtr ) )
                {
                    m_tailPtr = prvPtr;  // Case: Remove tail object
                }
                else
                {
                    nxtPtr->m_prevPtr_ = prvPtr;  // Case: Remove intermediate object
                }
            }
            else
            {
                if ( !( m_headPtr = nxtPtr ) )
                {
                    m_tailPtr = 0;  // Case: Remove last object
                }
                else
                {
                    nxtPtr->m_prevPtr_ = 0;  // Case: Remove Head object
                }
            }

            Item::remove_( &item );
            return true;
        }

        return false;
    }

    /** Insert the "item" into the list behind the "after" element.  If 'after'
        is nullptr, then 'item' is added to the head of the list.
     */
    void insertAfter( ExtendedItem& after, ExtendedItem& item ) noexcept
    {
        if ( item.insert_( this ) )
        {
            ExtendedItem* nxtPtr = (ExtendedItem*)( item.m_nextPtr_ = after.m_nextPtr_ );
            item.m_prevPtr_      = &after;
            after.m_nextPtr_     = &item;
            if ( !nxtPtr )
            {
                m_tailPtr = &item;
            }
            else
            {
                nxtPtr->m_prevPtr_ = &item;
            }
        }
    }

    /** Insert the "item" into the list ahead of the "before" element. If
        'before' is nullptr, then 'item' is added to the tail of the list.
     */
    void insertBefore( ExtendedItem& before, ExtendedItem& item ) noexcept
    {
        if ( item.insert_( this ) )
        {
            ExtendedItem* prvPtr = (ExtendedItem*)( item.m_prevPtr_ = before.m_prevPtr_ );
            item.m_nextPtr_      = &before;
            before.m_prevPtr_    = &item;
            if ( !prvPtr )
            {
                m_headPtr = &item;
            }
            else
            {
                prvPtr->m_nextPtr_ = &item;
            }
        }
    }

    /// Returns true if the specified item is already in the list, else false.
    bool find( const ExtendedItem& item ) const noexcept
    {
        return item.isInContainer_( this );
    }

    /** Return a pointer to the item after the item "item". Both items remain
        in the list.  Returns nullptr when the end-of-list is reached.

        NOTE: If 'item' is not in the list, then a fatal error is generated.
     */
    ExtendedItem* next( const ExtendedItem& item ) const noexcept
    {
        if ( item.validateNextOkay_( this ) )
        {
            return (ExtendedItem*)item.m_nextPtr_;
        }
        return nullptr;
    }

    /** Return a pointer to the item before the item "current". Both items 
        remain in the list.  Returns nullptr when the front-of-list is reached.
     */
    ExtendedItem* previous( const ExtendedItem& current ) const noexcept
    {
        return current.m_prevPtr_;
    }

protected:
    /// Points to the first item in the list.
    ExtendedItem* m_headPtr;

    /// Points to the last item in the list.
    ExtendedItem* m_tailPtr;

protected:
    /// Prevent access to the copy constructor -->Containers can not be copied!
    DListBase_( const DListBase_& m ) = delete;

    /// Prevent access to the assignment operator -->Containers can not be
    /// copied!
    DListBase_& operator=( const DListBase_& m ) = delete;

    /// Prevent access to the move constructor -->Containers can not be implicitly moved!
    DListBase_( DListBase_&& m ) = delete;

    /// Prevent access to the move assignment operator -->Containers can not be implicitly moved!
    DListBase_& operator=( DListBase_&& m ) = delete;
};


}  // end namespaces
}
#endif  // end header latch