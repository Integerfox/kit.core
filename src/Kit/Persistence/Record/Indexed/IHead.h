#ifndef KIT_PERSISTENCE_RECORD_INDEXED_IHEAD_H
#define KIT_PERSISTENCE_RECORD_INDEXED_IHEAD_H
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Persistence/Record/IDataRecord.h"
#include "Kit/Persistence/Types.h"

///
namespace Kit {
///
namespace Persistence {
///
namespace Record {
///
namespace Indexed {


/** This abstract class defines the interface to manage the newest
    entry in an IEntry record.  Or said another way, manages the logical
    head pointer to the virtual Ring Buffer of entries contain in a IEntry
    Record
*/
class IHead: public IDataRecord 
{
public:
    /** This method returns the 'offset-index' (starting from the start of
        IMedia storage used for the entries) of the most recently written
        entry.

        Returns true if there is at least one valid 'entry' persistent stored;
        else false is returned;
     */
    virtual bool getLatestOffset( Size_T& offset, uint64_t& indexValue ) const noexcept = 0;


    /** This method updates/sets the 'offset-index' (starting from the start of
        IMedia storage used for the entries) of the most recently written
        entry.
     */
    virtual void setLatestOffset( Size_T offset, uint64_t indexValue ) noexcept = 0;

public:
    /// Virtual destructor
    virtual ~IHead() noexcept = default;
};

}  // end namespaces
}
}
}
#endif  // end header latch
