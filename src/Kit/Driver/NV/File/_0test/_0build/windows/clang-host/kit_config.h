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

#define USE_KIT_SYSTEM_TRACE
#define USE_KIT_SYSTEM_ASSERT
#define KitTextToStringMaxUnsigned_T uint64_t

// Small erase buffer so the unit test exercises chunked erasing of a new file
#define OPTION_KIT_DRIVER_NV_FILE_BYTES_PER_PAGE 16

#endif
