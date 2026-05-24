#ifndef KIT_PERSISTENCE_RECORD_JOURNAL_IHEAD_H
#define KIT_PERSISTENCE_RECORD_JOURNAL_IHEAD_H
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
namespace Journal {


/** This abstract class defines the interface to manage the newest
    entry in an IEntry record.  Or said another way, manages the logical
    head pointer to the virtual Ring Buffer of entries contain in a IEntry
    Record
*/
class IHead : public IDataRecord
{
public:
    /// Head record state
    struct State_T
    {
        uint64_t latestTimestamp;  //!< Timestamp of the latest entry
        Size_T   latestOffset;     //!< Media offset of the latest entry
        bool     validLatest;      //!< Indicates if the latest entry is valid (i.e. has been written at least once)
    };

public:
    /** This method returns the 'offset-index' (starting from the start of
        IMedia storage used for the entries) of the most recently written
        entry.

        Returns true if there is at least one valid 'entry' persistent stored;
        else false is returned;
     */
    virtual bool getLatestOffset( Size_T& offset, uint64_t& timestamp ) const noexcept = 0;


    /** This method updates/sets the 'offset-index' (starting from the start of
        IMedia storage used for the entries) of the most recently written
        entry.
     */
    virtual void setLatestOffset( Size_T offset, uint64_t timestamp ) noexcept = 0;

    /// This method is to get the current state of the Head record
    virtual State_T getCurrentState() const noexcept = 0;

    /// This method is used to restore/set the state of the Head record, e.g. used when recoverying from errors
    virtual void restoreState( const State_T& state ) noexcept = 0;

public:
    /// Virtual destructor
    virtual ~IHead() noexcept = default;
};

}  // end namespaces
}
}
}
#endif  // end header latch
