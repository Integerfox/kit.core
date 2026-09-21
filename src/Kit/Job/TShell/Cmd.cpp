/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Cmd.h"
#include "Kit/Text/Strip.h"
#include <string.h>

#define OVERFLOW_MSG "WARNING: Insufficient memory to list all Job instances"

#define SUBCMD_START "start"
#define SUBCMD_STOP  "stop"
#define SUBCMD_LIST  "ls"
#define SUBCMD_HELP  "help"

//------------------------------------------------------------------------------
namespace Kit {
namespace Job {
namespace TShell {


///////////////////////////
Kit::TShell::Result_T Cmd::execute( Kit::TShell::IContext& context, char* cmdString ) noexcept
{
    const char*         subCmd  = Kit::Text::Strip::space( Kit::Text::Strip::notSpace( cmdString ) );
    Kit::Text::IString& outtext = context.getOutputBuffer();
    bool                io      = true;

    // Display running Jobs
    if ( *subCmd == '\0' )
    {
        unsigned numFound = 0;
        bool     success  = m_jobManager.getRunningJobs( m_jobList, OPTION_KIT_JOB_TSHELL_MAX_INSTANCES, numFound );
        if ( numFound == 0 )
        {
            io &= context.writeFrame( "NO Jobs are running." );
            return io ? Kit::TShell::Result_T::CMD_SUCCESS : Kit::TShell::Result_T::CMD_ERR_IO;
        }

        io &= context.writeFrame( "RUNNING Jobs:" );
        for ( unsigned idx = 0; idx < numFound; idx++ )
        {
            io &= context.writeFrame( m_jobList[idx]->getName() );
        }

        if ( !success )
        {
            io &= context.writeFrame( OVERFLOW_MSG );
        }

        return io ? Kit::TShell::Result_T::CMD_SUCCESS : Kit::TShell::Result_T::CMD_ERR_IO;
    }

    // START command
    if ( strcmp( subCmd, SUBCMD_START ) == 0 )
    {
        // Get Job name
        const char* jobName = Kit::Text::Strip::space( Kit::Text::Strip::notSpace( subCmd ) );
        if ( *jobName == '\0' )
        {
            outtext.format( "ERROR: missing one or more arguments. [%s]", cmdString );
            context.writeFrame( outtext );
            return Kit::TShell::Result_T::CMD_ERR_BAD_SYNTAX;
        }

        // Attempt to start the Job
        // NOTE: The casting of the string pointers to non-const pointer is OKAY
        //       because the pointers are derived from 'cmdString' which is a char*
        char*       endName = const_cast<char*>( Kit::Text::Strip::notSpace( jobName ) );
        const char* jobArgs = Kit::Text::Strip::space( endName );
        *endName            = '\0';
        if ( !m_jobManager.startJob( jobName, const_cast<char*>( jobArgs ) ) )
        {
            return Kit::TShell::Result_T::CMD_ERR_BAD_SYNTAX;
        }

        return Kit::TShell::Result_T::CMD_SUCCESS;
    }

    // STOP command
    if ( strcmp( subCmd, SUBCMD_STOP ) == 0 )
    {
        // Get Job name
        const char* jobName = Kit::Text::Strip::space( Kit::Text::Strip::notSpace( subCmd ) );
        if ( *jobName == '\0' )
        {
            outtext.format( "ERROR: missing one or more arguments. [%s]", cmdString );
            context.writeFrame( outtext );
            return Kit::TShell::Result_T::CMD_ERR_BAD_SYNTAX;
        }

        // check for STOP ALL
        if ( strcmp( jobName, "ALL" ) == 0 )
        {
            m_jobManager.stopAllJobs();
        }

        // Attempt to stop the Job
        else
        {
            char* endName = const_cast<char*>( Kit::Text::Strip::notSpace( jobName ) );
            *endName      = '\0';
            if ( !m_jobManager.stopJob( jobName ) )
            {
                return Kit::TShell::Result_T::CMD_ERR_BAD_SYNTAX;
            }
        }

        return Kit::TShell::Result_T::CMD_SUCCESS;
    }

    // LIST command
    if ( strcmp( subCmd, SUBCMD_LIST ) == 0 )
    {
        unsigned numFound = 0;
        bool     success  = m_jobManager.getAvailableJobs( m_jobList, OPTION_KIT_JOB_TSHELL_MAX_INSTANCES, numFound );
        if ( numFound == 0 )
        {
            io &= context.writeFrame( "NO Jobs available." );
            return io ? Kit::TShell::Result_T::CMD_SUCCESS : Kit::TShell::Result_T::CMD_ERR_IO;
        }

        io &= context.writeFrame( "AVAILABLE Jobs:" );
        for ( unsigned idx = 0; idx < numFound; idx++ )
        {
            io &= context.writeFrame( m_jobList[idx]->getName() );
        }

        if ( !success )
        {
            io &= context.writeFrame( OVERFLOW_MSG );
        }

        return io ? Kit::TShell::Result_T::CMD_SUCCESS : Kit::TShell::Result_T::CMD_ERR_IO;
    }

    // HELP command
    if ( strcmp( subCmd, SUBCMD_HELP ) == 0 )
    {
        // Get Job name
        const char* jobName = Kit::Text::Strip::space( Kit::Text::Strip::notSpace( subCmd ) );
        if ( *jobName == '\0' )
        {
            outtext.format( "ERROR: Job name missing. [%s]", cmdString );
            context.writeFrame( outtext );
            return Kit::TShell::Result_T::CMD_ERR_BAD_SYNTAX;
        }

        // Look-up the Job and display its help information
        const Kit::Job::IJob* job = m_jobManager.lookUpJob( jobName );
        if ( !job )
        {
            return Kit::TShell::Result_T::CMD_ERR_BAD_SYNTAX;
        }

        io &= context.writeFrame( jobName );
        io &= context.writeFrame( job->getDescription() );
        io &= context.writeFrame( job->getUsage() );
        return io ? Kit::TShell::Result_T::CMD_SUCCESS : Kit::TShell::Result_T::CMD_ERR_IO;
    }

    // If I get here the command syntax/arguments are bad
    return Kit::TShell::Result_T::CMD_ERR_BAD_SYNTAX;
}

}  // end namespace
}
}
//------------------------------------------------------------------------------