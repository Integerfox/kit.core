/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "ModelDatabase.h"
#include "IModelPoint.h"

//------------------------------------------------------------------------------
namespace Kit {
namespace Dm {


// global lock
static Kit::System::Mutex                                              globalMutex_;
StaticJsonDocument<OPTION_KIT_DM_MODEL_DATABASE_MAX_CAPACITY_JSON_DOC> IModelDatabase::g_doc_;
Kit::System::Mutex& IModelDatabase::getGlobalMutex_() noexcept
{
    return globalMutex_;
}

//////////////////////////////////////////////
ModelDatabase::ModelDatabase() noexcept
    : m_list()
{
}

ModelDatabase::ModelDatabase( const char* ignoreThisParameter_usedToCreateAUniqueConstructor ) noexcept
    : m_list( ignoreThisParameter_usedToCreateAUniqueConstructor )
{
}


//////////////////////////////////////////////

IModelPoint* ModelDatabase::lookupModelPoint( const char* modelPointName ) noexcept
{
    Kit::System::Mutex::ScopeLock criticalSection( m_lock );
    return find( modelPointName );
}

IModelPoint* ModelDatabase::getFirstByName() noexcept
{
    Kit::System::Mutex::ScopeLock criticalSection( m_lock );
    return m_list.first();
}

IModelPoint* ModelDatabase::getNextByName( IModelPoint& currentModelPoint ) noexcept
{
    Kit::System::Mutex::ScopeLock criticalSection( m_lock );
    IModelPoint*                  result = m_list.next( currentModelPoint );
    return result;
}

void ModelDatabase::insert_( IModelPoint& mpToAdd ) noexcept
{
    Kit::System::Mutex::ScopeLock criticalSection( m_lock );
    m_list.insert( mpToAdd );
}

Kit::System::Mutex& ModelDatabase::getMutex_() noexcept
{
    return m_lock;
}

bool ModelDatabase::fromJSON( const char* src, Kit::Text::IString* errorMsg, IModelPoint** retMp, uint16_t* retSequenceNumber ) noexcept
{
    // Get access to the Global JSON document
    Kit::System::Mutex::ScopeLock criticalSection( globalMutex_ );

    // Parse the JSON payload...
    DeserializationError err = deserializeJson( ModelDatabase::g_doc_, src );
    if ( err )
    {
        if ( errorMsg )
        {
            *errorMsg = err.c_str();
        }
        return false;
    }

    // Valid JSON... Parse the Model Point name
    const char* name = ModelDatabase::g_doc_["name"];
    if ( name == nullptr )
    {
        if ( errorMsg )
        {
            *errorMsg = "No valid 'name' key in the JSON input.";
        }
        return false;
    }

    // Look-up the Model Point name
    IModelPoint* mp = lookupModelPoint( name );
    if ( mp == 0 )
    {
        if ( errorMsg )
        {
            errorMsg->format( "Model Point name (%s) NOT found.", name );
        }
        return false;
    }

    // Attempt to parse the key/value pairs of interest
    JsonVariant                validElem  = ModelDatabase::g_doc_["valid"];
    JsonVariant                lockedElem = ModelDatabase::g_doc_["locked"];
    JsonVariant                valElem    = ModelDatabase::g_doc_["val"];
    uint16_t                   seqnum     = 0;
    IModelPoint::LockRequest_T lockAction = IModelPoint::eNO_REQUEST;
    bool                       parsed     = false;

    // Lock/Unlock the MP
    if ( !lockedElem.isNull() )
    {
        lockAction = lockedElem.as<bool>() ? IModelPoint::eLOCK : IModelPoint::eUNLOCK;
        seqnum     = mp->setLockState( lockAction );  // Note: This handles the use case of locking/unlock when the 'val' key/pair was not specified
        parsed     = true;
    }

    // Request to invalidate the MP
    if ( !validElem.isNull() && validElem.as<bool>() == false )
    {
        seqnum = mp->setInvalid( lockAction );
        parsed = true;
    }

    // Write a valid value to the MP
    else if ( valElem.isNull() == false )
    {
        if ( mp->fromJSON_( valElem, lockAction, seqnum, errorMsg ) == false )
        {
            return false;
        }
        parsed = true;
    }

    // Bad Syntax
    if ( !parsed )
    {
        if ( errorMsg )
        {
            *errorMsg = "JSON syntax is not valid or invalid payload semantics";
        }
        return false;
    }

    // Return the sequence number (when requested)
    if ( retSequenceNumber )
    {
        *retSequenceNumber = seqnum;
    }

    // Return the model point instance (when requested)
    if ( retMp )
    {
        *retMp = mp;
    }

    return true;
}


IModelPoint* ModelDatabase::find( const char* name ) noexcept
{
    IModelPoint* item = m_list.first();
    while ( item )
    {
        if ( strcmp( item->getName(), name ) == 0 )
        {
            return item;
        }
        item = m_list.next( *item );
    }

    return nullptr;
}

}  // end namespace
}
//------------------------------------------------------------------------------