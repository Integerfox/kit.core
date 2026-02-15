#ifndef KIT_DM_MP_NUMERICBASE_H_
#define KIT_DM_MP_NUMERICBASE_H_
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
#include "Kit/Text/FString.h"
#include <string.h>
#include <stdint.h>


/// Endianess of a Bit array.  For little endian set to true; else set to false
#ifndef OPTION_KIT_DM_MP_BITARRAY_IS_LITTLE_ENDIAN
#define OPTION_KIT_DM_MP_BITARRAY_IS_LITTLE_ENDIAN true
#endif


///
namespace Kit {
///
namespace Dm {
///
namespace Mp {

/** This template class provides a mostly concrete implementation for a Model
    Point who's data is a C numeric primitive type of type: 'ELEMTYPE'.

    NOTES:
        1) All methods in this class are NOT thread Safe unless explicitly
           documented otherwise.
 */
template <class ELEMTYPE, class MPTYPE>
class Numeric : public Kit::Dm::ModelPointBase
{
protected:
    /// The element's value
    ELEMTYPE m_data;

protected:
    /// Constructor: Invalid MP
    Numeric( Kit::Dm::IModelDatabase& myModelBase, const char* symbolicName )
        : Kit::Dm::ModelPointBase( myModelBase, symbolicName, &m_data, sizeof( m_data ), false )
    {
    }

    /// Constructor: Valid MP (requires initial value)
    Numeric( Kit::Dm::IModelDatabase& myModelBase, const char* symbolicName, ELEMTYPE initialValue )
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
    inline uint16_t write( ELEMTYPE                            newValue,
                           bool                                forceChgNotification = false,
                           Kit::Dm::IModelPoint::LockRequest_T lockRequest          = Kit::Dm::IModelPoint::eNO_REQUEST ) noexcept
    {
        return Kit::Dm::ModelPointBase::writeData( &newValue, sizeof( ELEMTYPE ), forceChgNotification, lockRequest );
    }

    /// Atomic increment
    inline uint16_t increment( ELEMTYPE                            incSize              = 1,
                               bool                                forceChgNotification = false,
                               Kit::Dm::IModelPoint::LockRequest_T lockRequest          = Kit::Dm::IModelPoint::eNO_REQUEST ) noexcept
    {
        Kit::System::Mutex::ScopeLock criticalSection( Kit::Dm::ModelPointBase::m_modelDatabase.getMutex_() );
        return write( m_data + incSize, forceChgNotification, lockRequest );
    }

    /// Atomic decrement
    inline uint16_t decrement( ELEMTYPE                            decSize              = 1,
                               bool                                forceChgNotification = false,
                               Kit::Dm::IModelPoint::LockRequest_T lockRequest          = Kit::Dm::IModelPoint::eNO_REQUEST ) noexcept
    {
        Kit::System::Mutex::ScopeLock criticalSection( Kit::Dm::ModelPointBase::m_modelDatabase.getMutex_() );
        return write( m_data - decSize, forceChgNotification, lockRequest );
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

/** This template class provides a mostly concrete implementation for a Model
    Point who's data is a C integer primitive type of type: 'ELEMTYPE'.

    NOTES:
        1) All methods in this class are NOT thread Safe unless explicitly
           documented otherwise.
 */
template <class ELEMTYPE, class MPTYPE>
class NumericInteger : public Numeric<ELEMTYPE, MPTYPE>
{
protected:
    /// The element's value
    ELEMTYPE m_data;

protected:
    /// Constructor: Invalid MP
    NumericInteger( Kit::Dm::IModelDatabase& myModelBase, const char* symbolicName )
        : Numeric<ELEMTYPE, MPTYPE>( myModelBase, symbolicName, &m_data, sizeof( m_data ), false )
    {
    }

    /// Constructor: Valid MP (requires initial value)
    NumericInteger( Kit::Dm::IModelDatabase& myModelBase, const char* symbolicName, ELEMTYPE initialValue )
        : Numeric<ELEMTYPE, MPTYPE>( myModelBase, symbolicName, &m_data, sizeof( m_data ), true )
    {
    }

public:
    /** Atomic Read and then clear bits operation.
        NOTES:
            1. The return data value is BEFORE the clear operation.
            2. If the MP is invalid then NO clear operation occurs.
            3. When the MP is in the valid state, the returned sequence number
               (if requested) is the AFTER the clear operation.
     */
    inline uint16_t readThenClearBits( ELEMTYPE& dstData, ELEMTYPE maskToClear, uint16_t* seqNumPtr = nullptr ) noexcept
    {
        Kit::System::Mutex::ScopeLock criticalSection( Kit::Dm::ModelPointBase::m_modelDatabase.getMutex_() );
        bool                          result = Kit::Dm::ModelPointBase::readData( &dstData, sizeof( ELEMTYPE ), seqNumPtr );
        if ( result )
        {
            ELEMTYPE newData = m_data & ~maskToClear;
            uint16_t seqNum  = Kit::Dm::ModelPointBase::writeData( &newData, sizeof( ELEMTYPE ), Kit::Dm::IModelPoint::eNO_REQUEST );
            if ( seqNumPtr )
            {
                *seqNumPtr = seqNum;
            }
        }
        return result;
    }

    /// Atomic Read and then clear (i.e. set to 0) operation.
    inline uint16_t readThenClear( ELEMTYPE& dstData, uint16_t* seqNumPtr = nullptr ) noexcept
    {
        return readThenClearBits( dstData, (ELEMTYPE)-1, seqNumPtr );
    }

    /// Atomic bitwise OR operation
    inline uint16_t bitwiseOR( ELEMTYPE                            maskToOR,
                               bool                                forceChgNotification = false,
                               Kit::Dm::IModelPoint::LockRequest_T lockRequest          = Kit::Dm::IModelPoint::eNO_REQUEST ) noexcept
    {
        Kit::System::Mutex::ScopeLock criticalSection( Kit::Dm::ModelPointBase::m_modelDatabase.getMutex_() );
        return write( m_data | maskToOR, forceChgNotification, lockRequest );
    }

    /// Atomic bitwise XOR operation
    inline uint16_t bitwiseXOR( ELEMTYPE                            maskToXOR,
                                bool                                forceChgNotification = false,
                                Kit::Dm::IModelPoint::LockRequest_T lockRequest          = Kit::Dm::IModelPoint::eNO_REQUEST ) noexcept
    {
        Kit::System::Mutex::ScopeLock criticalSection( Kit::Dm::ModelPointBase::m_modelDatabase.getMutex_() );
        return write( m_data ^ maskToXOR, forceChgNotification, lockRequest );
    }

    /// Atomic bitwise AND operation
    inline uint16_t bitwiseAND( ELEMTYPE                            maskToAND,
                                bool                                forceChgNotification = false,
                                Kit::Dm::IModelPoint::LockRequest_T lockRequest          = Kit::Dm::IModelPoint::eNO_REQUEST ) noexcept
    {
        Kit::System::Mutex::ScopeLock criticalSection( Kit::Dm::ModelPointBase::m_modelDatabase.getMutex_() );
        return write( m_data & maskToAND, forceChgNotification, lockRequest );
    }

    /// Atomic bitwise AND operation
    inline uint16_t bitwiseClearAndSet( ELEMTYPE                            maskToClear,
                                        ELEMTYPE                            maskToSet,
                                        bool                                forceChgNotification = false,
                                        Kit::Dm::IModelPoint::LockRequest_T lockRequest          = Kit::Dm::IModelPoint::eNO_REQUEST ) noexcept
    {
        Kit::System::Mutex::ScopeLock criticalSection( Kit::Dm::ModelPointBase::m_modelDatabase.getMutex_() );
        return write( ( m_data & ~maskToClear ) | maskToSet, forceChgNotification, lockRequest );
    }

protected:
    /// See Kit::Dm::Point. -->TODO: decimal & hexadecimal format output
    void setJSONVal( JsonDocument& doc ) noexcept
    {
        doc["val"] = m_data;
    }

public:
    /// See Kit::Dm::Point. TODO: parse either decimal or hexadecimal format input
    bool fromJSON_( JsonVariant& src, Kit::Dm::IModelPoint::LockRequest_T lockRequest, uint16_t& retSequenceNumber, Kit::Text::String* errorMsg ) noexcept
    {
        if ( src.is<ELEMTYPE>() )
        {
            retSequenceNumber = write( src.as<ELEMTYPE>(), lockRequest );
            return true;
        }
        if ( errorMsg )
        {
            *errorMsg = "Invalid syntax for the 'val' key/value pair";
        }
        return false;
    }
};


/** This template class extends the Numeric<> class to provide bit operation
    on the numeric value.  The datatype of the numeric MUST be an integer
    type.

    The underlying storage of the bit array is N bit integers.  A side effect of
    this storage mechanism the bit ordering in the JSON 'val' string is dependent on the
    target platform's Endian architecture.

    The toJSON()/fromJSON format is:
        \code

        { name:"<mpname>", type:"<mptypestring>", valid:true|false, seqnum:nnnn, locked:true|false, val:"<bits>" }

        where <bits> is a string of N digits ('1' or '0') where the left most digit is
        is the MSb of byte[0] and the right most digit is the LSb of byte[N].
        Whether byte[0] is the MSB or LSB is dependent on the big/little Endian
        architecture of the target platform.

        For example a 16bit Array (as binary hex: dataword[0]=0x30, dataword[1]=0x09)

            val:"0011000000001001"

        \endcode

    NOTE: All methods in this class ARE thread Safe unless explicitly
          documented otherwise.

 */
template <class WORDSIZE, class MPTYPE>
class BitArray_ : public Numeric<WORDSIZE, MPTYPE>
{
protected:
    /// Constructor. Invalid MP.
    BitArray_( Kit::Dm::IModelDatabase& myModelBase, const char* symbolicName )
        : Numeric<WORDSIZE, MPTYPE>( myModelBase, symbolicName )
    {
    }

    /// Constructor. Valid MP.  Requires an initial value
    BitArray_( Kit::Dm::IModelDatabase& myModelBase, const char* symbolicName, WORDSIZE initialValue )
        : Numeric<WORDSIZE, MPTYPE>( myModelBase, symbolicName, initialValue )
    {
    }

public:
    /// Atomic operation to set the zero indexed bit to a 1.
    inline uint16_t setBit( uint8_t bitPosition, Kit::Dm::IModelPoint::LockRequest_T lockRequest = Kit::Dm::IModelPoint::eNO_REQUEST ) noexcept
    {
        Kit::Dm::ModelPointBase::m_modelDatabase.lock_();
        uint16_t result = Numeric<WORDSIZE, MPTYPE>::write( Numeric<WORDSIZE, MPTYPE>::m_data | ( 1 << bitPosition ), lockRequest );
        Kit::Dm::ModelPointBase::m_modelDatabase.unlock_();
        return result;
    }

    /// Atomic operation to set the zero indexed bit to a 0.
    inline uint16_t clearBit( uint8_t bitPosition, Kit::Dm::IModelPoint::LockRequest_T lockRequest = Kit::Dm::IModelPoint::eNO_REQUEST ) noexcept
    {
        Kit::Dm::ModelPointBase::m_modelDatabase.lock_();
        uint16_t result = Numeric<WORDSIZE, MPTYPE>::write( Numeric<WORDSIZE, MPTYPE>::m_data & ( ~( 1 << bitPosition ) ), lockRequest );
        Kit::Dm::ModelPointBase::m_modelDatabase.unlock_();
        return result;
    }

    /// Atomic operation to toggle the zero indexed bit.
    inline uint16_t flipBit( uint8_t bitPosition, Kit::Dm::IModelPoint::LockRequest_T lockRequest = Kit::Dm::IModelPoint::eNO_REQUEST ) noexcept
    {
        Kit::Dm::ModelPointBase::m_modelDatabase.lock_();
        uint16_t result = Numeric<WORDSIZE, MPTYPE>::write( Numeric<WORDSIZE, MPTYPE>::m_data ^ ( 1 << bitPosition ), lockRequest );
        Kit::Dm::ModelPointBase::m_modelDatabase.unlock_();
        return result;
    }


public:
    /// Atomic operation to clear ONLY the bits as specified by the bit mask.
    inline uint16_t clearBitsByMask( WORDSIZE bitMask, Kit::Dm::IModelPoint::LockRequest_T lockRequest = Kit::Dm::IModelPoint::eNO_REQUEST ) noexcept
    {
        return Numeric<WORDSIZE, MPTYPE>::bitwiseAND( ~bitMask, lockRequest );
    }

    /// Atomic operation to set the bits specified by the bit mask
    inline uint16_t setBitsByMask( WORDSIZE bitMask, Kit::Dm::IModelPoint::LockRequest_T lockRequest = Kit::Dm::IModelPoint::eNO_REQUEST ) noexcept
    {
        return Numeric<WORDSIZE, MPTYPE>::bitwiseOR( bitMask, lockRequest );
    }

    /// Atomic operation to flip/toggle ONLY the bits as specified the bit mask
    inline uint16_t flipBitsByMask( WORDSIZE bitMask, Kit::Dm::IModelPoint::LockRequest_T lockRequest = Kit::Dm::IModelPoint::eNO_REQUEST ) noexcept
    {
        return Numeric<WORDSIZE, MPTYPE>::bitwiseXOR( bitMask, lockRequest );
    }



protected:
    /// See Kit::Dm::Point.
    void setJSONVal( JsonDocument& doc ) noexcept
    {
        Kit::Text::FString<64> tmp;
        const void*            dataPtr = &( Numeric<WORDSIZE, MPTYPE>::m_data );
        Kit::Text::bufferToAsciiBinary( dataPtr, sizeof( Numeric<WORDSIZE, MPTYPE>::m_data ), tmp, false, OPTION_KIT_DM_MP_BITARRAY_IS_LITTLE_ENDIAN );
        doc["val"] = (char*)tmp.getString();
    }


public:
    /// See Kit::Dm::Point.
    bool fromJSON_( JsonVariant& src, Kit::Dm::IModelPoint::LockRequest_T lockRequest, uint16_t& retSequenceNumber, Kit::Text::String* errorMsg ) noexcept
    {
        if ( src.is<const char*>() )
        {
            const char* val   = src.as<const char*>();
            WORDSIZE    value = 0;
            if ( Kit::Text::asciiBinaryToBuffer( &value, val, sizeof( value ), OPTION_KIT_DM_MP_BITARRAY_IS_LITTLE_ENDIAN ) > 0 )
            {
                retSequenceNumber = Numeric<WORDSIZE, MPTYPE>::write( value, lockRequest );
                return true;
            }
        }

        if ( errorMsg )
        {
            *errorMsg = "Invalid syntax for the 'val' key/value pair";
        }
        return false;
    }
};


/** This template class extends the implementation of Numeric<> class to support
    the pointers instead of integers

    NOTES:
    1) All methods in this class are NOT thread Safe unless explicitly
       documented otherwise.
 */
template <class MPTYPE>
class Pointer_ : public Numeric<size_t, MPTYPE>
{
protected:
    /// Constructor. Invalid MP.
    Pointer_( Kit::Dm::IModelDatabase& myModelBase, const char* symbolicName )
        : Numeric<size_t, MPTYPE>( myModelBase, symbolicName )
    {
    }

    /// Constructor. Valid MP.  Requires an initial value
    Pointer_( Kit::Dm::IModelDatabase& myModelBase, const char* symbolicName, void* initialValue )
        : Numeric<size_t, MPTYPE>( myModelBase, symbolicName, (size_t)initialValue )
    {
    }

public:
    /// See Kit::Dm::Point.
    void setJSONVal( JsonDocument& doc ) noexcept
    {
        Kit::Text::FString<20> tmp;
        tmp.format( PRINTF_SIZET_FMT, (PRINTF_SIZET_TYPE)Numeric<size_t, MPTYPE>::m_data );
        doc["val"] = (char*)tmp.getString();
    }

    /// See Kit::Dm::Point.
    bool fromJSON_( JsonVariant& src, Kit::Dm::IModelPoint::LockRequest_T lockRequest, uint16_t& retSequenceNumber, Kit::Text::String* errorMsg ) noexcept
    {
        if ( src.is<const char*>() )
        {
            const char*        val   = src.as<const char*>();
            unsigned long long value = 0;
            if ( Kit::Text::a2ull( value, val, 16 ) )
            {
                retSequenceNumber = Numeric<size_t, MPTYPE>::write( (size_t)value, lockRequest );
                return true;
            }
        }

        if ( errorMsg )
        {
            *errorMsg = "Invalid syntax for the 'val' key/value pair";
        }
        return false;
    }
};

};  // end namespaces
};
};
#endif  // end header latch
