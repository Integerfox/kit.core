#ifndef KIT_EVENT_QUEUE_IQUEUE_H_
#define KIT_EVENT_QUEUE_IQUEUE_H_
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
#include "Kit/EventQueue/IMsgNotification.h"
#include "Kit/System/ITimingSource.h"
#include "Kit/System/IRunnable.h"
#include "Kit/System/IEventManager.h"
#include "Kit/System/ISignable.h"

/** Default to supporting BOTH ITC message and Data Model change notifications
 */
#ifndef USE_KIT_EVENT_QUEUE_ITC_MSG_ONLY
#include "Kit/EventQueue/IChangeNotification.h"

#define KIT_EVENT_QUEUE_SUPPORTED_EVENT_TYPES virtual public IMsgNotification,           \
                                              virtual public IChangeNotification,        \
                                              virtual public Kit::System::IRunnable,     \
                                              virtual public Kit::System::IEventManager, \
                                              virtual public Kit::System::ISignable,     \
                                              virtual public Kit::System::ITimingSource

/// ITC Messages only
#else
#define KIT_EVENT_QUEUE_SUPPORTED_EVENT_TYPES virtual public IMsgNotification,           \
                                              virtual public Kit::System::IRunnable,     \
                                              virtual public Kit::System::IEventManager, \
                                              virtual public Kit::System::ISignable,     \
                                              virtual public Kit::System::ITimingSource
#endif  // end USE_KIT_EVENT_QUEUE_ITC_MSG_ONLY


///
namespace Kit {
///
namespace EventQueue {


/** This abstract class represents an Event Queue where events can be
    delivered to.  See README.md for additional details
 */
class IQueue : KIT_EVENT_QUEUE_SUPPORTED_EVENT_TYPES
{
};


}  // end namespaces
}
#endif  // end header latch
