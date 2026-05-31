/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "OrderedListBase.h"

//------------------------------------------------------------------------------
namespace Kit {
namespace Container {


void OrderedListBase::insert( KeyedItem& item ) noexcept
{
    KeyedItem* current = static_cast<KeyedItem*>( first() );
    while ( current )
    {
        if ( current->getKey().compareKey( item.getKey() ) > 0 )
        {
            insertBefore( *current, item );
            return;
        }
        current = static_cast<KeyedItem*>( next( *current ) );
    }

    putLast( item );
}


}  // end namespace
}
//------------------------------------------------------------------------------