#ifndef KIT_DM_MP_PSEUDO_MP_BASE_H_
#define KIT_DM_MP_PSEUDO_MP_BASE_H_
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


/** This template class provides the common infrastructure for 'pseudo' Model
    Points, i.e. a Model Point that provides a role-specific view (e.g. the
    RunRequestor and RunManager classes) of a 'data' Model Point.

    A pseudo Model Point is registered in the Model Database under its own name
    (so that it can be accessed via JSON), but it has no data of its own.  Its
    state - validity, sequence number, locked state, data size (getSize()),
    and toJSON() output - is that of the data Model Point.  Only the pseudo
    Model Point's public 'writer' methods can modify the data Model Point.  As
    such, the following generic operations are NOT supported (i.e. they do
    nothing): setInvalid(), setLockState(), touch(), exportData(), and
    importData().  In addition, setJSONVal() is not supported (it always returns
    false), but toJSON() is delegated to the data Model Point.

    Template Arguments:
        ELEMTYPE  - The type of the data Model Point's data
        MPTYPE    - The concrete data Model Point type.  The type must provide
                    the type safe read(), attach(), and detach() methods
                    (e.g. be derived from Kit::Dm::Mp::Scalar)

    IMPORTANT: A pseudo Model Point is transient/run-time state and MUST NEVER
               be persistently stored.

    NOTE: All methods in this class ARE thread Safe unless explicitly
          documented otherwise.
 */
template <class ELEMTYPE, class MPTYPE>
class PseudoMpBase : public Kit::Dm::ModelPointBase
{
protected:
    /// Constructor.  The data Model Point must outlive the instance
    PseudoMpBase( Kit::Dm::IModelDatabase& myModelBase, const char* symbolicName, MPTYPE& dataMp )
        : Kit::Dm::ModelPointBase( myModelBase, symbolicName, &m_unused, sizeof( m_unused ), false )
        , m_dataMp( dataMp )
        , m_unused( 0 )
    {
    }

public:
    /// Type safe read of the data Model Point. See Kit::Dm::IModelPoint
    inline bool read( ELEMTYPE& dstData, uint16_t* seqNumPtr = nullptr ) const noexcept
    {
        return m_dataMp.read( dstData, seqNumPtr );
    }

    /// Type safe register observer (of the data Model Point)
    inline void attach( Kit::Dm::Observer<MPTYPE>& observer,
                        uint16_t                   initialSeqNumber = SEQUENCE_NUMBER_UNKNOWN ) noexcept
    {
        m_dataMp.attach( observer, initialSeqNumber );
    }

    /// Type safe un-register observer (of the data Model Point)
    inline void detach( Kit::Dm::Observer<MPTYPE>& observer ) noexcept
    {
        m_dataMp.detach( observer );
    }

public:
    /// See Kit::Dm::IModelPoint. Delegates to the data Model Point
    uint16_t getSequenceNumber() const noexcept override
    {
        return m_dataMp.getSequenceNumber();
    }

    /// See Kit::Dm::IModelPoint. Delegates to the data Model Point
    bool isNotValid( uint16_t* seqNumPtr = nullptr ) const noexcept override
    {
        return m_dataMp.isNotValid( seqNumPtr );
    }

    /// See Kit::Dm::IModelPoint. Delegates to the data Model Point
    bool isLocked() const noexcept override
    {
        return m_dataMp.isLocked();
    }

    /// See Kit::Dm::IModelPoint. Delegates to the data Model Point, i.e. returns the size of the data Model Point's data
    size_t getSize() const noexcept override
    {
        return m_dataMp.getSize();
    }

    /// See Kit::Dm::IModelPoint. Delegates to the data Model Point
    bool toJSON( char* dst, size_t dstSize, bool& truncated, bool verbose = true, bool pretty = false ) noexcept override
    {
        return m_dataMp.toJSON( dst, dstSize, truncated, verbose, pretty );
    }

    /// See Kit::Dm::IModelPoint. Delegates to the data Model Point
    void genericAttach( IObserver& observer, uint16_t initialSeqNumber = SEQUENCE_NUMBER_UNKNOWN ) noexcept override
    {
        m_dataMp.genericAttach( observer, initialSeqNumber );
    }

    /// See Kit::Dm::IModelPoint. Delegates to the data Model Point
    void genericDetach( IObserver& observer ) noexcept override
    {
        m_dataMp.genericDetach( observer );
    }

    /// Not supported (does nothing).  Returns the current sequence number
    uint16_t touch() noexcept override
    {
        return getSequenceNumber();
    }

    /// Not supported (does nothing).  Returns the current sequence number
    uint16_t setInvalid( ChangeNotice_T chgNotice   = ChangeNotice_T::eNORMAL,
                         LockRequest_T  lockRequest = LockRequest_T::eNO_REQUEST ) noexcept override
    {
        return getSequenceNumber();
    }

    /// Not supported (does nothing).  Returns the current sequence number
    uint16_t setLockState( LockRequest_T lockRequest ) noexcept override
    {
        return getSequenceNumber();
    }

    /// Not supported (always returns zero)
    size_t getExternalSize( bool includeLockedState = false ) const noexcept override
    {
        return 0;
    }

    /// Not supported (always returns zero)
    size_t exportData( void* dstDataStream, size_t maxDstLength, uint16_t* retSequenceNumber = nullptr, bool includeLockedState = false ) const noexcept override
    {
        return 0;
    }

    /// Not supported (always returns zero)
    size_t importData( const void* srcDataStream, size_t srcLength, uint16_t* retSequenceNumber = nullptr, bool includeLockedState = false ) noexcept override
    {
        return 0;
    }

protected:
    /// Not supported (note: toJSON() is delegated to the data Model Point)
    bool setJSONVal( JsonDocument& doc ) noexcept override
    {
        return false;
    }

protected:
    /// The Model Point that holds the data
    MPTYPE& m_dataMp;

    /// Place holder for the base class's (unused) data
    uint8_t m_unused;
};


}  // end namespaces
}
}
#endif  // end header latch
