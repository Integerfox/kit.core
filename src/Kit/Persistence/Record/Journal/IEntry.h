#ifndef KIT_PERSISTENCE_RECORD_JOURNAL_IENTRY_H
#define KIT_PERSISTENCE_RECORD_JOURNAL_IENTRY_H
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

///
namespace Kit {
///
namespace Persistence {
///
namespace Record {
///
namespace Journal {

/** This abstract class defines interface for managing 'entries' from a
    a collection of entries (i.e. read an entry from a Journal EntryRecord).

    See the README.md file for more details about "Journal Records".

    NOTE: This interface/class is NOT THREAD SAFE and should only be 'used' from
          the Record Server's thread.
*/
class IEntry : public Kit::Persistence::Record::IDataRecord
{
public:
    /** This structure define an 'marker' that identifies an entry's location
        in persistent media.
     */
    struct Marker_T
    {
        uint64_t timestamp;    //!< The timestamp value for the entry (Note: this is free-running counter, i.e. is NOT elapsed based)
        Size_T   mediaOffset;  //!< Offset, within the IMedia storage to the start of the Entry

        /// Constructor 
        Marker_T()
            : timestamp( 0 )
            , mediaOffset( 0 )
        {
        }
    };

public:
    /** This method reads/retrieves the latest entry (from the list of Journal
        Entries) stored in the persistent media.  The method is synchronous in
        that the method does not return until the entry has been 'read' from
        the persistent media.

        Returns true if the latest entry is valid and has been read into 'dst';
        else false is returned.

        NOTE: When getLatest() returns false, this means there are NO entries
              stored.

        NOTE: 'dst' is ALWAYS updated EVEN if no entry was 'found', basically 'dst'
              is used as a work buffer when traversing the list.
     */
    virtual bool getLatest( IPayload& dst, Marker_T& entryMarker ) noexcept = 0;

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
    virtual bool getNext( uint64_t       newerThanTimestamp,
                          const Marker_T beginHereMarker,
                          IPayload&      dst,
                          Marker_T&      entryMarker ) noexcept = 0;

    /** This method is similar to getNext(), except that it returns the next
        oldest entry.
     */
    virtual bool getPrevious( uint64_t       olderThanTimestamp,
                              const Marker_T beginHereMarker,
                              IPayload&      dst,
                              Marker_T&      entryMarker ) noexcept = 0;

public:
    /** This method can be used to read an entry by its 'entry index'.  The
        entry index is a zero based index.

        Returns true if the entry at 'entryIndex' is a valid entry; else false
        is returned. Note: if 'entryIndex' is out of range, false is returned.

        NOTE: 'dst' is ALWAYS updated EVEN if no valid entry was 'found',
              basically 'dst' is used as a work buffer when traversing the list.
     */
    virtual bool getByEntryIndex( Size_T    entryIndex,
                                  IPayload& dst,
                                  Marker_T& entryMarker ) noexcept = 0;


    /** This method returns the maximum allowed 'entryIndex' when calling
        getByEntryIndex().
     */
    virtual Size_T getMaxIndex() const noexcept = 0;

public:
    /** This method appends an entry to the list of Journal Entries.  The method
        is synchronous in that the method does not return until the entry has
        been 'written' the persistent media.

        Returns true on success; else if an error occurred (e.g. IO error while
        writing) false is returned.
     */
    virtual bool addEntry( const IPayload& src ) noexcept = 0;

public:
    /** This method resets the head pointer and the timestamp to zero.  It is
         essentially a logical erase of the entries.  The method returns true if
         the reset was successful; else false is returned when an error occurred.

         NOTE: With respect to 'make room' for more entries, there is no actual
               need to either logical or physical the entries, since the the
               design always guarantees that the latest N entries are persistently
               stored.
     */
    virtual bool resetHead() noexcept = 0;
};


}  // end namespaces
}
}
}
#endif  // end header latch
