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

/** Default to supporting BOTH ITC message and Data Model change notifications
 */
#ifndef USE_KIT_EVENT_QUEUE_ITC_MSG_ONLY
#define KIT_EVENT_QUEUE_SUPPORTED_EVENT_TYPES public IMsgNotification, public IChangeNotification
#include "Kit/EventQueue/IChangeNotification.h"

/// ITC Messages only
#else
#define KIT_EVENT_QUEUE_SUPPORTED_EVENT_TYPES public IMsgNotification
#endif  // end USE_KIT_EVENT_QUEUE_ITC_MSG_ONLY


///
namespace Kit {
///
namespace EventQueue {



/** This abstract class represents an Event Queue where events can be 
    delivered to.  See README.txt for additional details
 */
class IQueue: KIT_EVENT_QUEUE_SUPPORTED_EVENT_TYPES
{
};


}  // end namespaces
}
#endif  // end header latch
