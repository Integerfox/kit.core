#ifndef KIT_TEXT_MAPPINGS_MINGW_STRAPI_H_
#define KIT_TEXT_MAPPINGS_MINGW_STRAPI_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */
/** @file

    This file provides the mapping of the 'strapi' to the MinGW C/C++ compiler.

    The MinGW compiler provides all of the strapi natively.

 */




 ///////////////////////////////////////////////////////////////////////////////

#include <string.h>

#define HAVE_KIT_TEXT_STRCASECMP
#define HAVE_KIT_TEXT_STRNCASECMP
#define HAVE_KIT_TEXT_STRUPR
#define HAVE_KIT_TEXT_STRLWR
#define HAVE_KIT_TEXT_STRSET
#define HAVE_KIT_TEXT_STRNSET
#define HAVE_KIT_TEXT_STRREV
#define HAVE_KIT_TEXT_STRTOK_R
#define HAVE_KIT_TEXT_SNPRINTF
#define HAVE_KIT_TEXT_VSNPRINTF      



#endif  // end header latch
