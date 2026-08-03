#ifndef KIT_LOGGING_FRAMEWORK_LOGSINK_JOURNAL_H
#define KIT_LOGGING_FRAMEWORK_LOGSINK_JOURNAL_H
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Logging/Framework/LogSink.h"
#include "Kit/Persistence/Record/Journal/IEntry.h"


///
namespace Kit {
///
namespace Logging {
///
namespace Framework {

/** This concrete class implements a LogSink that writes log entries to a KIT persistent
    storage journal file
*/
class JournalLogSink : public LogSink
{
public:
    JournalLogSink( Kit::EventQueue::IQueue&                                            myEventQueue,
                    Kit::Container::RingBufferMP<Kit::Logging::Framework::EntryData_T>& incomingEntriesBuffer,
                    Kit::Persistence::Record::Journal::IEntry&                          entryRecord ) noexcept
        : Kit::Logging::Framework::LogSink( myEventQueue, incomingEntriesBuffer )
        , m_entryRecord( entryRecord )
    {
    }

protected:
    void dispatchLogEntry( Kit::Logging::Framework::EntryData_T& src ) noexcept override
    {
        m_entryRecord.addEntry( src );
    }

protected:
    /// Reference to the Journal's EntryRecord API that will be used to store log entries
    Kit::Persistence::Record::Journal::IEntry& m_entryRecord;
};

}  // end namespaces
}
}
#endif  // end header latch