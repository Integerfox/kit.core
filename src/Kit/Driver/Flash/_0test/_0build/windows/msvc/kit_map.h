/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

/** Note: Intentionally there is NO Header latch (see LHeader Pattern) */

// IO mapping.  Note: Needs to be include FIRST because of winsock2.h ordering issues
#include "Kit/Io/_mappings/_win32/mappings.h"

// OSAL mappings
#include "Kit/System/Win32/mappings.h"

// strapi mapping
#include "Kit/Text/_mappings/_msvc/strapi.h"