/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "MapItem.h"

//------------------------------------------------------------------------------
namespace Kit {
namespace Container {

MapItem::MapItem() noexcept
{
    initialize( nullptr );
}

void MapItem::initialize( MapItem* parent ) noexcept
{
    m_parentPtr_   = parent;
    m_is_NOT_root_ = true;
    m_balance_     = eEVEN_;
    m_nextPtr_     = nullptr;
    m_prevPtr_     = nullptr;
}

}  // end namespace
}
//------------------------------------------------------------------------------