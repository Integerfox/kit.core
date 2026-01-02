#ifndef KIT_TEXT_MAPPINGS_ARM_GCC_RP2XXX_STRAPI_H_
#define KIT_TEXT_MAPPINGS_ARM_GCC_RP2XXX_STRAPI_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file

    This file provides the mapping of the 'strapi' to the GCC ARM-M4 NOABI
    C/C++ compiler with the Raspberry PI PICO micro-controllers

 */



 ///////////////////////////////////////////////////////////////////////////////

#include <string.h>


#define HAVE_KIT_TEXT_STRSET
#define HAVE_KIT_TEXT_STRNSET
#define HAVE_KIT_TEXT_STRREV
#define HAVE_KIT_TEXT_STRTOK_R
#define HAVE_KIT_TEXT_SNPRINTF
#define HAVE_KIT_TEXT_VSNPRINTF

extern int strcasecmp_MAP( const char* s1, const char* s2 );
extern int strncasecmp_MAP( const char* s1, const char* s2, size_t n );
extern char* strupr_MAP( char* s1 );
extern char* strlwr_MAP( char* s1 );


#endif  // end header latch
