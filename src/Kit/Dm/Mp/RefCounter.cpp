/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */


#include "RefCounter.h"
#include "Kit/Text/StringTo.h"
#include <cstdint>
#include <limits.h>


//------------------------------------------------------------------------------
namespace Kit {
namespace Dm {
namespace Mp {

///////////////////////////////////////////////////////////////////////////////
void RefCounter::updateAndCheckForChangeNotification( uint32_t newValue )
{
    // Generate change notices on transition to valid OR zero-to-not-zero OR not-zero-to-zero
    if ( !m_valid || ( m_data == 0 && newValue != 0 ) || ( newValue == 0 && m_data != 0 ) )
    {
        processDataUpdated();
    }
    m_data = newValue;
}


uint16_t RefCounter::increment( uint32_t incrementAmount, bool forceChangeNotification, LockRequest_T lockRequest ) noexcept
{
    Kit::System::Mutex::ScopeLock criticalSection( Kit::Dm::ModelPointBase::m_modelDatabase.getMutex_() );
    if ( testAndUpdateLock( lockRequest ) )
    {
        // Increment the counter and prevent overflow
        uint32_t newValue = m_data + incrementAmount;
        if ( newValue < m_data )
        {
            newValue = UINT32_MAX;
        }

        // Generate change notices on transition to valid OR zero-to-not-zero
        updateAndCheckForChangeNotification( newValue );
    }
    return m_seqNum;
}

uint16_t RefCounter::decrement( uint32_t decrementAmount, bool forceChangeNotification, LockRequest_T lockRequest ) noexcept
{
    Kit::System::Mutex::ScopeLock criticalSection( Kit::Dm::ModelPointBase::m_modelDatabase.getMutex_() );
    if ( testAndUpdateLock( lockRequest ) )
    {
        // Decrement the counter and prevent underflow
        uint32_t newValue = m_data - decrementAmount;
        if ( newValue > m_data )
        {
            newValue = 0;
        }

        // Generate change notices on transition to valid OR zero-to-not-zero
        updateAndCheckForChangeNotification( newValue );
    }
    return m_seqNum;
}

uint16_t RefCounter::reset( uint32_t newValue, bool forceChangeNotification, LockRequest_T lockRequest ) noexcept
{
    Kit::System::Mutex::ScopeLock criticalSection( Kit::Dm::ModelPointBase::m_modelDatabase.getMutex_() );
    if ( forceChangeNotification || testAndUpdateLock( lockRequest ) )
    {
        // Generate change notices on transition to valid OR zero-to-not-zero
        updateAndCheckForChangeNotification( newValue );
    }
    return m_seqNum;
}

///////////////////////////////////////////////////////////////////////////////
bool RefCounter::setJSONVal( JsonDocument& doc ) noexcept
{
    doc["val"] = m_data;
    return true;
}

bool RefCounter::fromJSON_( JsonVariant& src, LockRequest_T lockRequest, uint16_t& retSequenceNumber, Kit::Text::IString* errorMsg ) noexcept
{
    // Parse as a numeric -->i.e. 'set a explicit value'
    if ( src.is<uint32_t>() )
    {
        retSequenceNumber = reset( src.as<uint32_t>(), lockRequest );
        return true;
    }

    // Attempt to parse the 'action' -->MUST BE A STRING at this point!
    const char* jsonValue = src;
    if ( jsonValue == nullptr )
    {
        if ( errorMsg )
        {
            *errorMsg = "Missing 'val' key/value pair";
        }
        return false;
    }

    // Increment action
    if ( jsonValue[0] == '+' )
    {
        uint32_t numValue;
        if ( Kit::Text::StringTo::unsignedInt( numValue, jsonValue + 1 ) == false )
        {
            if ( errorMsg )
            {
                *errorMsg = "Invalid/bad syntax for the 'val' key/value pair";
            }
            return false;
        }
        retSequenceNumber = increment( (uint32_t)numValue, lockRequest );
        return true;
    }

    // Decrement action
    if ( jsonValue[0] == '-' )
    {
        uint32_t numValue;
        if ( Kit::Text::StringTo::unsignedInt( numValue, jsonValue + 1 ) == false )
        {
            if ( errorMsg )
            {
                *errorMsg = "Invalid/bad syntax for the 'val' key/value pair";
            }
            return false;
        }
        retSequenceNumber = decrement( (uint32_t)numValue, lockRequest );
        return true;
    }

    // Bad syntax
    if ( errorMsg )
    {
        *errorMsg = "Invalid/bad syntax for the 'val' key/value pair";
    }
    return false;
}


}  // end namespace
}
}
//------------------------------------------------------------------------------