#ifndef KIT_LOGGING_FRAMEWORK_JOURNALLOGSINK_H_
#define KIT_LOGGING_FRAMEWORK_JOURNALLOGSINK_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Persistence/Record/Journal/Server.h"
#include "Kit/Logging/Framework/EntryData.h"
#include "Kit/Time/BootTime.h"
#include <atomic>

///
namespace Kit {
///
namespace Logging {
///
namespace Framework {

/** This concrete class is a 'log sink' that writes the application's log
    entries to persistent storage using a Journal Record.  It extends the
    Kit::Persistence::Record::Journal::Server with:

    - Pausing updates.  When the optional 'pauseUpdates' flag is
      non-null and set to true, no entries are written to persistent storage.
      The entries are NOT removed from the log buffer while paused, i.e. no
      entries are lost (unless the log buffer overflows, see the Logging
      framework's overflow handling).  This allows the application to
      temporarily reduce/halt write traffic to the storage media.

      The flag is a std::atomic<bool>, i.e. it can be set and cleared from any
      thread.  The sink only reads the flag.

      NOTE: Pausing is timely, NOT immediate.  The flag is checked before each
            entry is removed from the log buffer, i.e. a write that is already
            in progress (and the entry being written) completes after the
            flag is set.  There is no acknowledgement that the sink has
            paused

      NOTE: Clearing the flag does not, by itself, trigger writing the queued
            entries.  They are written on the next change notification of the
            log buffer, i.e. when the next entry is logged.  It is recommended
            that the application logs an entry when it resumes updates.

    - Boot-counter correction.  The boot counter (the upper bits of a log
      entry's timestamp, see Kit::Time::getBootTime()) is zero until the
      boot-counter record has been loaded from persistent storage.  Entries
      logged before then wait in the RAM log buffer, and would be written with
      boot counter zero: readers of the journal would see them as entries
      from a different boot.  Also, when the boot-counter record imports the
      stored (previous boot's) counter and then increments it, an entry logged
      between the two steps carries the previous boot's counter.

      The RAM log buffer only holds entries from the current boot, so this
      class captures the current boot counter when the sink is opened, and
      writes every entry whose boot counter does not match with the current
      boot counter.  The elapsed time is kept.

      IMPORTANT: The application MUST open the sink after the boot-counter
                 record has been loaded (and incremented).

    NOTE: When the RingBufferMP 'incomingEntriesBuffer' is created it MUST be
          created with the 'initializeMemory' constructor flag set to false,
          since EntryData_T has a vtable pointer.
 */
class JournalLogSink : public Kit::Persistence::Record::Journal::Server<EntryData_T>
{
public:
    /** Constructor.  See Kit::Persistence::Record::Journal::Server.

        When 'pauseUpdates' is not null, and the pointed-to value is
        true, the log entries are held in the log buffer instead of being
        written to persistent storage.
     */
    JournalLogSink( Kit::EventQueue::IQueue&                   myEventQueue,
                    Kit::Persistence::Record::Journal::IEntry& entryRecord,
                    Kit::Container::RingBufferMP<EntryData_T>& incomingEntriesBuffer,
                    const std::atomic<bool>*                   pauseUpdates = nullptr ) noexcept;

public:
    /// Make all of the base class's request() methods visible (the method below hides them otherwise)
    using Kit::Persistence::Record::Journal::Server<EntryData_T>::request;

    /// Captures the current boot counter, then opens the sink (See Kit::Itc::OpenCloseSync)
    void request( OpenMsg& msg ) noexcept override;

protected:
    /// See Kit::Persistence::Record::Journal::Server
    bool hookRemovingEntry( EntryData_T& entry ) noexcept override;

    /// See Kit::Persistence::Record::Journal::Server
    bool hookAddingEntry( EntryData_T& entry ) noexcept override;

protected:
    /// When not null, and the pointed-to value is true, log entries are held in the log buffer
    const std::atomic<bool>* m_pauseUpdates;

    /// The current boot counter for this boot cycle (captured when the sink is opened)
    KitTimeBootCount_T m_currentCycleBootCounter;
};


}  // end namespaces
}
}
#endif  // end header latch
