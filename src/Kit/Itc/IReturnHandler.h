#ifndef KIT_ITC_IRETURN_HANDLER_h_
#define KIT_ITC_IRETURN_HANDLER_h_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

///
namespace Kit {
///
namespace Itc {

/** This abstract class represents the action that is executed by ITC framework
    in response to a Service completing the operation requested by a ServerMessage.
    The ReturnHandler is executed when a Service invokes the returnToSender()
    operation of the message. Implementations of this class are the responsibility
    of the client (e.g. the client is using synchronous or asynchronous semantics
    when issuing the request).
 */
class IReturnHandler
{
public:
    /** This abstract operation is invoked when a Service has completed processing
        the client request message. The child implementations should notify
        the client that the request is complete.
     */
    virtual void rts() noexcept = 0;

public:
    /// Virtual destructor
    virtual ~IReturnHandler() = default;
};

}  // end namespaces
}
#endif  // end header latch