/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "kit_config.h"
#include "IApi.h"

//------------------------------------------------------------------------------
namespace Kit {
namespace Driver {
namespace NV {

// Define the virtual destructor out-of-line so the vtable is emitted here
// rather than in every translation unit that includes IApi.h
IApi::~IApi() noexcept = default;

}  // end namespaces
}
}
