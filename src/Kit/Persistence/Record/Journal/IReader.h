#ifndef KIT_PERSISTENCE_RECORD_JOURNAL_IREADER_H
#define KIT_PERSISTENCE_RECORD_JOURNAL_IREADER_H
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

/** This abstract class defines interface for reading 'entries' from a
    a Journal Entry Record.

    See the README.md file for more details about "Journal Records".

    NOTE: This interface/class IS THREAD SAFE and can be called from any thread,
          EXCEPT for the thread that the Journal::Server executes in.
*/
class IReader
{
public:
    /** This method reads/retrieves the latest entry (from the list of Journal
        Entries) stored in the persistent media.  The method is synchronous in
        that the method does not return until the entry has been 'read' from
        the persistent media.

        Returns true if the latest entry is valid and has been read into 'dst';
        else false is returned.

        NOTE: When retrieveLatest() returns false, this means there are NO entries
              stored.

        NOTE: 'dst' is ALWAYS updated EVEN if no entry was 'found', basically 'dst'
              is used as a work buffer when traversing the list.
     */
    virtual bool retrieveLatest( IPayload& dst, IEntry::Marker_T& entryMarker ) noexcept = 0;

public:
    /** This method walks the entire 'list of entries' and returns the next
        newer entry as specified by the 'newerThan' value.  The traversal
        starts with the 'beginHereMarker' The method is synchronous in that the
        method does not return until the entry has been 'read' from the persistent
        media.

        Returns true if the 'next' entry was found and has been read into 'dst';
        else false is returned.

        NOTE: 'dst' is ALWAYS updated EVEN if no entry was 'found', basically 'dst'
              is used as a work buffer when traversing the list.

     */
    virtual bool retrieveNext( uint64_t               newerThanTimestamp,
                               const IEntry::Marker_T beginHereMarker,
                               IPayload&              dst,
                               IEntry::Marker_T&      entryMarker ) noexcept = 0;

    /** This method is similar to retrieveNext(), except that it returns the next
        oldest entry.
     */
    virtual bool retrievePrevious( uint64_t               olderThanTimestamp,
                                   const IEntry::Marker_T beginHereMarker,
                                   IPayload&              dst,
                                   IEntry::Marker_T&      entryMarker ) noexcept = 0;

public:
    /** This method can be used to read an entry by its 'entry index'.  The
        entry index is a zero based index.

        Returns true if the entry at 'entryIndex' is a valid entry; else false
        is returned. Note: if 'entryIndex' is out of range, false is returned.

        NOTE: 'dst' is ALWAYS updated EVEN if no valid entry was 'found',
              basically 'dst' is used as a work buffer when traversing the list.
     */
    virtual bool retrieveByEntryIndex( Size_T            entryIndex,
                                       IPayload&         dst,
                                       IEntry::Marker_T& entryMarker ) noexcept = 0;


    /** This method returns the maximum allowed 'entryIndex' when calling
        retrieveByEntryIndex().
     */
    virtual Size_T maxIndex() const noexcept = 0;
};


}  // end namespaces
}
}
}
#endif  // end header latch
