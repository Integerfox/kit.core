#ifndef KIT_DM_MP_RUN_MANAGER_H_
#define KIT_DM_MP_RUN_MANAGER_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */


#include "Kit/Dm/Mp/RunControl.h"
#include "Kit/Dm/Mp/PseudoMpBase.h"

///
namespace Kit {
///
namespace Dm {
///
namespace Mp {


/** This class provides the 'manager' side of a RunControl Model Point, i.e.
    the interface for the owner of a 'runnable' widget to receive requests (from
    a RunRequestor), respond to the requests, and maintain the widget's actual
    state.

    The owner of the RunManager instance determines the request's status, i.e.
    the class does not decide whether a transition is supported or allowed.
    The following guidelines are recommended so that all widgets behave
    consistently:
        - A repeated request, i.e. a request for the state the widget is already
          in (e.g. STOP when STOPPED) is responded to with COMPLETED.
        - A transition that the widget never supports is responded to with
          NOT_SUPPORTED.
        - A transition that is not allowed from the current state is responded
          to with NOT_ACCEPTED.
        - A transition that was attempted, but encountered an error, is responded
          to with FAILED (and, when the widget cannot continue, the ERRORED
          state).

    The class enforces the following policies:
        - A response must match the in-flight request (identified by its
          request identifier), and must be a final status (not NOP/PENDING/ERR_REQUEST_IN_PROGRESS).
        - The ERRORED state is terminal, i.e. once in the ERRORED state the
          actual state cannot be changed.

    The RunControl Model Point is invalid until setState() is called the first
    time.  The owner is responsible for responding to every request; the
    class does NOT time-out requests.

    The toJSON() output is that of the RunControl Model Point.  The fromJSON()
    format only supports the writer methods:
        \code

        val:{state:"<RunState>"}                                           // setState()
        val:{id:nnnn, status:"<RunRequestStatus>", newState:"<RunState>"}  // respond()

        The two forms use different keys so that they cannot be confused, i.e.
        the keys cannot be mixed.

        Examples:
                { name:"mp_runManager", val:{state:"STOPPED"} }
                { name:"mp_runManager", val:{id:1, status:"COMPLETED", newState:"RUNNING"} }

        \endcode

    IMPORTANT: This (pseudo) Model Point is transient/run-time state and MUST
               NEVER be persistently stored.

    NOTE: All methods in this class ARE thread Safe unless explicitly
          documented otherwise.
 */
class RunManager : public PseudoMpBase<RunControl_T, RunControl>
{
public:
    /// Constructor.  The RunControl Model Point must outlive the instance
    RunManager( Kit::Dm::IModelDatabase& myModelBase, const char* symbolicName, RunControl& dataMp );

public:
    /** Returns true if there is a request in-flight and returns the request
        and its identifier.  Returns false when there is not a request in-flight

        This method should NOT be used within an observer's change notification
        callback (use getPendingRequestAndSync() instead).
     */
    bool getPendingRequest( RunRequest& retRequest, uint16_t& retRequestId ) const noexcept;

    /** Same as getPendingRequest(), except that the 'observerToSync' is also
        synchronized with the data Model Point (and re-subscribed for change
        notifications).  This method should ONLY be used within an observer's
        change notification callback, and 'observerToSync' MUST be the
        observer argument provided to the callback.  Note: Even when false is
        returned, the observer is re-subscribed.
     */
    bool getPendingRequestAndSync( RunRequest& retRequest, uint16_t& retRequestId, IObserver& observerToSync ) noexcept;

    /** Responds to the in-flight request 'requestId' with the final status
        'result' and the widget's actual state after the request.  Returns false
        (and nothing is changed) if the response violates the class's policies.
        The data Model Point's sequence number (after the operation) is
        optionally returned via 'retSeqNum'.
     */
    bool respond( uint16_t requestId, RunRequestStatus result, RunState actualState, uint16_t* retSeqNum = nullptr ) noexcept;

    /** Sets the widget's actual state (e.g. initial state, or a state change
        that was not the result of a request) without affecting an in-flight
        request.  Returns false (and nothing is changed) if the widget is in the
        ERRORED state and 'actualState' is not ERRORED.  The data Model Point's
        sequence number (after the operation) is optionally returned via
        'retSeqNum'.
     */
    bool setState( RunState actualState, uint16_t* retSeqNum = nullptr ) noexcept;

    ///  See Kit::Dm::ModelPoint.
    const char* getTypeAsText() const noexcept override
    {
        return "Kit::Dm::Mp::RunManager";
    }

public:
    /// See Kit::Dm::Point.  Only supports the setState() and respond() methods
    bool fromJSON_( JsonVariant& src, LockRequest_T lockRequest, uint16_t& retSequenceNumber, Kit::Text::IString* errorMsg ) noexcept override;
};


}  // end namespaces
}
}
#endif  // end header latch
