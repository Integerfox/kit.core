/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Item.h"
#include "Kit/System/FatalError.h"
#include "Kit/System/Shutdown.h"


//------------------------------------------------------------------------------
namespace Kit {
namespace Container {

Item::Item() noexcept
    : m_inListPtr_( nullptr )
{
}

bool Item::isInContainer_( const void* containerPtr ) const noexcept
{
    if ( m_inListPtr_ != containerPtr )
    {
        return false;
    }
    return true;
}

bool Item::insert_( void* newContainerPtr ) noexcept
{
    if ( m_inListPtr_ )
    {
        Kit::System::FatalError::logf( Kit::System::Shutdown::eCONTAINER,
                                       "Container Error: Double Insert. item=%p, prev container=%p, new container=%p",
                                       this,
                                       m_inListPtr_,
                                       newContainerPtr );
        return false;
    }
    else
    {
        m_inListPtr_ = newContainerPtr;
        return true;
    }
}

void Item::remove_( Item* itemPtr ) noexcept
{
    if ( itemPtr )
    {
        itemPtr->m_inListPtr_ = nullptr;
    }
}
bool Item::validateNextOkay_( const void* containerPtr ) const noexcept
{
    if ( m_inListPtr_ != containerPtr )
    {
        Kit::System::FatalError::logf( Kit::System::Shutdown::eCONTAINER,
                                       "Container Error: Invalid next() call. item=%p, container=%p",
                                       this,
                                       containerPtr );
        return false;
    }
    return true;
}

}  // end namespaces
}
//------------------------------------------------------------------------------