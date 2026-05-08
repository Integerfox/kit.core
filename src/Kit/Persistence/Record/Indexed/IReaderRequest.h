#ifndef KIT_PERSISTENCE_RECORD_INDEXED_IENTRYREQUEST_H
#define KIT_PERSISTENCE_RECORD_INDEXED_IENTRYREQUEST_H
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Itc/SAP.h"
#include "Kit/Itc/RequestMessage.h"
#include "Kit/Itc/ResponseMessage.h"
#include "Kit/Persistence/Record/IPayload.h"
#include "Kit/Persistence/Record/Indexed/IEntry.h"

///
namespace Kit {
///
namespace Persistence {
///
namespace Record {
///
namespace Indexed {

/** This abstract class define ITC message type and payload for the application
    to request read Indexed Entry data

    See the Kit/Itc/README.txt file for the semantics for the 'ownership' of the
    payload contents.

    NOTE: This interface can/should NOT be used synchronously.  The application
          is required to only use asynchronous semantics.
 */
class GetLatestRequest
{
public:
    /// SAP for this API
    typedef Kit::Itc::SAP<GetLatestRequest> SAP;

public:
    /// Payload for Message: GetLatest
    class Payload
    {
    public:
        /// INPUT/OUTPUT: Memory to hold the retrieved entry
        Kit::Persistence::Record::IPayload& m_entryDst;

        /// OUTPUT (response field): The 'marker' associated with the found/retrieved entry
        IEntry::Marker_T m_markerEntryRetrieved;

        /** GET results (response field)
            true  = entry was successfully retrieved/found
            false = no valid entry (that met the getXxx() criteria)
         */
        bool m_success;

    public:
        /// Constructor. Use for getLatest() message
        Payload( Kit::Persistence::Record::IPayload& entryDst )
            : m_entryDst( entryDst ), m_success( false )
        {
        }
    };


public:
    /// Message Type: GetLatest
    typedef Kit::Itc::RequestMessage<GetLatestRequest, Payload> GetLatestMsg;

    /// Request: GetLatest message
    virtual void request( GetLatestMsg& msg ) = 0;

public:
    /// Virtual Destructor
    virtual ~GetLatestRequest() {}
};


/** This abstract class define ITC message type and payload for asynchronous
    response (to the application) of a GetLatest message.

    The Application is responsible for implementing the response method(s).
 */
class GetLatestResponse
{
public:
    /// Response Message Type
    typedef Kit::Itc::ResponseMessage<GetLatestResponse,
                                      GetLatestRequest,
                                      GetLatestRequest::Payload>
        GetLatestMsg;

public:
    /// Response
    virtual void response( GetLatestMsg& msg ) = 0;


public:
    /// Virtual destructor
    virtual ~GetLatestResponse() {}
};

////////////////////////////////////////////////////////////////////////////////
/** This abstract class define ITC message type and payload for the application
    to request read Indexed Entry data

    See the Kit/Itc/README.txt file for the semantics for the 'ownership' of the
    payload contents.

    NOTE: This interface can/should NOT be used synchronously.  The application
          is required to only use asynchronous semantics.
 */
class GetNextRequest
{
public:
    /// SAP for this API
    typedef Kit::Itc::SAP<GetNextRequest> SAP;

public:
    /// Payload for Message: GetNext
    class Payload
    {
    public:
        /// INPUT: newer timestamp to search criteria
        uint64_t m_newerThan;

        /// INPUT/OUTPUT: Memory to hold the retrieved entry
        Kit::Persistence::Record::IPayload& m_entryDst;

        /// INPUT: The 'marker' on where to begin searching from
        IEntry::Marker_T& m_beginHereMarker;

        /// OUTPUT (response field): The 'marker' associated with the found/retrieved entry
        IEntry::Marker_T m_markerEntryRetrieved;

        /** GET results (response field)
            true  = entry was successfully retrieved/found
            false = no valid entry (that met the getXxx() criteria)
         */
        bool m_success;

    public:
        /// Constructor. Use for getNext() message
        Payload( Kit::Persistence::Record::IPayload& entryDst, IEntry::Marker_T& beginHere, uint64_t newerThan )
            : m_newerThan( newerThan ), m_entryDst( entryDst ), m_beginHereMarker( beginHere ), m_success( false )
        {
        }
    };

public:
    /// Message Type: GetNext
    typedef Kit::Itc::RequestMessage<GetNextRequest, Payload> GetNextMsg;

    /// Request: GetNext message
    virtual void request( GetNextMsg& msg ) = 0;

public:
    /// Virtual Destructor
    virtual ~GetNextRequest() {}
};


/** This abstract class define ITC message type and payload for asynchronous
    response (to the application) of a GetNext message.

    The Application is responsible for implementing the response method(s).
 */
class GetNextResponse
{
public:
    /// Response Message Type
    typedef Kit::Itc::ResponseMessage<GetNextResponse,
                                      GetNextRequest,
                                      GetNextRequest::Payload>
        GetNextMsg;

public:
    /// Response
    virtual void response( GetNextMsg& msg ) = 0;


public:
    /// Virtual destructor
    virtual ~GetNextResponse() {}
};

////////////////////////////////////////////////////////////////////////////////
/** This abstract class define ITC message type and payload for the application
    to request read Indexed Entry data

    See the Kit/Itc/README.txt file for the semantics for the 'ownership' of the
    payload contents.

    NOTE: This interface can/should NOT be used synchronously.  The application
          is required to only use asynchronous semantics.
 */
class GetPreviousRequest
{
public:
    /// SAP for this API
    typedef Kit::Itc::SAP<GetPreviousRequest> SAP;

public:
    /// Payload for Message: GetPrevious
    class Payload
    {
    public:
        /// INPUT: newer timestamp to search criteria
        uint64_t m_olderThan;

        /// INPUT (optional): The 'marker' on where to begin searching from
        IEntry::Marker_T& m_beginHereMarker;

        /// INPUT/OUTPUT: Memory to hold the retrieved entry
        Kit::Persistence::Record::IPayload& m_entryDst;

        /// OUTPUT (response field): The 'marker' associated with the found/retrieved entry
        IEntry::Marker_T m_markerEntryRetrieved;

        /** GET results (response field)
            true  = entry was successfully retrieved/found
            false = no valid entry (that met the getXxx() criteria)
         */
        bool m_success;

    public:
        /// Constructor. Use for getPrevious() message
        Payload( Kit::Persistence::Record::IPayload& entryDst, IEntry::Marker_T& beginHere, uint64_t olderThan )
            : m_olderThan( olderThan ), m_beginHereMarker( beginHere ), m_entryDst( entryDst ), m_success( false )
        {
        }
    };


    /// Message Type: GetPrevious
    typedef Kit::Itc::RequestMessage<GetPreviousRequest, Payload> GetPreviousMsg;

    /// Request: GetPrevious message
    virtual void request( GetPreviousMsg& msg ) = 0;

public:
    /// Virtual Destructor
    virtual ~GetPreviousRequest() {}
};

/** This abstract class define ITC message type and payload for asynchronous
    response (to the application) of a GetPrevious message.

    The Application is responsible for implementing the response method(s).
 */
class GetPreviousResponse
{
public:
    /// Response Message Type
    typedef Kit::Itc::ResponseMessage<GetPreviousResponse,
                                      GetPreviousRequest,
                                      GetPreviousRequest::Payload>
        GetPreviousMsg;

public:
    /// Response
    virtual void response( GetPreviousMsg& msg ) = 0;


public:
    /// Virtual destructor
    virtual ~GetPreviousResponse() {}
};


////////////////////////////////////////////////////////////////////////////////
/** This abstract class define ITC message type and payload for the application
    to request read Indexed Entry data

    See the Kit/Itc/README.txt file for the semantics for the 'ownership' of the
    payload contents.

    NOTE: This interface can/should NOT be used synchronously.  The application
          is required to only use asynchronous semantics.
 */
class GetByEntryIndexRequest
{
public:
    /// SAP for this API
    typedef Kit::Itc::SAP<GetByEntryIndexRequest> SAP;

public:
    /// Payload for Message: GetByBufferIndex
    class Payload
    {
    public:
        /// INPUT: buffer index
        size_t m_index;

        /// INPUT/OUTPUT: Memory to hold the retrieved entry
        Kit::Persistence::Record::IPayload& m_entryDst;

        /// OUTPUT (response field): The 'marker' associated with the found/retrieved entry
        IEntry::Marker_T m_markerEntryRetrieved;

        /** GET results (response field)
            true  = entry was successfully retrieved/found
            false = no valid entry (that met the getXxx() criteria)
         */
        bool m_success;

    public:
        /// Constructor. Use for getLatest() message
        Payload( Kit::Persistence::Record::IPayload& entryDst, size_t index )
            : m_index( index ), m_entryDst( entryDst ), m_success( false )
        {
        }
    };


    /// Message Type: GetByBufferIndex
    typedef Kit::Itc::RequestMessage<GetByEntryIndexRequest, Payload> GetByBufferIndexMsg;

    /// Request: GetByBufferIndex message
    virtual void request( GetByBufferIndexMsg& msg ) = 0;

public:
    /// Virtual Destructor
    virtual ~GetByEntryIndexRequest() {}
};

/** This abstract class define ITC message type and payload for asynchronous
    response (to the application) of a GetByEntryIndexRequest message.

    The Application is responsible for implementing the response method(s).
 */
class GetByEntryIndexResponse
{
public:
    /// Response Message Type
    typedef Kit::Itc::ResponseMessage<GetByEntryIndexResponse,
                                      GetByEntryIndexRequest,
                                      GetByEntryIndexRequest::Payload>
        GetByEntryIndexMsg;

public:
    /// Response
    virtual void response( GetByEntryIndexMsg& msg ) = 0;


public:
    /// Virtual destructor
    virtual ~GetByEntryIndexResponse() {}
};

////////////////////////////////////////////////////////////////////////////////
/** This abstract class define ITC message type and payload for the application
    to clear/delete all entries

    See the Kit/Itc/README.txt file for the semantics for the 'ownership' of the
    payload contents.

    NOTE: This interface can/should NOT be used synchronously.  The application
          is required to only use asynchronous semantics.
 */
class ResetHeadRequest
{
public:
    /// SAP for this API
    typedef Kit::Itc::SAP<ResetHeadRequest> SAP;

public:
    /// Payload for Message
    class Payload
    {
    public:
        /** Clear results (response field)
            true  = all entries where successfully cleared
            false = an error occurred
         */
        bool m_success;

    public:
        /// Constructor. Use for resetHead() message
        Payload()
            : m_success( false )
        {
        }
    };


    /// Message Type: ResetHead
    typedef Kit::Itc::RequestMessage<ResetHeadRequest, Payload> ResetHeadMsg;

    /// Request: ResetHead message
    virtual void request( ResetHeadMsg& msg ) = 0;

public:
    /// Virtual Destructor
    virtual ~ResetHeadRequest() {}
};

/** This abstract class define ITC message type and payload for asynchronous
    response (to the application) of a ResetHead message.

    The Application is responsible for implementing the response method(s).
 */
class ResetHeadResponse
{
public:
    /// Response Message Type
    typedef Kit::Itc::ResponseMessage<ResetHeadResponse,
                                      ResetHeadRequest,
                                      ResetHeadRequest::Payload>
        ResetHeadMsg;

public:
    /// Response
    virtual void response( ResetHeadMsg& msg ) = 0;

public:
    /// Virtual destructor
    virtual ~ResetHeadResponse() {}
};


}  // end namespaces
}
}
}
#endif  // end header latch
