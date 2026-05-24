#ifndef KIT_PERSISTENCE_RECORD_JOURNAL_RESET_SYNC_H
#define KIT_PERSISTENCE_RECORD_JOURNAL_RESET_SYNC_H
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Persistence/Record/Journal/IReset.h"
#include "Kit/Persistence/Record/Journal/IResetRequest.h"

///
namespace Kit {
///
namespace Persistence {
///
namespace Record {
///
namespace Journal {

/** This concrete class provides a synchronous ITC wrapper for the
    IReset::logicalReset() method.  A child class is required since the class
    does not have a public constructor.  The expectation is the class
    implementing the LogicalResetRequest ITC message will be the child class.
*/
class ResetSync : public IReset,
                  public LogicalResetRequest
{
public:
    /// See Kit::Persistence::Record::Journal::IReset
    bool logicalReset() noexcept override;

protected:
    /// Constructor.
    ResetSync( Kit::EventQueue::IQueue& eventQueue ) noexcept
        : m_myEventQueue( eventQueue )
    {
    }

protected:
    /// IEventQueue of the child class implementing the LogicalResetRequest method
    Kit::EventQueue::IQueue& m_myEventQueue;
};


}  // end namespaces
}
}
}
#endif  // end header latch
