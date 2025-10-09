/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Mailbox.h"
#include "Kit/System/Thread.h"
#include "Kit/System/GlobalLock.h"
#include "Kit/System/SimTick.h"

#include <type_traits>

/// Compile time check for the EventQueue being configured 'correctly'
static_assert( std::is_base_of<Kit::EventQueue::IMsgNotification, Kit::EventQueue::IQueue>::value,
               "IQueue must inherit from IMsgNotification" );

//------------------------------------------------------------------------------
namespace Kit {
namespace Itc {

////////////////////////////////////////////////////////////////////////////////
Mailbox::Mailbox( Kit::System::ISignable& myEventLoop ) noexcept
    : m_eventLoop( myEventLoop )
{
}

void Mailbox::post( IMessage& msg ) noexcept
{
    // Update my internal FIFO
    Kit::System::GlobalLock::begin();
    put( msg );
    Kit::System::GlobalLock::end();

    // Wake up my event loop to process the message
    m_eventLoop.signal();
}

void Mailbox::postSync( IMessage& msg ) noexcept
{
    post( msg );
    KIT_SYSTEM_SIM_TICK_APPLICATION_WAIT();
    Kit::System::Thread::wait();
}

void Mailbox::processMessages() noexcept
{
    // Get the next message
    Kit::System::GlobalLock::begin();
    IMessage* msgPtr = get();
    Kit::System::GlobalLock::end();

    // Dispatch at MOST one message
    if ( msgPtr )
    {
        msgPtr->process();
    }
}

bool Mailbox::isPendingMessage() noexcept
{
    // Get the next message
    Kit::System::GlobalLock::begin();
    IMessage* msgPtr = first();
    Kit::System::GlobalLock::end();
    return msgPtr != nullptr;
}

}  // end namespace
}
//------------------------------------------------------------------------------