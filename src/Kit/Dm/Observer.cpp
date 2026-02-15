/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */


#include "Observer.h"
#include "Kit/System/FatalError.h"

//------------------------------------------------------------------------------
namespace Kit {
namespace Dm {

/////////////////////////////
ObserverBase::ObserverBase( Kit::EventQueue::IChangeNotification& myEventQueue ) noexcept
    : m_point( nullptr )
    , m_eventQueueHdl( &myEventQueue )
    , m_state( 0 )
    , m_seqNumber( IModelPoint::SEQUENCE_NUMBER_UNKNOWN )
{
}

ObserverBase::ObserverBase() noexcept
    : m_point( nullptr )
    , m_eventQueueHdl( nullptr )
    , m_state( 0 )
    , m_seqNumber( IModelPoint::SEQUENCE_NUMBER_UNKNOWN )
{
}

void ObserverBase::setEventQueue( Kit::EventQueue::IChangeNotification& myEventQueue )
{
    m_eventQueueHdl = &myEventQueue;
}

Kit::EventQueue::IChangeNotification* ObserverBase::getIChangeNotification_() const noexcept
{
     if ( m_eventQueueHdl == nullptr )
    {
        Kit::System::FatalError::logf( Kit::System::Shutdown::eDATA_MODEL,
                                       "Kit::Dm::Observer().  Protocol Error.  getIChangeNotification_() was called before setEventQueue()" );
        return nullptr;
    }

    return m_eventQueueHdl;
}

void ObserverBase::setModelPoint_( IModelPoint* modelPoint ) noexcept
{
    m_point = modelPoint;
}

int ObserverBase::getState_() const noexcept
{
    return m_state;
}

void ObserverBase::setState_( int newState ) noexcept
{
    m_state = newState;
}

uint16_t ObserverBase::getSequenceNumber_() const noexcept
{
    return m_seqNumber;
}

void ObserverBase::setSequenceNumber_( uint16_t newSeqNumber ) noexcept
{
    m_seqNumber = newSeqNumber;
}

IModelPoint* ObserverBase::getModelPoint_() noexcept
{
    if ( m_point == nullptr )
    {
        Kit::System::FatalError::logf( Kit::System::Shutdown::eDATA_MODEL,
                                       "Kit::Dm::Observer().  Protocol Error.  getModelPoint_() was called before setModelPoint_()" );
        return nullptr;
    }

    return m_point;
}

}  // end namespace
}
//------------------------------------------------------------------------------