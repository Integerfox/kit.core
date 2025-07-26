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
//#include "Kit/System/FatalError.h"


/// 
using namespace Kit::Container;


////////////////////////////////////////////////////////////////////////////////
bool Item::insert_( void* newContainerPtr )
{
    if ( m_inListPtr_ )
    {
  //      FatalError::logf( "Container Error: Double Insert. item=%p, prev container=%p, new container=%p", this, m_inListPtr_, newContainerPtr );
        return false;
    }
    else
    {
        m_inListPtr_ = newContainerPtr;
        return true;
    }
}

bool Item::isInContainer_( const void* containerPtr ) const noexcept
{
    if ( m_inListPtr_ != containerPtr )
    {
        return false;
    }
    return true;
}

void Item::remove_( Item* itemPtr ) noexcept
{
    if ( itemPtr )
    {
        itemPtr->m_inListPtr_ = 0;
    }
}


