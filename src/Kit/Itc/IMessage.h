#ifndef KIT_ITC_IMESSAGE_h_
#define KIT_ITC_IMESSAGE_h_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Container/ListItem.h"

///
namespace Kit {
///
namespace Itc {

/** This abstract class defines the operations for an ITC message
 */
class IMessage : public Kit::Container::ExtendedListItem
{
public:
    /** This abstract operation is invoked by the mailbox owner to cause the
        message to be processed after it has been retrieved from the mailbox.
     */
    virtual void process() noexcept = 0;

public:
    /// Virtual destructor
    virtual ~IMessage() = default;
};

}  // end namespaces
}
#endif  // end header latch
