#ifndef KIT_PERSISTENCE_RECORD_PRIVATE_H
#define KIT_PERSISTENCE_RECORD_PRIVATE_H
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

/** Size, in bytes, of the 'work buffer' that is default work buffer for concrete
    chunk classes.

    The ENTIRE chunk (the chunk's meta data plus the Record's data) is assembled
    in the work buffer, i.e. the buffer MUST be at least as large as the largest
    Record's payload plus its Chunk's meta data.  Use IChunk::getMetadataLength()
    for the meta data size (e.g. Crc: data length field + EDC; Mirrored:
    transaction ID + data length field + EDC). A Record that does not fit will
    fail to be written AND fail to be loaded (i.e. it will be reset to defaults).
 */
#ifndef OPTION_KIT_PERSISTENCE_RECORD_CHUNK_WORK_BUFFER_SIZE
#define OPTION_KIT_PERSISTENCE_RECORD_CHUNK_WORK_BUFFER_SIZE     1024
#endif
///
namespace Kit {
///
namespace Persistence {
///
namespace Record {
///
namespace Chunk {

/** This PACKAGE SCOPED buffer is a singleton that is available as 'work buffer'
    for Chunk and Record instance to use WHEN executing in the RecordServer's
    thread.

    NOTE: The buffer is NOT thread safe.  It assume only a single RecordServer 
          thread is executing at any given time. If the application has multiple
          RecordServers (aka multiple threads for persistent storage) - then
          this buffer SHOULD NOT BE USED and the application is responsible for
          providing the Chunk instances thread safe buffers.
*/
extern uint8_t g_workBuffer_[OPTION_KIT_PERSISTENCE_RECORD_CHUNK_WORK_BUFFER_SIZE];

}  // end namespaces
}
}
}
#endif  // end header latch
