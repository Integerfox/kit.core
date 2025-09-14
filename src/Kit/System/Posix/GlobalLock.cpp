/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/System/GlobalLock.h"
#include "Kit/System/Mutex.h"

namespace Kit {
namespace System {

static Mutex global_;

//////////////////////////////////////////////////////////////////////////////
void GlobalLock::begin()
{
    global_.lock();
}


void GlobalLock::end()
{
    global_.unlock();
}

}  // end namespace
}
//------------------------------------------------------------------------------