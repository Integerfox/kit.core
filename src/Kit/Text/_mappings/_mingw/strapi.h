#ifndef Cpl_Text_x_mappings_mingw_strapi_h_
#define Cpl_Text_x_mappings_mingw_strapi_h_
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

#define HAVE_CPL_TEXT_STRCASECMP
#define HAVE_CPL_TEXT_STRNCASECMP
#define HAVE_CPL_TEXT_STRUPR
#define HAVE_CPL_TEXT_STRLWR
#define HAVE_CPL_TEXT_STRSET
#define HAVE_CPL_TEXT_STRNSET
#define HAVE_CPL_TEXT_STRREV
#define HAVE_CPL_TEXT_STRTOK_R
#define HAVE_CPL_TEXT_SNPRINTF
#define HAVE_CPL_TEXT_VSNPRINTF      



#endif  // end header latch
