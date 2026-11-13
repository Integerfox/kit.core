#ifndef KIT_DM_MP_RUN_CONTROL_H_
#define KIT_DM_MP_RUN_CONTROL_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */


#include "Kit/Dm/Mp/Scalar.h"
#include "Kit/Type/BetterEnum.h"
#include <stdint.h>
#include <string.h>

///
namespace Kit {
///
namespace Dm {
///
namespace Mp {


/** This enumeration defines the state transition that is being requested of a
    'runnable' widget

    @param RunRequest                        Enum

    @param NOP                               No operation request, i.e. the initial state before any request has been made
    @param START                             Start the widget
    @param STOP                              Stop the widget
    @param PAUSE                             Pause the widget
    @param RESUME                            Resume the (paused) widget
 */
// clang-format off
BETTER_ENUM( RunRequest, uint8_t
             , NOP = 0
             , START
             , STOP
             , PAUSE
             , RESUME
);
// clang-format on


/** This enumeration defines the actual state of a 'runnable' widget

    @param RunState                          Enum

    @param INVALID                           The widget is in an invalid state, i.e. its actual state is not known
    @param RUNNING                           The widget is running
    @param STOPPED                           The widget is stopped
    @param PAUSED                            The widget is paused
    @param ERRORED                           The widget has encountered an error.  This is a terminal state, i.e. there is no recovery from the ERRORED state
 */
// clang-format off
BETTER_ENUM( RunState, uint8_t
             , INVALID = 0
             , RUNNING
             , STOPPED
             , PAUSED
             , ERRORED
);
// clang-format on


/** This enumeration defines the status of a request

    @param RunRequestStatus                  Enum

    @param NOP                               No operation request, i.e. the initial state before any request has been made
    @param PENDING                           The request is in-flight, i.e. waiting for the widget's response
    @param COMPLETED                         The request was completed successfully
    @param NOT_SUPPORTED                     The requested transition is not supported by the widget
    @param FAILED                            An error occurred while attempting the transition
    @param NOT_ACCEPTED                      The current state does not allow a transition to the requested state, e.g. the widget is in the ERRORED state or the data Model Point is in the invalid state (i.e. the widget's state has not been published)
    @param ERR_REQUEST_IN_PROGRESS           A request is already in progress.  This value is ONLY returned to the requestor, i.e. it is NEVER stored in the Model Point
 */
// clang-format off
BETTER_ENUM( RunRequestStatus, uint8_t
             , NOP = 0
             , PENDING
             , COMPLETED
             , NOT_SUPPORTED
             , FAILED
             , NOT_ACCEPTED
             , ERR_REQUEST_IN_PROGRESS
);
// clang-format on

/** Data stored by the RunControl Model Point.

    The following are needed because the structure is used as a 'data source'
    for a Model Point:
    ----------------------------------------------------------------------------
    - The comparison operation
    - The constructors, copy constructor, and copy operator are implemented to
      ensure that any 'pad' bytes in the structure get zero'd out.  This is
      desired because the default 'comparison operation' in the DM framework
      uses memcmp.
 */
struct RunControl_T
{
    /** Identifier of the most recent request.  The identifier is incremented
        for each new request (and never zero once a request has been made), i.e.
        it allows a requestor to correlate a result with its request even when
        change notifications have been coalesced.
     */
    uint16_t requestId;

    /// The most recent requested state
    RunRequest requested;

    /// The actual state of the widget
    RunState actual;

    /// Status of the most recent request
    RunRequestStatus status;


    /// Constructor (to ensure any pad bytes get zero'd -->needed for brute force compare operation)
    RunControl_T()
    {
        memset( (void*)this, 0, sizeof( RunControl_T ) );
    }

    /// Copy Constructor (to ensure any pad bytes get zero'd)
    RunControl_T( const RunControl_T& other )
    {
        memcpy( (void*)this, (void*)&other, sizeof( RunControl_T ) );
    }

    /// Constructor - all arguments
    RunControl_T( uint16_t         requestId,
                  RunRequest       requested,
                  RunState         actual,
                  RunRequestStatus status )
    {
        memset( (void*)this, 0, sizeof( RunControl_T ) );
        this->requestId = requestId;
        this->requested = requested;
        this->actual    = actual;
        this->status    = status;
    }

    /// Comparison operator (for DM support)
    bool operator==( RunControl_T const& other ) const
    {
        return memcmp( this, &other, sizeof( RunControl_T ) ) == 0;
    }

    /// Copy operator
    RunControl_T& operator=( const RunControl_T& other )
    {
        memcpy( (void*)this, (void*)&other, sizeof( RunControl_T ) );
        return *this;
    }
};


/** This class provides a Model Point that is the 'data' for a closed-loop
    request/response interface to a 'runnable' widget, i.e. a widget that can be
    started, stopped, paused, and resumed.  The Model Point only holds the data
    (and enforces the data's consistency); the policies for using the data are
    implemented by the RunRequestor and RunManager 'pseudo' Model Points. The
    RunRequestor and RunManager are the ONLY intended interfaces for modifying
    the data.

    The concrete Model Point is invalid until the RunManager publishes the
    widget's actual state.  A request is not accepted while the Model Point is
    invalid.

    The actual state ERRORED is a terminal state, i.e. once in the ERRORED
    state, the state cannot be changed (short of invalidating the Model Point)
    and no new requests are accepted.

    IMPORTANT: The Model Point's data is transient/run-time state and MUST NEVER
               be persistently stored.

    The toJSON() format is:
        \code

        { name:"<mpname>", type:"<mptypestring>", valid:true|false, seqnum:nnnn, locked:true|false,
          val:{requested:"<RunRequest>", actual:"<RunState>", status:"<RunRequestStatus>", requestId:nnnn}
        }

        \endcode

    The fromJSON() is NOT supported, i.e. the Model Point cannot be modified via
    a JSON object (use the pseudo Model Points instead).

    NOTE: All methods in this class ARE thread Safe unless explicitly
          documented otherwise.
 */
class RunControl : public Scalar<RunControl_T, RunControl>
{
public:
    /// Constructor. Invalid MP.
    RunControl( Kit::Dm::IModelDatabase& myModelBase, const char* symbolicName );

public:
    ///  See Kit::Dm::ModelPoint.
    const char* getTypeAsText() const noexcept override
    {
        return "Kit::Dm::Mp::RunControl";
    }


public:
    /** This method has 'PACKAGE Scope' in that is should only be called by
        the RunRequestor class. It is ONLY public to avoid the tight coupling of
        C++ friend mechanism.

        Atomically starts a new request.  Returns:
            PENDING                 - The request was started; 'newRequestId' is
                                      the request's identifier
            ERR_REQUEST_IN_PROGRESS - A request is already in-flight
            NOT_ACCEPTED            - The MP is invalid, the actual state is
                                      INVALID or ERRORED, 'request' is NOP, or
                                      the MP is locked

        The MP's sequence number (after the operation) is returned via
        'retSeqNum' (when not null).
     */
    RunRequestStatus issueRequest_( RunRequest request, uint16_t& newRequestId, uint16_t* retSeqNum = nullptr ) noexcept;

    /** This method has 'PACKAGE Scope' in that is should only be called by
        the RunManager class. It is ONLY public to avoid the tight coupling of
        C++ friend mechanism.

        Atomically completes the in-flight request 'requestId' with the
        specified 'result' and updates the actual state.  Returns false (and
        leaves the MP unchanged) when: there is no matching in-flight request,
        'result' is NOP, PENDING, or ERR_REQUEST_IN_PROGRESS, the MP is locked,
        or the transition is not allowed from the ERRORED state.

        The MP's sequence number (after the operation) is returned via
        'retSeqNum' (when not null).
     */
    bool respondToRequest_( uint16_t requestId, RunRequestStatus result, RunState newState, uint16_t* retSeqNum = nullptr ) noexcept;

    /** This method has 'PACKAGE Scope' in that is should only be called by
        the RunManager class. It is ONLY public to avoid the tight coupling of
        C++ friend mechanism.

        Updates the actual state (without affecting an in-flight request).  When
        the MP is invalid, the MP becomes valid with the status COMPLETED and
        no request made (i.e. the requested state is NOP and the request id is
        zero).
        Returns false (and leaves the MP unchanged) when the MP is locked or
        the transition is not allowed from the ERRORED state.

        The MP's sequence number (after the operation) is returned via
        'retSeqNum' (when not null).
     */
    bool updateState_( RunState newState, uint16_t* retSeqNum = nullptr ) noexcept;

public:
    /// See Kit::Dm::Point.  Always fails, i.e. the MP cannot be modified via JSON
    bool fromJSON_( JsonVariant& src, LockRequest_T lockRequest, uint16_t& retSequenceNumber, Kit::Text::IString* errorMsg ) noexcept override;

protected:
    /// See Kit::Dm::Point.
    bool setJSONVal( JsonDocument& doc ) noexcept override;

protected:
    /// write() are not accessible - use the RunRequestor and RunManager instead
    using Scalar<RunControl_T, RunControl>::write;
};


}  // end namespaces
}
}
#endif  // end header latch
