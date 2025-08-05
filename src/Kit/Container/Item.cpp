/*-----------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Item.h"
#include "Kit/System/fatalError.h"


//------------------------------------------------------------------------------
namespace Kit {
namespace Container {

bool Item::insert_( void* newContainerPtr ) noexcept
{
    if ( m_inListPtr_ )
    {
        Kit::System::fatalError( "Container Error: Double Insert. <item, prev container, new container>", (size_t)this, (size_t)m_inListPtr_, (size_t)newContainerPtr );
        return false;
    }
    else
    {
        m_inListPtr_ = newContainerPtr;
        return true;
    }
}

bool Item::validateNextOkay_( const void* containerPtr ) const noexcept
{
    if ( m_inListPtr_ != containerPtr )
    {
        Kit::System::fatalError( "Container Error: Invalid next() call. <item, container>", (size_t)this, (size_t)containerPtr );
        return false;
    }
    return true;
}

} // end namespaces
}
//------------------------------------------------------------------------------