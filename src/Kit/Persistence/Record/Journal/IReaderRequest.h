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
class ReadRequest
{
public:
    /// SAP for this API
    typedef Kit::Itc::SAP<ReadRequest> SAP;

public:
    /// Payload for Message: RetrieveLatest
    class LatestPayload
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
        LatestPayload( Kit::Persistence::Record::IPayload& entryDst )
            : m_entryDst( entryDst ), m_success( false )
        {
        }
    };

    /// Message Type: RetrieveLatest
    typedef Kit::Itc::RequestMessage<ReadRequest, LatestPayload> RetrieveLatestMsg;

    /// Request: RetrieveLatest message
    virtual void request( RetrieveLatestMsg& msg ) = 0;

public:
    /// Payload for Message: RetrieveNext
    class NextPayload
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
        NextPayload( Kit::Persistence::Record::IPayload& entryDst, IEntry::Marker_T& beginHere, uint64_t newerThan )
            : m_newerThan( newerThan ), m_entryDst( entryDst ), m_beginHereMarker( beginHere ), m_success( false )
        {
        }
    };

    /// Message Type: RetrieveNext
    typedef Kit::Itc::RequestMessage<ReadRequest, NextPayload> RetrieveNextMsg;

    /// Request: RetrieveNext message
    virtual void request( RetrieveNextMsg& msg ) = 0;

public:
    /// Payload for Message: RetrievePrevious
    class PreviousPayload
    {
    public:
        /// INPUT: newer timestamp to search criteria
        uint64_t m_olderThan;

        /// INPUT: The 'marker' on where to begin searching from
        IEntry::Marker_T& m_beginHereMarker;

        /// INPUT/OUTPUT: Memory to hold the retrieved entry
        Kit::Persistence::Record::IPayload& m_entryDst;

        /// OUTPUT (response field): The 'marker' associated with the found/retrieved entry
        IEntry::Marker_T m_markerEntryRetrieved;

        /** GET results (response field)
            true  = entry was successfully retrieved/found
            false = no valid entry (that met the retrieveXxx() criteria)
         */
        bool m_success;

    public:
        /// Constructor. Use for retrievePrevious() message
        PreviousPayload( Kit::Persistence::Record::IPayload& entryDst, IEntry::Marker_T& beginHere, uint64_t olderThan )
            : m_olderThan( olderThan ), m_beginHereMarker( beginHere ), m_entryDst( entryDst ), m_success( false )
        {
        }
    };

    /// Message Type: RetrievePrevious
    typedef Kit::Itc::RequestMessage<ReadRequest, PreviousPayload> RetrievePreviousMsg;

    /// Request: RetrievePrevious message
    virtual void request( RetrievePreviousMsg& msg ) = 0;

public:
    /// Payload for Message: RetrieveByEntryIndex
    class ByEntryIndexPayload
    {
    public:
        /// INPUT: entry index
        Size_T m_index;

        /// INPUT/OUTPUT: Memory to hold the retrieved entry
        Kit::Persistence::Record::IPayload& m_entryDst;

        /// OUTPUT (response field): The 'marker' associated with the found/retrieved entry
        IEntry::Marker_T m_markerEntryRetrieved;

        /** GET results (response field)
            true  = entry was successfully retrieved/found
            false = no valid entry (that met the retrieveXxx() criteria)
         */
        bool m_success;

    public:
        /// Constructor. Use for retrieveByEntryIndex() message
        ByEntryIndexPayload( Kit::Persistence::Record::IPayload& entryDst, Size_T index )
            : m_index( index ), m_entryDst( entryDst ), m_success( false )
        {
        }
    };


    /// Message Type: RetrieveByEntryIndex
    typedef Kit::Itc::RequestMessage<ReadRequest, ByEntryIndexPayload> RetrieveByEntryIndexMsg;

    /// Request: RetrieveByEntryIndex message
    virtual void request( RetrieveByEntryIndexMsg& msg ) = 0;

public:
    /// Virtual Destructor
    virtual ~ReadRequest() {}
};


/** This abstract class define ITC message type and payload for asynchronous
    response (to the application) of a RetrieveLatest message.

    The Application is responsible for implementing the response method(s).
 */
class ReadResponse
{
public:
    /// Response Message Type
    typedef Kit::Itc::ResponseMessage<ReadResponse,
                                      ReadRequest,
                                      ReadRequest::LatestPayload>
        RetrieveLatestMsg;

    /// Response
    virtual void response( RetrieveLatestMsg& msg ) = 0;

public:
    /// Response Message Type
    typedef Kit::Itc::ResponseMessage<ReadResponse,
                                      ReadRequest,
                                      ReadRequest::NextPayload>
        RetrieveNextMsg;

    /// Response
    virtual void response( RetrieveNextMsg& msg ) = 0;

public:
    /// Response Message Type
    typedef Kit::Itc::ResponseMessage<ReadResponse,
                                      ReadRequest,
                                      ReadRequest::PreviousPayload>
        RetrievePreviousMsg;

    /// Response
    virtual void response( RetrievePreviousMsg& msg ) = 0;

public:
    /// Response Message Type
    typedef Kit::Itc::ResponseMessage<ReadResponse,
                                      ReadRequest,
                                      ReadRequest::ByEntryIndexPayload>
        RetrieveByEntryIndexMsg;

public:
    /// Response
    virtual void response( RetrieveByEntryIndexMsg& msg ) = 0;


public:
    /// Virtual destructor
    virtual ~ReadResponse() {}
};


}  // end namespaces
}
}
}
#endif  // end header latch
