#ifndef KIT_PERSISTENCE_RECORD_JOURNAL_IWRITER_H
#define KIT_PERSISTENCE_RECORD_JOURNAL_IWRITER_H
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Persistence/Record/Journal/IEntry.h"

///
namespace Kit {
///
namespace Persistence {
///
namespace Record {
///
namespace Journal {

/** This abstract class defines interface for 'resetting' entries in a
    a Journal Entry Record.

    See the README.md file for more details about "Journal Records".

    NOTE: This interface/class IS THREAD SAFE and can be called from any thread,
          EXCEPT for the thread that the Journal::Server executes in.
*/
class IReset
{

public:
    /** This method resets the head pointer and the timestamp to zero.  It is
        essentially a logical erase of the entires.

         Returns true if the reset was successful; else false is returned when an
         error occurred during the reset.

        NOTE: With respect to 'making room' for more entries, there is no actual
              need to either logically or physically erase the entries, since the
              design always guarantees that the latest N entries are persistently
              stored.
     */
    virtual bool logicalReset() noexcept = 0;

public:
    /// Virtual destructor
    virtual ~IReset() noexcept = default;
};


}  // end namespaces
}
}
}
#endif  // end header latch
