#ifndef KIT_ITC_SAP_H_
#define KIT_ITC_SAP_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/EventQueue/IQueue.h"
#include <type_traits>


/// Compile time check for the EventQueue being configured 'correctly'
static_assert( std::is_base_of<Kit::EventQueue::IMsgNotification, Kit::EventQueue::IQueue>::value,
               "IQueue must inherit from IMsgNotification" );


///
namespace Kit {
///
namespace Itc {

/** This concrete template class represents the interface to a ITC Service
    Access Point (SAP). A SAP 'binds' a Request API with a Event Queue interface
    (aka a mailbox). In effect a Service is the 'fully-qualified' identifier for
    posting a message to a specific Request API.
 */
template <class SERVICE>
class SAP : public Kit::EventQueue::IMsgNotification
{
public:
    /** This constructor requires a reference to the
        request API and its associated mailbox/post-message
        API.
     */
    SAP( SERVICE& api, Kit::EventQueue::IQueue& eventQueue ) noexcept
        : m_api( api ), m_eventQueue( eventQueue )
    {
    }

public:
    /// Returns a reference to the associated Kit::Itc::Request instance
    SERVICE& getService() noexcept
    {
        return m_api;
    }


public:
    /// See Kit::EventQueue::IMsgNotification
    void post( IMessage& msg ) noexcept override
    {
        m_eventQueue.post( msg );
    }

    /// See Kit::EventQueue::IMsgNotification
    void postSync( IMessage& msg ) noexcept override
    {
        m_eventQueue.postSync( msg );
    }

protected:
    /// Reference to the instance that implements the request service
    SERVICE& m_api;

    /// Reference to the associated post-message/mailbox interface
    Kit::EventQueue::IQueue& m_eventQueue;
};


}  // end namespaces
}
#endif  // end header latch
