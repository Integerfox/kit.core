#ifndef KIT_PERSISTENCE_RECORD_JOURNAL_IREADERREQUEST_H
#define KIT_PERSISTENCE_RECORD_JOURNAL_IREADERREQUEST_H
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
#include "Kit/Persistence/Record/Journal/IEntry.h"

///
namespace Kit {
///
namespace Persistence {
///
namespace Record {
///
namespace Journal {

/** This abstract class define ITC message type and payload for the application
    to request read Journal Entry data

    See the Kit/Itc/README.txt file for the semantics for the 'ownership' of the
    payload contents.

    NOTE: This interface can/should NOT be used synchronously.  The application
          is required to only use asynchronous semantics.
 */
class RetrieveLatestRequest
{
public:
    /// SAP for this API
    typedef Kit::Itc::SAP<RetrieveLatestRequest> SAP;

public:
    /// Payload for Message: RetrieveLatest
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
    /// Message Type: RetrieveLatest
    typedef Kit::Itc::RequestMessage<RetrieveLatestRequest, Payload> RetrieveLatestMsg;

    /// Request: RetrieveLatest message
    virtual void request( RetrieveLatestMsg& msg ) = 0;

public:
    /// Virtual Destructor
    virtual ~RetrieveLatestRequest() {}
};


/** This abstract class define ITC message type and payload for asynchronous
    response (to the application) of a RetrieveLatest message.

    The Application is responsible for implementing the response method(s).
 */
class RetrieveLatestResponse
{
public:
    /// Response Message Type
    typedef Kit::Itc::ResponseMessage<RetrieveLatestResponse,
                                      RetrieveLatestRequest,
                                      RetrieveLatestRequest::Payload>
        RetrieveLatestMsg;

public:
    /// Response
    virtual void response( RetrieveLatestMsg& msg ) = 0;


public:
    /// Virtual destructor
    virtual ~RetrieveLatestResponse() {}
};

////////////////////////////////////////////////////////////////////////////////
/** This abstract class define ITC message type and payload for the application
    to request read Journal Entry data

    See the Kit/Itc/README.txt file for the semantics for the 'ownership' of the
    payload contents.

    NOTE: This interface can/should NOT be used synchronously.  The application
          is required to only use asynchronous semantics.
 */
class RetrieveNextRequest
{
public:
    /// SAP for this API
    typedef Kit::Itc::SAP<RetrieveNextRequest> SAP;

public:
    /// Payload for Message: RetrieveNext
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
    /// Message Type: RetrieveNext
    typedef Kit::Itc::RequestMessage<RetrieveNextRequest, Payload> RetrieveNextMsg;

    /// Request: RetrieveNext message
    virtual void request( RetrieveNextMsg& msg ) = 0;

public:
    /// Virtual Destructor
    virtual ~RetrieveNextRequest() {}
};


/** This abstract class define ITC message type and payload for asynchronous
    response (to the application) of a RetrieveNext message.

    The Application is responsible for implementing the response method(s).
 */
class RetrieveNextResponse
{
public:
    /// Response Message Type
    typedef Kit::Itc::ResponseMessage<RetrieveNextResponse,
                                      RetrieveNextRequest,
                                      RetrieveNextRequest::Payload>
        RetrieveNextMsg;

public:
    /// Response
    virtual void response( RetrieveNextMsg& msg ) = 0;


public:
    /// Virtual destructor
    virtual ~RetrieveNextResponse() {}
};

////////////////////////////////////////////////////////////////////////////////
/** This abstract class define ITC message type and payload for the application
    to request read Journal Entry data

    See the Kit/Itc/README.txt file for the semantics for the 'ownership' of the
    payload contents.

    NOTE: This interface can/should NOT be used synchronously.  The application
          is required to only use asynchronous semantics.
 */
class RetrievePreviousRequest
{
public:
    /// SAP for this API
    typedef Kit::Itc::SAP<RetrievePreviousRequest> SAP;

public:
    /// Payload for Message: RetrievePrevious
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


    /// Message Type: RetrievePrevious
    typedef Kit::Itc::RequestMessage<RetrievePreviousRequest, Payload> RetrievePreviousMsg;

    /// Request: RetrievePrevious message
    virtual void request( RetrievePreviousMsg& msg ) = 0;

public:
    /// Virtual Destructor
    virtual ~RetrievePreviousRequest() {}
};

/** This abstract class define ITC message type and payload for asynchronous
    response (to the application) of a RetrievePrevious message.

    The Application is responsible for implementing the response method(s).
 */
class RetrievePreviousResponse
{
public:
    /// Response Message Type
    typedef Kit::Itc::ResponseMessage<RetrievePreviousResponse,
                                      RetrievePreviousRequest,
                                      RetrievePreviousRequest::Payload>
        RetrievePreviousMsg;

public:
    /// Response
    virtual void response( RetrievePreviousMsg& msg ) = 0;


public:
    /// Virtual destructor
    virtual ~RetrievePreviousResponse() {}
};


////////////////////////////////////////////////////////////////////////////////
/** This abstract class define ITC message type and payload for the application
    to request read Journal Entry data

    See the Kit/Itc/README.txt file for the semantics for the 'ownership' of the
    payload contents.

    NOTE: This interface can/should NOT be used synchronously.  The application
          is required to only use asynchronous semantics.
 */
class RetrieveByEntryIndexRequest
{
public:
    /// SAP for this API
    typedef Kit::Itc::SAP<RetrieveByEntryIndexRequest> SAP;

public:
    /// Payload for Message: RetrieveByBufferIndex
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


    /// Message Type: RetrieveByBufferIndex
    typedef Kit::Itc::RequestMessage<RetrieveByEntryIndexRequest, Payload> RetrieveByBufferIndexMsg;

    /// Request: RetrieveByBufferIndex message
    virtual void request( RetrieveByBufferIndexMsg& msg ) = 0;

public:
    /// Virtual Destructor
    virtual ~RetrieveByEntryIndexRequest() {}
};

/** This abstract class define ITC message type and payload for asynchronous
    response (to the application) of a RetrieveByEntryIndexRequest message.

    The Application is responsible for implementing the response method(s).
 */
class RetrieveByEntryIndexResponse
{
public:
    /// Response Message Type
    typedef Kit::Itc::ResponseMessage<RetrieveByEntryIndexResponse,
                                      RetrieveByEntryIndexRequest,
                                      RetrieveByEntryIndexRequest::Payload>
        RetrieveByEntryIndexMsg;

public:
    /// Response
    virtual void response( RetrieveByEntryIndexMsg& msg ) = 0;


public:
    /// Virtual destructor
    virtual ~RetrieveByEntryIndexResponse() {}
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
