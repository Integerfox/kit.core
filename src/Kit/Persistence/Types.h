#ifndef KIT_PERSISTENCE_TYPES_H_
#define KIT_PERSISTENCE_TYPES_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "kit_config.h"
#include <stdint.h>


///
namespace Kit {
///
namespace Persistence {

/*----------------------------------------------------------------------------*/
/** This is the type for all size, lengths, offsets, etc. for the Persistence
    namespace. NOTE: Is important that this type be a 'uintXX_t' type (must be
    an unsigned integer type).  This is because other types like - size_t, 
    unsigned, etc. - have different number of bits depending on the MCU/OS
    platform, Having target dependent sizes (e.g. think HW vs simulator) causes
    problems when the type (i.e. sizeof(Size_T)) is used for the length of fields
    that get persistently stored.
*/
#ifndef OPTION_KIT_PERSISTENCE_SIZE_TYPE
using Size_T = uint32_t;
#else
using Size_T = OPTION_KIT_PERSISTENCE_SIZE_TYPE;
#endif

/// Maximum value for KitPersistenceSize_T.  This is useful for error return values, etc.
constexpr Size_T KIT_PERSISTENCE_SIZE_MAX = static_cast<Size_T>(-1);

/** Endianess for meta data that is persistently stored.  The default is Little
    Endian.  To change to Big endian set the macro to 0.

    NOTE: This ONLY applies to the meta-data - NOT the application data payload.
          The application is responsible for managing the Endianess of its data.
 */
#ifndef OPTION_KIT_PERSISTENCE_MEDIA_LITTLE_ENDIAN
#define OPTION_KIT_PERSISTENCE_MEDIA_LITTLE_ENDIAN  1
#endif

/** Kit::Memory::CursorXX class type for storage media using Little Endian byte
    ordering for multi-byte meta data fields
 */
#if OPTION_KIT_PERSISTENCE_MEDIA_LITTLE_ENDIAN > 0
#include "Kit/Memory/CursorLE.h"
#define KIT_PERSISTENCE_MEDIA_CURSOR Kit::Memory::CursorLE
#else
#include "Kit/Memory/CursorBE.h"
#define KIT_PERSISTENCE_MEDIA_CURSOR Kit::Memory::CursorBE
#endif

}  // end namespaces
}
#endif  // end header latch
