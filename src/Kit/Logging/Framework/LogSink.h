#ifndef Cpl_Logging_LogSink_h_
#define Cpl_Logging_LogSink_h_
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
// #include "Kit/Container/RingBufferMP.h"
#include "Kit/EventQueue/IQueue.h"
#include "Kit/Itc/OpenCloseSync.h"
// #include "Kit/Dm/SubscriberComposer.h"
#include "Kit/Logging/Framework/EntryData.h"


/** Maximum number of entries that can be written as the result of single
    change notification
 */
#ifndef OPTION_KIT_LOGGING_FRAMEWORK_MAX_BATCH_WRITE
#define OPTION_KIT_LOGGING_FRAMEWORK_MAX_BATCH_WRITE 4
#endif


///
namespace Kit {
///
namespace Logging {
///
namespace Framework {

/** This class is responsible for consuming the application's Log buffer and
    'dispatching' log entries.  By default, the class simply drains the
    log buffer and discards the entries.  A child class is required to provide
    a meaningful implementation of the dispatchLogEntry() method.

    FYI: The Kit::Persistent framework provides an alternate 'log sink' that
         writes log entries to Non-volatile storage along with an a API to
         retrieve the log entries.
 */
class LogSink : public Kit::Itc::OpenCloseSync
{
public:
    /// Constructor
    LogSink( Kit::EventQueue::IQueue& myMbox /*,
             Kit::Container::RingBufferMP<Kit::Logging::EntryData_T>& incomingEntriesBuffer */ ) noexcept;

public:
    /// This method starts the sink (See Kit::Itc::OpenCloseSync)
    void request( OpenMsg& msg ) noexcept override;

    /// This method stops the sink (See Kit::Itc::OpenCloseSync)
    void request( CloseMsg& msg ) noexcept override;

protected:
    /// Element Count Change notification
    // void elementCountChanged( Kit::Dm::Mp::Uint32& mp, Kit::Dm::SubscriberApi& clientObserver ) noexcept;

protected:
    /// Platform specific method to save the entry
    virtual void dispatchLogEntry( Kit::Logging::Framework::EntryData_T& src )
    {
        // Default is to do nothing
    }

protected:
    /// Observer for change notification (to the RingBuffer)
    // Kit::Dm::SubscriberComposer<LogSink, Kit::Dm::Mp::Uint32>   m_observerElementCount;

    /// The Log Ring Buffer
    // Kit::Container::RingBufferMP<Kit::Logging::Framework::EntryData_T>&    m_logBuffer;

    /// Track my open state
    bool m_opened;
};


}  // end namespaces
}
}
#endif  // end header latch