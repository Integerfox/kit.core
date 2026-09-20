#ifndef KIT_CONFIG_H_
#define KIT_CONFIG_H_
/*-----------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file Project/build specific 'Options' (see LConfig Pattern) */


#define USE_KIT_SYSTEM_ASSERT
#define USE_KIT_SYSTEM_TRACE

// newlib-nano does not set __int64_t_defined so PRIX64 is undefined, but
// int64_t/uint64_t ARE available and %llX IS supported by nano printf.
// Provide the format specifier as a literal string instead.
#define OPTION_KIT_DM_MP_MAX_INT_HEX_PRINTF_FORMAT "llX"

#endif
