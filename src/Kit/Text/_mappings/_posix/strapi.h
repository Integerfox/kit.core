#ifndef KIT_TEXT_MAPPINGS_POSIX_STRAPI_H_
#define KIT_TEXT_MAPPINGS_POSIX_STRAPI_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file

    This file provides the mapping of the 'strapi' for POSIX compliant
    platform.

 */




 ///////////////////////////////////////////////////////////////////////////////

#include <stdio.h> // For sprintf

#define HAVE_KIT_TEXT_STRCASECMP
#define HAVE_KIT_TEXT_STRNCASECMP
#define HAVE_KIT_TEXT_STRTOK_R
#define HAVE_KIT_TEXT_SNPRINTF
#define HAVE_KIT_TEXT_VSNPRINTF      

#define strset_MAP(s,f)		strnset_MAP(s,f,strlen(s))

extern char* strupr_MAP( char* s1 );
extern char* strlwr_MAP( char* s1 );
extern char* strnset_MAP( char* s1, int fill, size_t len );
extern char* strrev_MAP( char* s1 );


#endif  // end header latch

