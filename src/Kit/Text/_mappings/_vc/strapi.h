#ifndef KIT_TEXT_MAPPPINGS_VC_STRAPI_H_
#define KIT_TEXT_MAPPPINGS_VC_STRAPI_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file

    This file provides the mapping of the 'strapi' to Microsoft's VC C/C++ compiler.

    The VC compiler provides all of the strapi - just with the 'wrong names'

 */




 ///////////////////////////////////////////////////////////////////////////////

#include <string.h>
#include <stdio.h>

///
#define strcasecmp_MAP          _stricmp
///
#define strncasecmp_MAP         _strnicmp
///
#define strupr_MAP              _strupr
///
#define strlwr_MAP              _strlwr
///
#define strset_MAP              _strset
///
#define strnset_MAP             _strnset
///
#define strrev_MAP              _strrev
///
#define strtok_r_MAP(s,d,p)     strtok(s,d)     // NOTE: strtok() under windows IS Thread safe (but not "interleave" safe inside a single thread)
///
#define snprintf_MAP(b,c,f,...) _snprintf_s(b,c,c,f,##__VA_ARGS__) 
///
#define vsnprintf_MAP           _vsnprintf




#endif  // end header latch
