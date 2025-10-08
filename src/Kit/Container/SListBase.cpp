/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "SListBase.h"

//------------------------------------------------------------------------------
namespace Kit {
namespace Container {

void SListBase::move( SListBase& dst ) noexcept
{
    // clear the destination list
    dst.clearTheList();

    // Copy each item (so the debug info is correct)
    ListItem* nextPtr;
    while ( ( nextPtr = getFirst() ) )
    {
        dst.putLast( *nextPtr );
    }
}

void SListBase::clearTheList() noexcept
{
    // Drain list so the debug traps work correctly
    while ( getFirst() )
    {
        ;
    }
}


ListItem* SListBase::getFirst() noexcept
{
    ListItem* nextPtr;
    if ( ( nextPtr = m_headPtr ) )
    {
        if ( !( m_headPtr = nextPtr->m_nextPtr_ ) )
        {
            m_tailPtr = nullptr;
        }
    }

    ListItem::remove_( nextPtr );
    return nextPtr;
}

ListItem* SListBase::getLast() noexcept
{
    ListItem* lastPtr = m_tailPtr;
    if ( lastPtr )
    {
        remove( *m_tailPtr );
    }
    return lastPtr;
}

void SListBase::putFirst( ListItem& item ) noexcept
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

void SListBase::putLast( ListItem& item ) noexcept
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

bool SListBase::remove( ListItem& item ) noexcept
{
    if ( item.isInContainer_( this ) )
    {
        ListItem* nxtPtr;
        ListItem* prvPtr;
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
                    if ( !( m_headPtr = nxtPtr->m_nextPtr_ ) )
                    {
                        m_tailPtr = nullptr;
                    }
                }
                ListItem::remove_( &item );
                return true;
            }
        }
        // Note: I should never get here (i have already checked that I am in list
        // at the top of the method)
    }
    return false;
}

void SListBase::insertAfter( ListItem& after, ListItem& item ) noexcept
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

void SListBase::insertBefore( ListItem& before, ListItem& item ) noexcept
{
    if ( item.insert_( this ) )
    {
        ListItem* nxtPtr;
        ListItem* prvPtr;
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

ListItem* SListBase::next( const ListItem& item ) const noexcept
{
    if ( item.validateNextOkay_( this ) )
    {
        return item.m_nextPtr_;
    }
    return nullptr;
}

} // end namespace
}
//------------------------------------------------------------------------------