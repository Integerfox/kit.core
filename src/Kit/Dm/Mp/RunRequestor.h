#ifndef KIT_DM_MP_RUN_REQUESTOR_H_
#define KIT_DM_MP_RUN_REQUESTOR_H_
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


/** This class provides the 'requestor' side of a RunControl Model Point, i.e.
    the interface for a client that wants to start, stop, pause, or resume a
    'runnable' widget and to monitor the result of the request.  The widget's
    side is the RunManager class.

    The class enforces the following policies:
        - Only one request can be in-flight at any given time (the request
          is ERR_REQUEST_IN_PROGRESS when there is already a request in-flight).
        - A request is NOT_ACCEPTED when the RunControl Model Point is invalid
          (i.e. the RunManager has not published the widget's state), the
          widget's state is INVALID or ERRORED, or the request is NOP.

    A request that is accepted is PENDING until the RunManager responds.  A
    client monitors the result by attaching an observer and comparing the
    request's identifier (returned by request()) with the RunControl data's
    'requestId' (change notifications may be coalesced, i.e. the client may not
    see every status transition).

    The toJSON() output is that of the RunControl Model Point.  The fromJSON()
    format only supports issuing a request:
        \code

        val:"START|STOP|PAUSE|RESUME"

        Example:
                { name:"mp_runRequestor", val:"START" }

        \endcode

    IMPORTANT: This (pseudo) Model Point is transient/run-time state and MUST
               NEVER be persistently stored.

    NOTE: All methods in this class ARE thread Safe unless explicitly
          documented otherwise.
 */
class RunRequestor : public PseudoMpBase<RunControl_T, RunControl>
{
public:
    /// Constructor.  The RunControl Model Point must outlive the instance
    RunRequestor( Kit::Dm::IModelDatabase& myModelBase, const char* symbolicName, RunControl& dataMp );

public:
    /** Makes a request.  Returns PENDING when the request was accepted, and
        'retRequestId' is set to the request's identifier.  The identifier is
        used to correlate the request with its result (see readStatus()).
        Otherwise returns ERR_REQUEST_IN_PROGRESS (a request is already
        in-flight) or NOT_ACCEPTED (see class description) and 'retRequestId'
        is not updated.  The data Model Point's sequence number (after the
        operation) is optionally returned via 'retSeqNum'.
     */
    RunRequestStatus request( RunRequest request, uint16_t& retRequestId, uint16_t* retSeqNum = nullptr ) noexcept;

    /// Convenience method. See request()
    inline RunRequestStatus start( uint16_t& retRequestId, uint16_t* retSeqNum = nullptr ) noexcept { return request( RunRequest::START, retRequestId, retSeqNum ); }

    /// Convenience method. See request()
    inline RunRequestStatus stop( uint16_t& retRequestId, uint16_t* retSeqNum = nullptr ) noexcept { return request( RunRequest::STOP, retRequestId, retSeqNum ); }

    /// Convenience method. See request()
    inline RunRequestStatus pause( uint16_t& retRequestId, uint16_t* retSeqNum = nullptr ) noexcept { return request( RunRequest::PAUSE, retRequestId, retSeqNum ); }

    /// Convenience method. See request()
    inline RunRequestStatus resume( uint16_t& retRequestId, uint16_t* retSeqNum = nullptr ) noexcept { return request( RunRequest::RESUME, retRequestId, retSeqNum ); }

    /** Reads the status of the most recent request, the identifier of the most
        recent request (compare with the identifier returned by request() to
        determine if the status is for the caller's request), and the widget's
        actual state, i.e. a convenience for calling read().  The data Model
        Point's sequence number is optionally returned via 'retSeqNum'.  Returns
        false (and the output arguments are not updated) when the data Model
        Point is invalid.

        This method should NOT be used within an observer's change notification
        callback (use readStatusAndSync() instead).
     */
    bool readStatus( RunRequestStatus& retStatus,
                     RunState&         retActualState,
                     uint16_t&         retRequestId,
                     uint16_t*         retSeqNum = nullptr ) const noexcept;

    /** Same as readStatus(), except that the 'observerToSync' is also
        synchronized with the data Model Point (and re-subscribed for change
        notifications).  This method should ONLY be used within an observer's
        change notification callback, and 'observerToSync' MUST be the
        observer argument provided to the callback.  Note: Even when false is
        returned, the observer is re-subscribed.
     */
    bool readStatusAndSync( RunRequestStatus& retStatus,
                            RunState&         retActualState,
                            uint16_t&         retRequestId,
                            IObserver&        observerToSync,
                            uint16_t*         retSeqNum = nullptr ) noexcept;

    ///  See Kit::Dm::ModelPoint.
    const char* getTypeAsText() const noexcept override
    {
        return "Kit::Dm::Mp::RunRequestor";
    }

public:
    /// See Kit::Dm::Point.  Only supports issuing a request
    bool fromJSON_( JsonVariant& src, LockRequest_T lockRequest, uint16_t& retSequenceNumber, Kit::Text::IString* errorMsg ) noexcept override;
};


}  // end namespaces
}
}
#endif  // end header latch
