#ifndef KIT_STATESMITH_ICONTEXT_H_
#define KIT_STATESMITH_ICONTEXT_H_
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
namespace Foo {
///
namespace Bar {

/** This abstract class defines the public interface for interacting with
    concrete widget that implements the 'Context' for the MyFSM State Diagram

    NOTE: The public facing interface and the subsequent mapping to FSM events is
          COMPLETELY fabricated and has NO meaningful relationship to the actual
          State Machine diagram.  It simply a mechanism for the example main loop
          to 'send' events to the FSM.

    This interface is NOT thread safe.
 */
class IWidget
{
public:
    /// Initializes/starts the Widget
    virtual void start() noexcept = 0;

    /// This method provides the Widget with periodic 'tick' for its internal processing
    virtual void tick() noexcept = 0;

    /// Notifies the Widget that the application Abc attribute was changed
    virtual void notifyAbcChanged() noexcept = 0;

    /// Notifies the Widget that processing for Xyz was started
    virtual void notifyXyzStarted() noexcept = 0;

    /// Notifies the Widget that processing for Xyz was completed
    virtual void notifyXyzCompleted() noexcept = 0;

public:
    /// Virtual destructor
    virtual ~IWidget() {}
};


}  // end namespaces
}
#endif  // end header latch
