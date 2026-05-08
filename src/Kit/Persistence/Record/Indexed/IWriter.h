#ifndef KIT_PERSISTENCE_RECORD_INDEXED_IWRITER_H
#define KIT_PERSISTENCE_RECORD_INDEXED_IWRITER_H
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Persistence/Record/Indexed/IEntry.h"

///
namespace Kit {
///
namespace Persistence {
///
namespace Record {
///
namespace Indexed {

/** This abstract class defines interface for writing 'entries' to a
    an Indexed Entry Record.

    See the README.md file for more details about "Indexed Records".

    NOTE: This interface/class IS THREAD SAFE and can be called from any thread,
          EXCEPT for the thread that the Indexed::Server executes in.
*/
class IWriter
{
public:
    /** This method appends an entry to the list of Indexed Entries.  The method
        is synchronous in that the method does not return until the entry has
        been 'written' the persistent media.

        Returns true on success; else if an error occurred (e.g. IO error while
        writing) false is returned.
     */
    virtual bool append( const IPayload& src ) noexcept = 0;

public:
    /** This method resets the head pointer and the timestamp to zero.  It is
        essentially a logical erase of the entires.  

        NOTE: With respect to 'making room' for more entries, there is no actual
              need to either logically or physically erase the entries, since the
              design always guarantees that the latest N entries are persistently
              stored.
     */
    virtual void logicalReset() noexcept = 0;
};


}  // end namespaces
}
}
}
#endif  // end header latch
