/** Note: Intentionally there is NO Header latch (see LHeader Pattern) */

// IO mapping.  Note: Needs to be include FIRST because of winsock2.h ordering issues
#include "Kit/Io/_mappings/_win32/mappings.h"

// OSAL mappings
#include "Kit/System/Win32/mappings.h"

// strapi mapping
#include "Kit/Text/_mappings/_msvc/strapi.h"