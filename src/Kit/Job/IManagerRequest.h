#ifndef KIT_JOB_IMANAGERREQUEST_H_
#define KIT_JOB_IMANAGERREQUEST_H_
/*-----------------------------------------------------------------------------
 * COPYRIGHT_HEADER_TO_BE_FILLED_LATER
 *----------------------------------------------------------------------------*/
/** @file */


#include "Kit/Itc/RequestMessage.h"
#include "Kit/Itc/ResponseMessage.h"
#include "Kit/Itc/SAP.h"
#include "Kit/Job/IJob.h"

///
namespace Kit {
///
namespace Job {


/** This abstract class define ITC message type and payload for the application
    to start a Job
 */
class IManagerRequest
{
public:
    /// SAP for this API
    typedef Kit::Itc::SAP<IManagerRequest> SAP;

public:
    /// Payload for Message: Start Job
    class StartJobPayload
    {
    public:
        /// IN: The name of the Job to run
        const char* jobName;

        /// IN: Optional 'command line' arguments for the Job
        char* jobArgs;

        /** OUT: results
            true  = Job was found and started
            false = no such Job
         */
        bool success;

        /// Constructor.
        StartJobPayload( const char* name, char* args )
            : jobName( name ), jobArgs( args ), success( false )
        {
        }
    };


    /// Message Type: Start
    typedef Kit::Itc::RequestMessage<IManagerRequest, StartJobPayload> StartJobMsg;

    /// Request: Start message
    virtual void request( StartJobMsg& msg ) = 0;


public:
    /// Payload for Message: Stop Job
    class StopJobPayload
    {
    public:
        /// IN: The name of the Job to stop
        const char* jobName;

        /** OUT: results
            true  = Job was stopped
            false = The job was not in the started state (or invalid job name)
         */
        bool success;

        /// Constructor.
        StopJobPayload( const char* name )
            : jobName( name ), success( false )
        {
        }
    };


    /// Message Type: StopJob
    typedef Kit::Itc::RequestMessage<IManagerRequest, StopJobPayload> StopJobMsg;

    /// Request: Stop message
    virtual void request( StopJobMsg& msg ) = 0;


public:
    /// Payload for Message: Stop ALL jobs
    class StopAllJobsPayload
    {
    public:
        /** OUT: results
            true  = At least on Job was stopped
            false = No Jobs where stopped, i.e. no active Jobs were running
         */
        bool success;

        /// Constructor.
        StopAllJobsPayload()
            : success( false )
        {
        }
    };


    /// Message Type: StopAllJobs
    typedef Kit::Itc::RequestMessage<IManagerRequest, StopAllJobsPayload> StopAllJobsMsg;

    /// Request: StopAllJobs message
    virtual void request( StopAllJobsMsg& msg ) = 0;

public:
    /// Payload for Message: Get Available Jobs
    class AvailJobsPayload
    {
    public:
        /// INPUT/OUTPUT: Memory to store the returned list
        Kit::Job::IJob** dstList;

        /// INPUT: Maximum number of elements that can be stored in 'dstList'.
        unsigned dstMaxElements;

        /** OUTPUT: Number of items returned. NOTE: If numElements > dstMaxElement,
                   the returned list is truncated.
         */
        unsigned numElements;

        /** OUT: results
            true  = All available jobs returned
            false = The returned list is truncated because of insufficent client memory
         */
        bool success;

        /// Constructor.
        AvailJobsPayload( Kit::Job::IJob** list, unsigned maxElems )
            : dstList( list ), dstMaxElements( maxElems ), numElements( 0 ), success( false ) {}
    };

    /// Message Type: GetAvailableJobs
    typedef Kit::Itc::RequestMessage<IManagerRequest, AvailJobsPayload> GetAvailableJobsMsg;

    /// Request: GetAvailableJobs message
    virtual void request( GetAvailableJobsMsg& msg ) = 0;

public:
    /// Payload for Message: Get Running Jobs
    class RunningJobsPayload
    {
    public:
        /// INPUT/OUTPUT: Memory to store the returned list
        Kit::Job::IJob** dstList;

        /// INPUT: Maximum number of elements that can be stored in 'dstList'.
        unsigned dstMaxElements;

        /** OUTPUT: Number of items returned. NOTE: If numElements > dstMaxElement,
                   the returned list is truncated.
         */
        unsigned numElements;

        /** OUT: results
            true  = All running jobs returned
            false = The returned list is truncated because of insufficent client memory
         */
        bool success;

        /// Constructor.
        RunningJobsPayload( Kit::Job::IJob** list, unsigned maxElems )
            : dstList( list ), dstMaxElements( maxElems ), numElements( 0 ), success( false ) {}
    };

    /// Message Type: GetRunningJobs
    typedef Kit::Itc::RequestMessage<IManagerRequest, RunningJobsPayload> GetRunningJobsMsg;

    /// Request: GetRunningJobs message
    virtual void request( GetRunningJobsMsg& msg ) = 0;

public:
    /// Payload for Message:
    class LookupJobPayload
    {
    public:
        /// INPUT: Name of the Job instance to lookup
        const char* name;

        /// OUTPUT: Found instance (or null if not found)
        IJob* foundInstance;

        /// Constructor.
        LookupJobPayload( const char* nameToLookup )
            : name( nameToLookup ), foundInstance( nullptr ) {}
    };

    /// Message Type: Lookup
    typedef Kit::Itc::RequestMessage<IManagerRequest, LookupJobPayload> LookupJobMsg;

    /// Request: Lookup message
    virtual void request( LookupJobMsg& msg ) = 0;


public:
    /// Virtual Destructor
    virtual ~IManagerRequest() {}
};


/** This abstract class define the ITC Response messages for the IManagerRequest
    interface.  Nominal usage of the IManager/IJob does NOT require Asynchronous
    ITC.  This class is provided for "completeness" and if there is ever a use
    case where asynchronous ITC is needed.
 */
class IManagerResponse
{
public:
    /// Response Message Type: Start
    typedef Kit::Itc::ResponseMessage<IManagerResponse,
                                      IManagerRequest,
                                      IManagerRequest::StartJobPayload>
        StartMsg;

    /// Response: StartMsg
    virtual void response( StartMsg& msg ) noexcept = 0;

public:
    /// Response Message Type: Stop
    typedef Kit::Itc::ResponseMessage<IManagerResponse,
                                      IManagerRequest,
                                      IManagerRequest::StopJobPayload>
        StopMsg;

    /// Response: StopMsg
    virtual void response( StopMsg& msg ) noexcept = 0;

public:
    /// Response Message Type: Stop All Jobs
    typedef Kit::Itc::ResponseMessage<IManagerResponse,
                                      IManagerRequest,
                                      IManagerRequest::StopAllJobsPayload>
        StopAllJobsMsg;

    /// Response: StopAllJobsMsg
    virtual void response( StopAllJobsMsg& msg ) noexcept = 0;

public:
    /// Response Message Type: GetAvailableJobs
    typedef Kit::Itc::ResponseMessage<IManagerResponse,
                                      IManagerRequest,
                                      IManagerRequest::AvailJobsPayload>
        GetAvailableJobsMsg;

    /// Response: GetAvailableJobsMsg
    virtual void response( GetAvailableJobsMsg& msg ) noexcept = 0;

public:
    /// Response Message Type: GetRunningJobs
    typedef Kit::Itc::ResponseMessage<IManagerResponse,
                                      IManagerRequest,
                                      IManagerRequest::RunningJobsPayload>
        GetRunningJobsMsg;

    /// Response: GetRunningJobsMsg
    virtual void response( GetRunningJobsMsg& msg ) noexcept = 0;

public:
    /// Response Message Type: Lookup
    typedef Kit::Itc::ResponseMessage<IManagerResponse,
                                      IManagerRequest,
                                      IManagerRequest::LookupJobPayload>
        LookupJobMsg;

    /// Response: LookupJobMsg
    virtual void response( LookupJobMsg& msg ) noexcept = 0;

public:
    /// Virtual destructor
    virtual ~IManagerResponse() = default;
};


}  // end namespaces
}
#endif  // end header latch