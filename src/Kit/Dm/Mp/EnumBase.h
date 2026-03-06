#ifndef KIT_DM_MP_ENUM_BASE_H_
#define KIT_DM_MP_ENUM_BASE_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */


#include "Kit/Dm/ModelPointBase.h"
#include "Kit/Dm/Observer.h"

///
namespace Kit {
///
namespace Dm {
///
namespace Mp {

/** This template class provides a mostly concrete implementation for a Model
    Point who's data is a BETTER_ENUM type. The child classes must provide the
    following:

        getTypeAsText()
 */
template <class BETTERENUM_TYPE, class MPTYPE>
class EnumBase : public Kit::Dm::ModelPointBase
{
protected:
    /// The element's value
    BETTERENUM_TYPE m_data;

public:
    /// Constructor: Invalid MP
    EnumBase( Kit::Dm::IModelDatabase& myModelBase, const char* symbolicName )
        : Kit::Dm::ModelPointBase( myModelBase, symbolicName, &m_data, sizeof( m_data ), false )
        , m_data( BETTERENUM_TYPE::_from_index_unchecked( 0 ) )
    {
    }

    /// Constructor: Valid MP (requires initial value)
    EnumBase( Kit::Dm::IModelDatabase& myModelBase, const char* symbolicName, BETTERENUM_TYPE initialValue )
        : Kit::Dm::ModelPointBase( myModelBase, symbolicName, &m_data, sizeof( m_data ), true )
        , m_data( BETTERENUM_TYPE::_from_index_unchecked( 0 ) )
    {
        m_data = initialValue;
    }

public:
    /// Type safe read. See Kit::Dm::IModelPoint
    inline bool read( BETTERENUM_TYPE& dstData, uint16_t* seqNumPtr = 0 ) const noexcept
    {
        return readData( &dstData, sizeof( BETTERENUM_TYPE ), seqNumPtr );
    }

    /// Type safe write. See Kit::Dm::IModelPoint
    inline uint16_t write( BETTERENUM_TYPE                     newValue,
                           bool                                forceChangeNotification = false,
                           Kit::Dm::IModelPoint::LockRequest_T lockRequest             = Kit::Dm::IModelPoint::eNO_REQUEST ) noexcept
    {
        return writeData( &newValue, sizeof( BETTERENUM_TYPE ), forceChangeNotification, lockRequest );
    }

    /// Updates the MP with the valid-state/data from 'src'. Note: the src.lock state is NOT copied
    inline uint16_t copyFrom( const MPTYPE&                       src,
                              Kit::Dm::IModelPoint::LockRequest_T lockRequest = Kit::Dm::IModelPoint::eNO_REQUEST ) noexcept
    {
        return copyDataAndStateFrom( src, lockRequest );
    }

    /// Type safe register observer
    inline void attach( Kit::Dm::Observer<MPTYPE>& observer,
                        uint16_t                   initialSeqNumber = SEQUENCE_NUMBER_UNKNOWN ) noexcept
    {
        attachObserver( observer, initialSeqNumber );
    }

    /// Type safe un-register observer
    inline void detach( Kit::Dm::Observer<MPTYPE>& observer ) noexcept
    {
        detachObserver( observer );
    }

    /// See Kit::Dm::ModelPointCommon
    inline bool readAndSync( BETTERENUM_TYPE& dstData, IObserver& observerToSync )
    {
        uint16_t seqNum;
        return ModelPointBase::readAndSync( &dstData, sizeof( BETTERENUM_TYPE ), seqNum, observerToSync );
    }

    /// See Kit::Dm::ModelPointCommon
    inline bool readAndSync( BETTERENUM_TYPE& dstData, uint16_t& seqNum, IObserver& observerToSync )
    {
        return ModelPointBase::readAndSync( &dstData, sizeof( BETTERENUM_TYPE ), seqNum, observerToSync );
    }

protected:
    /// See Kit::Dm::Point.
    bool setJSONVal( JsonDocument& doc ) noexcept
    {
        doc["val"] = (char*) m_data._to_string();
        return true;
    }

public:
    /// See Kit::Dm::Point.
    bool fromJSON_( JsonVariant&                        src,
                    Kit::Dm::IModelPoint::LockRequest_T lockRequest,
                    uint16_t&                           retSequenceNumber,
                    Kit::Text::IString*                 errorMsg ) noexcept
    {
        // Get the enum string
        const char* newValue = src;
        if ( newValue == nullptr )
        {
            if ( errorMsg )
            {
                *errorMsg = "Invalid syntax for the 'val' key/value pair";
            }
            return false;
        }

        // Convert the text to an enum value
        auto maybeValue = BETTERENUM_TYPE::_from_string_nothrow( newValue );
        if ( !maybeValue )
        {
            if ( errorMsg )
            {
                errorMsg->format( "Invalid enum value (%s)", newValue );
            }
            return false;
        }

        retSequenceNumber = write( *maybeValue, lockRequest );
        return true;
    }
};


}  // end namespaces
}
}
#endif  // end header latch
