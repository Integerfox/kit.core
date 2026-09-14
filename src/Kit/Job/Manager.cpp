/*-----------------------------------------------------------------------------
 * COPYRIGHT_HEADER_TO_BE_FILLED_LATER
 *----------------------------------------------------------------------------*/
/** @file */

#include "Manager.h"
#include "Kit/Itc/SyncReturnHandler.h"
#include "Kit/System/Trace.h"

//------------------------------------------------------------------------------
namespace Kit {
namespace Job {

static IJob* searchList( Kit::Container::SList<IJob>& listToSearch,
                         const char*                  nameToFind )
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
        IJob* itemPtr = m_inactiveJobs.first();
        while ( itemPtr )
        {
            itemPtr->initialize_();
            itemPtr = m_inactiveJobs.next( *itemPtr );
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

        // Shutdown my inactive Jobs
        IJob* itemPtr = m_inactiveJobs.first();
        while ( itemPtr )
        {
            itemPtr->shutdown_();
            itemPtr = m_inactiveJobs.next( *itemPtr );
        }

        // Shutdown my started Jobs (and return the instance to the inactive list (for the use case of restarting the Manager)
        itemPtr = m_startedJobs.getFirst();
        while ( itemPtr )
        {
            itemPtr->stop_();
            itemPtr->shutdown_();

            // Return the instance to the inactive list
            m_inactiveJobs.put( *itemPtr );

            // Get the next item
            itemPtr = m_startedJobs.getFirst();
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

    // Look-up the Job by name
    IJob* job = searchList( m_inactiveJobs, payload.jobName );
    if ( job != nullptr )
    {
        payload.success = true;

        // Put the Job instance into the started list
        m_inactiveJobs.remove( *job );
        m_startedJobs.put( *job );

        // Start the Job
        char* args = payload.jobArgs;
        if ( args == nullptr )
        {
            KIT_SYSTEM_TRACE_MSG( OPTION_KIT_JOB_TRACE_SECTION, "FAILED to start: %s due to nullptr for 'args'", job->getName() );
            payload.success = false;

            // Restore the Job to the inactive list
            m_startedJobs.remove( *job );
            m_inactiveJobs.put( *job );
        }
        else
        {
            KIT_SYSTEM_TRACE_MSG( OPTION_KIT_JOB_TRACE_SECTION, "Starting: %s", job->getName() );
            if ( !job->start_( *this, args ) )
            {
                KIT_SYSTEM_TRACE_MSG( OPTION_KIT_JOB_TRACE_SECTION, "FAILED to start: %s %s", job->getName(), args );
                payload.success = false;

                // Restore the Job to the inactive list
                m_startedJobs.remove( *job );
                m_inactiveJobs.put( *job );
            }
        }
    }

    msg.returnToSender();
}

void Manager::request( IManagerRequest::StopJobMsg& msg ) noexcept
{
    IManagerRequest::StopJobPayload& payload = msg.getPayload();
    payload.success                          = false;

    // Look-up the Job by name in the 'running' list
    IJob* job = searchList( m_startedJobs, payload.jobName );
    if ( job != nullptr )
    {
        KIT_SYSTEM_TRACE_MSG( OPTION_KIT_JOB_TRACE_SECTION, "Stopping: %s", job->getName() );
        job->stop_();

        // Return the instance to the inactive list
        m_startedJobs.remove( *job );
        m_inactiveJobs.put( *job );

        payload.success = true;
    }

    msg.returnToSender();
}
void Manager::request( IManagerRequest::StopAllJobsMsg& msg ) noexcept
{
    // Walk the running list
    msg.getPayload().success = false;  // Set result to NO active jobs
    IJob* item               = m_startedJobs.getFirst();
    while ( item )
    {
        // Stop the instance
        KIT_SYSTEM_TRACE_MSG( OPTION_KIT_JOB_TRACE_SECTION, "Stopping: %s", item->getName() );
        item->stop_();

        // Move the stop instance to the inactive list
        m_inactiveJobs.put( *item );
        msg.getPayload().success = true;

        // Get the next item
        item = m_startedJobs.getFirst();
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
    IJob* item = m_inactiveJobs.first();
    while ( item && maxElems )
    {
        payload.dstList[idx] = item;
        idx++;
        maxElems--;
        payload.numElements++;

        item = m_inactiveJobs.next( *item );
    }

    // Still room left in the Client's list
    if ( item == nullptr )
    {
        // Walk the running list
        item = m_startedJobs.first();
        while ( item && maxElems )
        {
            payload.dstList[idx] = item;
            idx++;
            maxElems--;
            payload.numElements++;

            item = m_startedJobs.next( *item );
        }

        // All jobs have been added to the list
        if ( item == nullptr )
        {
            payload.success = true;
        }
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
    IJob* item = m_startedJobs.first();
    while ( item && maxElems )
    {
        payload.dstList[idx] = item;
        idx++;
        maxElems--;
        payload.numElements++;

        item = m_startedJobs.next( *item );
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

    // Search both lists
    IJob* job = searchList( m_inactiveJobs, payload.name );
    if ( job == nullptr )
    {
        job = searchList( m_startedJobs, payload.name );
    }
    payload.foundInstance = job;

    msg.returnToSender();
}

////////////////
bool Manager::startJob( const char* jobName, char* optionalArgs ) noexcept
{
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

void Manager::stopAllJobs() noexcept
{
    IManagerRequest::StopAllJobsPayload payload;
    Kit::Itc::SyncReturnHandler         srh;
    IManagerRequest::StopAllJobsMsg     msg( *this, payload, srh );
    m_eventQueue.postSync( msg );
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
Kit::Text::IString& Manager::getWorkBuffer0() noexcept
{
    return m_workBuffer0;
}

Kit::Text::IString& Manager::getWorkBuffer1() noexcept
{
    return m_workBuffer1;
}

bool Manager::completed( IJob& jobThatCompleted ) noexcept
{
    // Look-up the Job by name in the 'running' list
    IJob* job = searchList( m_startedJobs, jobThatCompleted.getName() );
    if ( job != nullptr )
    {
        // Return the instance to the inactive list
        m_startedJobs.remove( *job );
        m_inactiveJobs.put( *job );

        return true;
    }

    return false;
}

}  // end namespace
}
//------------------------------------------------------------------------------