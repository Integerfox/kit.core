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

///
#define strcasecmp_MAP          _stricmp
///
#define strncasecmp_MAP         _strnicmp
///
#define strupr_MAP              _strupr
///
#define strlwr_MAP              _strlwr
///
#define strtok_r_MAP(s,d,p)     strtok(s,d)     // NOTE: strtok() under windows IS Thread safe (but not "interleave" safe inside a single thread)

#define HAVE_KIT_TEXT_SNPRINTF      
#define HAVE_KIT_TEXT_VSNPRINTF


#endif  // end header latch
