/*-----------------------------------------------------------------------------
 * COPYRIGHT_HEADER_TO_BE_FILLED_LATER
 *----------------------------------------------------------------------------*/
/** @file */

#include "Manager.h"
#include "Kit/Itc/SyncReturnHandler.h"
#include "Kit/System/Trace.h"
#include <string.h>

//------------------------------------------------------------------------------
namespace Kit {
namespace Job {

static IJob* searchList( Kit::Container::OrderedList<IJob>& listToSearch,
                         const char*                        nameToFind )
{
    IJob* jobPtr = listToSearch.first();
    while ( jobPtr )
    {
        // Check if the command matches
        if ( strcmp( nameToFind, jobPtr->getName() ) == 0 )
        {
            return jobPtr;
        }
        // Get next job in the list
        jobPtr = listToSearch.next( *jobPtr );
    }

    // If I get here, then the command was not found
    return nullptr;
}

////////////
void Manager::request( Kit::Itc::IOpenRequest::OpenMsg& msg ) noexcept
{
    if ( !m_opened )
    {
        m_opened = true;

        // Initialize the Job instances
        IJob* itemPtr = m_jobs.first();
        while ( itemPtr )
        {
            itemPtr->initialize_();
            itemPtr = m_jobs.next( *itemPtr );
        }
    }

    // The open request never fails
    msg.getPayload().success = true;
    msg.returnToSender();
}

void Manager::request( Kit::Itc::ICloseRequest::CloseMsg& msg ) noexcept
{
    if ( m_opened )
    {
        m_opened = false;

        // Stop any running Jobs, then shutdown all of the Jobs
        IJob* itemPtr = m_jobs.first();
        while ( itemPtr )
        {
            if ( itemPtr->isRunning_() )
            {
                itemPtr->stop_();
            }
            itemPtr->shutdown_();
            itemPtr = m_jobs.next( *itemPtr );
        }
    }

    // The close request never fails
    msg.getPayload().success = true;
    msg.returnToSender();
}


/////////////////////
void Manager::request( IManagerRequest::StartJobMsg& msg ) noexcept
{
    IManagerRequest::StartJobPayload& payload = msg.getPayload();
    payload.success                           = false;

    if ( payload.jobName == nullptr || *payload.jobName == '\0' )
    {
        KIT_SYSTEM_TRACE_MSG( OPTION_KIT_JOB_TRACE_SECTION, "FAILED to start: Job name is empty" );
    }
    else
    {
        // Look-up the Job by name
        IJob* job = searchList( m_jobs, payload.jobName );
        if ( job != nullptr )
        {
            // Stop the Job if it is already running
            if ( job->isRunning_() )
            {
                job->stop_();
            }

            // Start the Job
            char* args = payload.jobArgs;
            if ( args == nullptr )
            {
                KIT_SYSTEM_TRACE_MSG( OPTION_KIT_JOB_TRACE_SECTION, "FAILED to start: %s due to nullptr for 'args'", job->getName() );
            }
            else
            {
                KIT_SYSTEM_TRACE_MSG( OPTION_KIT_JOB_TRACE_SECTION, "Starting: %s", job->getName() );
                if ( job->start_( *this, args ) )
                {
                    payload.success = true;
                }
                else
                {
                    KIT_SYSTEM_TRACE_MSG( OPTION_KIT_JOB_TRACE_SECTION, "FAILED to start: %s %s", job->getName(), args );
                }
            }
        }
        else
        {
            KIT_SYSTEM_TRACE_MSG( OPTION_KIT_JOB_TRACE_SECTION, "Job not found: %s", payload.jobName );
        }
    }

    msg.returnToSender();
}

void Manager::request( IManagerRequest::StopJobMsg& msg ) noexcept
{
    IManagerRequest::StopJobPayload& payload = msg.getPayload();
    payload.success                          = false;

    // Look-up the Job by name in the 'running' list
    IJob* job = searchList( m_jobs, payload.jobName );
    if ( job != nullptr )
    {
        if ( job->isRunning_() )
        {
            KIT_SYSTEM_TRACE_MSG( OPTION_KIT_JOB_TRACE_SECTION, "Stopping: %s", job->getName() );
            job->stop_();
            payload.success = true;
        }
        else
        {
            KIT_SYSTEM_TRACE_MSG( OPTION_KIT_JOB_TRACE_SECTION, "Job is NOT running: %s", job->getName() );
        }
    }
    else
    {
        KIT_SYSTEM_TRACE_MSG( OPTION_KIT_JOB_TRACE_SECTION, "Job not found: %s", payload.jobName );
    }

    msg.returnToSender();
}
void Manager::request( IManagerRequest::StopAllJobsMsg& msg ) noexcept
{
    // Walk the running list
    msg.getPayload().success = false;  // Set result to NO active jobs
    IJob* item               = m_jobs.first();
    while ( item )
    {
        // Stop the instance
        if ( item->isRunning_() )
        {
            KIT_SYSTEM_TRACE_MSG( OPTION_KIT_JOB_TRACE_SECTION, "Stopping: %s", item->getName() );
            item->stop_();
            msg.getPayload().success = true;
        }

        // Get the next item
        item = m_jobs.next( *item );
    }

    msg.returnToSender();
}

void Manager::request( IManagerRequest::JobRunningMsg& msg ) noexcept
{
    IManagerRequest::JobRunningPayload& payload = msg.getPayload();
    payload.running                             = false;

    // Look-up the Job by name in the 'running' list
    IJob* job = searchList( m_jobs, payload.jobName );
    if ( job != nullptr )
    {
        payload.running = job->isRunning_();
    }

    msg.returnToSender();
}

void Manager::request( IManagerRequest::GetAvailableJobsMsg& msg ) noexcept
{
    IManagerRequest::AvailJobsPayload& payload = msg.getPayload();
    payload.success                            = false;  // Set result to: list is Truncated
    unsigned idx                               = 0;
    unsigned maxElems                          = payload.dstMaxElements;

    // Walk the inactive list
    IJob* item = m_jobs.first();
    while ( item )
    {
        if ( maxElems == 0 )
        {
            break;
        }
        payload.dstList[idx] = item;
        idx++;
        maxElems--;
        payload.numElements++;

        item = m_jobs.next( *item );
    }

    // All jobs have been added to the list
    if ( item == nullptr )
    {
        payload.success = true;
    }

    msg.returnToSender();
}

void Manager::request( IManagerRequest::GetRunningJobsMsg& msg ) noexcept
{
    IManagerRequest::RunningJobsPayload& payload = msg.getPayload();
    payload.success                              = false;  // Set result to: list is Truncated
    unsigned idx                                 = 0;
    unsigned maxElems                            = payload.dstMaxElements;

    // Walk the running list
    IJob* item = m_jobs.first();
    while ( item )
    {
        if ( item->isRunning_() )
        {
            if ( maxElems == 0 )
            {
                break;
            }
            payload.dstList[idx] = item;
            idx++;
            maxElems--;
            payload.numElements++;
        }

        item = m_jobs.next( *item );
    }

    // All jobs have been added to the list
    if ( item == nullptr )
    {
        payload.success = true;
    }

    msg.returnToSender();
}

void Manager::request( IManagerRequest::LookupJobMsg& msg ) noexcept
{
    IManagerRequest::LookupJobPayload& payload = msg.getPayload();

    // Search the list
    IJob* job             = searchList( m_jobs, payload.name );
    payload.foundInstance = job;

    msg.returnToSender();
}

////////////////
bool Manager::startJob( const char* jobName, char* optionalArgs ) noexcept
{
    if ( jobName == nullptr || *jobName == '\0' || !m_opened )
    {
        return false;
    }

    IManagerRequest::StartJobPayload payload( jobName, optionalArgs );
    Kit::Itc::SyncReturnHandler      srh;
    IManagerRequest::StartJobMsg     msg( *this, payload, srh );
    m_eventQueue.postSync( msg );

    return payload.success;
}

bool Manager::stopJob( const char* jobName ) noexcept
{
    IManagerRequest::StopJobPayload payload( jobName );
    Kit::Itc::SyncReturnHandler     srh;
    IManagerRequest::StopJobMsg     msg( *this, payload, srh );
    m_eventQueue.postSync( msg );

    return payload.success;
}

bool Manager::stopAllJobs() noexcept
{
    IManagerRequest::StopAllJobsPayload payload;
    Kit::Itc::SyncReturnHandler         srh;
    IManagerRequest::StopAllJobsMsg     msg( *this, payload, srh );
    m_eventQueue.postSync( msg );

    return payload.success;
}

bool Manager::isJobRunning( const char* jobName ) noexcept
{
    IManagerRequest::JobRunningPayload payload( jobName );
    Kit::Itc::SyncReturnHandler        srh;
    IManagerRequest::JobRunningMsg     msg( *this, payload, srh );
    m_eventQueue.postSync( msg );

    return payload.running;
}

bool Manager::getAvailableJobs( Kit::Job::IJob* dstList[], unsigned dstMaxElements, unsigned& numElemsFound ) noexcept
{
    IManagerRequest::AvailJobsPayload    payload( dstList, dstMaxElements );
    Kit::Itc::SyncReturnHandler          srh;
    IManagerRequest::GetAvailableJobsMsg msg( *this, payload, srh );
    m_eventQueue.postSync( msg );

    numElemsFound = payload.numElements;
    return payload.success;
}
bool Manager::getRunningJobs( Kit::Job::IJob* dstList[], unsigned dstMaxElements, unsigned& numElemsFound ) noexcept
{
    IManagerRequest::RunningJobsPayload payload( dstList, dstMaxElements );
    Kit::Itc::SyncReturnHandler         srh;
    IManagerRequest::GetRunningJobsMsg  msg( *this, payload, srh );
    m_eventQueue.postSync( msg );

    numElemsFound = payload.numElements;
    return payload.success;
}

Kit::Job::IJob* Manager::lookUpJob( const char* jobName ) noexcept
{
    IManagerRequest::LookupJobPayload payload( jobName );
    Kit::Itc::SyncReturnHandler       srh;
    IManagerRequest::LookupJobMsg     msg( *this, payload, srh );
    m_eventQueue.postSync( msg );

    return payload.foundInstance;
}

//////////////////////////////
Kit::EventQueue::IQueue& Manager::getEventQueue() noexcept
{
    return m_eventQueue;
}

Kit::Text::IString& Manager::getWorkBuffer0() noexcept
{
    return m_workBuffer0;
}

Kit::Text::IString& Manager::getWorkBuffer1() noexcept
{
    return m_workBuffer1;
}

}  // end namespace
}
//------------------------------------------------------------------------------