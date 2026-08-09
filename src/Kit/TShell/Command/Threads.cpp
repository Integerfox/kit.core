/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Threads.h"

//------------------------------------------------------------------------------
namespace Kit {
namespace TShell {
namespace Command {


Result_T Threads::execute( IContext& context, char* cmdString ) noexcept
{
    // NOTE: The command IGNORES any/all command arguments!
    constexpr int ID_COLUMN_WIDTH = static_cast<int>( sizeof( void* ) * 2U  + 2 );
    
    // House keeping
    Kit::Text::IString& outtext = context.getOutputBuffer();
    m_contextPtr               = &context;
    m_count                    = 0;
    m_io                       = true;

    // Display list header
    m_io &= context.writeFrame( " " );
    outtext.format( "%-16s  %-1s  %-*s", "Name", "R", ID_COLUMN_WIDTH, "ID" );
    hookHeaderTitle( outtext );
    m_io &= context.writeFrame( outtext );

    // Display the threads
    Kit::System::Thread::traverse( *this );

    // Finished-up and exit
    outtext.format( "Total number of threads: %u", m_count );
    m_io &= context.writeFrame( " " );
    m_io &= context.writeFrame( outtext );
    return m_io ? Result_T::CMD_SUCCESS : Result_T::CMD_ERR_IO;
}


Kit::Type::TraverserStatus Threads::item( Kit::System::Thread& t ) noexcept
{
    constexpr int ID_COLUMN_WIDTH = static_cast<int>( sizeof( void* ) * 2U + 2);
    Kit::Text::IString& outtext = m_contextPtr->getOutputBuffer();

    m_count++;
    outtext.format( "%-16s  %-1s  %-*p", t.getName(), t.isActive() ? "Y" : "n", ID_COLUMN_WIDTH, reinterpret_cast<void*>( t.getId() ) );
    hookThreadEntry( outtext, t );
    m_io &= m_contextPtr->writeFrame( outtext );
    return Kit::Type::TraverserStatus::eCONTINUE;
}


/////////////////////////////////////////////////////////
void Threads::hookHeaderTitle( Kit::Text::IString& text )
{
    // Default is: No additional columns
}

void Threads::hookThreadEntry( Kit::Text::IString& text, Kit::System::Thread& currentThread )
{
    // Default is: No additional columns
}

}  // end namespace
}
}
//------------------------------------------------------------------------------