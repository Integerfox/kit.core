/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "DListBase.h"

//------------------------------------------------------------------------------
namespace Kit {
namespace Container {

DListBase::DListBase() noexcept
    : m_headPtr( nullptr ), m_tailPtr( nullptr )
{
}


void DListBase::move( DListBase& dst ) noexcept
{
    // clear the destination list
    dst.clearTheList();

    // Copy each item (so the debug info is correct)
    ExtendedListItem* nextPtr;
    while ( ( nextPtr = getFirst() ) )
    {
        dst.putLast( *nextPtr );
    }
}

void DListBase::clearTheList() noexcept
{
    // Drain list so the debug traps work correctly
    while ( getFirst() )
    {
        ;
    }
}


ExtendedListItem* DListBase::getFirst() noexcept
{
    ExtendedListItem* firstPtr = m_headPtr;
    if ( firstPtr )
    {
        remove( *firstPtr );
    }
    return firstPtr;
}

ExtendedListItem* DListBase::getLast() noexcept
{
    ExtendedListItem* lastPtr = m_tailPtr;
    if ( lastPtr )
    {
        remove( *lastPtr );
    }
    return lastPtr;
}

void DListBase::putFirst( ExtendedListItem& item ) noexcept
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

void DListBase::putLast( ExtendedListItem& item ) noexcept
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

bool DListBase::remove( ExtendedListItem& item ) noexcept
{
    if ( item.isInContainer_( this ) )
    {
        ExtendedListItem* prvPtr = item.m_prevPtr_;
        ExtendedListItem* nxtPtr = static_cast<ExtendedListItem*>( item.m_nextPtr_ );
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

void DListBase::insertAfter( ExtendedListItem& after, ExtendedListItem& item ) noexcept
{
    if ( item.insert_( this ) )
    {
        ExtendedListItem* nxtPtr = static_cast<ExtendedListItem*>( item.m_nextPtr_ = after.m_nextPtr_ );
        item.m_prevPtr_          = &after;
        after.m_nextPtr_         = &item;
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

void DListBase::insertBefore( ExtendedListItem& before, ExtendedListItem& item ) noexcept
{
    if ( item.insert_( this ) )
    {
        ExtendedListItem* prvPtr = static_cast<ExtendedListItem*>( item.m_prevPtr_ = before.m_prevPtr_ );
        item.m_nextPtr_          = &before;
        before.m_prevPtr_        = &item;
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


ExtendedListItem* DListBase::next( const ExtendedListItem& item ) const noexcept
{
    if ( item.validateNextOkay_( this ) )
    {
        return static_cast<ExtendedListItem*>( item.m_nextPtr_ );
    }
    return nullptr;
}


}  // end namespace
}
//------------------------------------------------------------------------------