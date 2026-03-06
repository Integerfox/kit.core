#ifndef KIT_DM_MP_SCALAR_H_
#define KIT_DM_MP_SCALAR_H_
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
#include <stdint.h>

///
namespace Kit {
///
namespace Dm {
///
namespace Mp {


/** This template class is partial concrete class that provides a basic
    implementation for a Model Point a single element of type: 'ELEMTYPE'.

    NOTE: ELEMTYPE must support the assignment operator
 */
template <class ELEMTYPE, class MPTYPE>
class Scalar : public Kit::Dm::ModelPointBase
{
protected:
    /// The element's value
    ELEMTYPE m_data;

protected:
    /// Constructor: Invalid MP
    Scalar( Kit::Dm::IModelDatabase& myModelBase, const char* symbolicName )
        : Kit::Dm::ModelPointBase( myModelBase, symbolicName, &m_data, sizeof( m_data ), false )
    {
    }

    /// Constructor: Valid MP (requires initial value)
    Scalar( Kit::Dm::IModelDatabase& myModelBase, const char* symbolicName, const ELEMTYPE& initialValue )
        : Kit::Dm::ModelPointBase( myModelBase, symbolicName, &m_data, sizeof( m_data ), true )
    {
        m_data = initialValue;
    }

public:
    /// Type safe read. See Kit::Dm::IModelPoint
    inline bool read( ELEMTYPE& dstData, uint16_t* seqNumPtr = nullptr ) const noexcept
    {
        return Kit::Dm::ModelPointBase::readData( &dstData, sizeof( ELEMTYPE ), seqNumPtr );
    }

    /// Type safe write. See Kit::Dm::IModelPoint
    inline uint16_t write( const ELEMTYPE&                     newValue,
                           bool                                forceChgNotification = false,
                           Kit::Dm::IModelPoint::LockRequest_T lockRequest          = Kit::Dm::IModelPoint::eNO_REQUEST ) noexcept
    {
        return Kit::Dm::ModelPointBase::writeData( &newValue, sizeof( ELEMTYPE ), forceChgNotification, lockRequest );
    }

    /// Updates the MP with the valid-state/data from 'src'. Note: the src.lock state is NOT copied
    inline uint16_t copyFrom( const MPTYPE& src,
                              LockRequest_T lockRequest = Kit::Dm::IModelPoint::eNO_REQUEST ) noexcept
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
    inline bool readAndSync( ELEMTYPE& dstData, IObserver& observerToSync )
    {
        uint16_t seqNum;
        return ModelPointBase::readAndSync( &dstData, sizeof( ELEMTYPE ), seqNum, observerToSync );
    }

    /// See Kit::Dm::ModelPointCommon
    inline bool readAndSync( ELEMTYPE&  dstData,
                             uint16_t&  seqNum,
                             IObserver& observerToSync )
    {
        return ModelPointBase::readAndSync( &dstData, sizeof( ELEMTYPE ), seqNum, observerToSync );
    }
};

}  // end namespaces
}
}
#endif  // end header latch
