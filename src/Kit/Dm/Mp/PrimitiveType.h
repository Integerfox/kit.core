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
#include "Kit/Text/StringTo.h"
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <type_traits>
#include <sys/types.h>

/// Endianess of a Bit array.  For little endian set to true; else set to false
#ifndef OPTION_KIT_DM_MP_BITARRAY_IS_LITTLE_ENDIAN
#define OPTION_KIT_DM_MP_BITARRAY_IS_LITTLE_ENDIAN true
#endif

/// The largest supported signed integer type for SignedInteger MP - as a intXX_t type
#ifndef OPTION_KIT_DM_MP_MAX_SIGNED_INT_TYPE
#define OPTION_KIT_DM_MP_MAX_SIGNED_INT_TYPE int64_t
#endif

/// The largest supported unsigned integer type for UnsignedInteger MP - as a uintXX_t type
#ifndef OPTION_KIT_DM_MP_MAX_UNSIGNED_INT_TYPE
#define OPTION_KIT_DM_MP_MAX_UNSIGNED_INT_TYPE uint64_t
#endif

/// Printf HEX format specifier for the largest supported unsigned integer type (e.g. "llu" for uint64_t)
#ifndef OPTION_KIT_DM_MP_MAX_INT_HEX_PRINTF_FORMAT
#define OPTION_KIT_DM_MP_MAX_INT_HEX_PRINTF_FORMAT PRIX64
#endif

///
namespace Kit {
///
namespace Dm {
///
namespace Mp {

/** This template class provides a mostly concrete implementation for a Model
    Point who's data is a C primitive type of type: 'ELEMTYPE'.
 */
template <class ELEMTYPE, class MPTYPE>
class PrimitiveType : public Kit::Dm::ModelPointBase
{
protected:
    /// The element's value
    ELEMTYPE m_data;

protected:
    /// Constructor: Invalid MP
    PrimitiveType( Kit::Dm::IModelDatabase& myModelBase, const char* symbolicName )
        : Kit::Dm::ModelPointBase( myModelBase, symbolicName, &m_data, sizeof( m_data ), false )
    {
    }

    /// Constructor: Valid MP (requires initial value)
    PrimitiveType( Kit::Dm::IModelDatabase& myModelBase, const char* symbolicName, ELEMTYPE initialValue )
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

protected:
    /// See Kit::Dm::Point.
    bool setJSONVal( JsonDocument& doc ) noexcept override
    {
        doc["val"] = m_data;
        return true;
    }    

public:
    /// See Kit::Dm::Point.
    bool fromJSON_( JsonVariant& src, LockRequest_T lockRequest, uint16_t& retSequenceNumber, Kit::Text::IString* errorMsg ) noexcept override
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

/** This template class extends the PrimitiveType class to provide numeric
    operations.
 */
template <class ELEMTYPE, class MPTYPE>
class NumericBase : public PrimitiveType<ELEMTYPE, MPTYPE>
{
protected:
    /// Constructor: Invalid MP
    NumericBase( Kit::Dm::IModelDatabase& myModelBase, const char* symbolicName )
        : PrimitiveType<ELEMTYPE, MPTYPE>( myModelBase, symbolicName )
    {
    }

    /// Constructor: Valid MP (requires initial value)
    NumericBase( Kit::Dm::IModelDatabase& myModelBase, const char* symbolicName, ELEMTYPE initialValue )
        : PrimitiveType<ELEMTYPE, MPTYPE>( myModelBase, symbolicName, initialValue )
    {
    }

public:
    /// Atomic increment
    inline uint16_t increment( ELEMTYPE                            incSize              = 1,
                               bool                                forceChgNotification = false,
                               Kit::Dm::IModelPoint::LockRequest_T lockRequest          = Kit::Dm::IModelPoint::eNO_REQUEST ) noexcept
    {
        Kit::System::Mutex::ScopeLock criticalSection( Kit::Dm::ModelPointBase::m_modelDatabase.getMutex_() );
        return this->write( this->m_data + incSize, forceChgNotification, lockRequest );
    }

    /// Atomic decrement
    inline uint16_t decrement( ELEMTYPE                            decSize              = 1,
                               bool                                forceChgNotification = false,
                               Kit::Dm::IModelPoint::LockRequest_T lockRequest          = Kit::Dm::IModelPoint::eNO_REQUEST ) noexcept
    {
        Kit::System::Mutex::ScopeLock criticalSection( Kit::Dm::ModelPointBase::m_modelDatabase.getMutex_() );
        return this->write( this->m_data - decSize, forceChgNotification, lockRequest );
    }
};

/** This template class provides a mostly concrete implementation for a Model
    Point who's data is a C integer primitive type of type: 'ELEMTYPE'.
 */
template <class ELEMTYPE, class MPTYPE>
class Integer : public NumericBase<ELEMTYPE, MPTYPE>
{
protected:
    /// Constructor: Invalid MP
    Integer( Kit::Dm::IModelDatabase& myModelBase, const char* symbolicName )
        : NumericBase<ELEMTYPE, MPTYPE>( myModelBase, symbolicName )
    {
    }

    /// Constructor: Valid MP (requires initial value)
    Integer( Kit::Dm::IModelDatabase& myModelBase, const char* symbolicName, ELEMTYPE initialValue )
        : NumericBase<ELEMTYPE, MPTYPE>( myModelBase, symbolicName, initialValue )
    {
    }

public:
    /** Atomic Read and then clear bits operation.

        NOTES:
            1. The return data value is BEFORE the clear operation.
            2. If the MP is invalid then the model point is set to the valid
               state AND 'dstData' is set to zero.
            3. The returned sequence number is the AFTER the clear operation.
     */
    inline uint16_t readThenClearBits( ELEMTYPE&                           dstData,
                                       ELEMTYPE                            maskToClear,
                                       bool                                forceChgNotification = false,
                                       Kit::Dm::IModelPoint::LockRequest_T lockRequest          = Kit::Dm::IModelPoint::eNO_REQUEST ) noexcept
    {
        Kit::System::Mutex::ScopeLock criticalSection( Kit::Dm::ModelPointBase::m_modelDatabase.getMutex_() );
        dstData          = this->m_data;
        ELEMTYPE newData = dstData & ~maskToClear;
        return Kit::Dm::ModelPointBase::writeData( &newData, sizeof( ELEMTYPE ), forceChgNotification, lockRequest );
    }

    /// Atomic Read and then clear (i.e. set to 0) operation.
    inline uint16_t readThenClear( ELEMTYPE&                           dstData,
                                   bool                                forceChgNotification = false,
                                   Kit::Dm::IModelPoint::LockRequest_T lockRequest          = Kit::Dm::IModelPoint::eNO_REQUEST ) noexcept
    {
        return readThenClearBits( dstData, (ELEMTYPE)-1, forceChgNotification, lockRequest );
    }

    /// Atomic bitwise OR operation
    inline uint16_t bitwiseOR( ELEMTYPE                            maskToOR,
                               bool                                forceChgNotification = false,
                               Kit::Dm::IModelPoint::LockRequest_T lockRequest          = Kit::Dm::IModelPoint::eNO_REQUEST ) noexcept
    {
        Kit::System::Mutex::ScopeLock criticalSection( Kit::Dm::ModelPointBase::m_modelDatabase.getMutex_() );
        return this->write( this->m_data | maskToOR, forceChgNotification, lockRequest );
    }

    /// Atomic bitwise XOR operation
    inline uint16_t bitwiseXOR( ELEMTYPE                            maskToXOR,
                                bool                                forceChgNotification = false,
                                Kit::Dm::IModelPoint::LockRequest_T lockRequest          = Kit::Dm::IModelPoint::eNO_REQUEST ) noexcept
    {
        Kit::System::Mutex::ScopeLock criticalSection( Kit::Dm::ModelPointBase::m_modelDatabase.getMutex_() );
        return this->write( this->m_data ^ maskToXOR, forceChgNotification, lockRequest );
    }

    /// Atomic bitwise AND operation
    inline uint16_t bitwiseAND( ELEMTYPE                            maskToAND,
                                bool                                forceChgNotification = false,
                                Kit::Dm::IModelPoint::LockRequest_T lockRequest          = Kit::Dm::IModelPoint::eNO_REQUEST ) noexcept
    {
        Kit::System::Mutex::ScopeLock criticalSection( Kit::Dm::ModelPointBase::m_modelDatabase.getMutex_() );
        return this->write( this->m_data & maskToAND, forceChgNotification, lockRequest );
    }

    /// Atomic bitwise AND operation
    inline uint16_t bitwiseClearAndSet( ELEMTYPE                            maskToClear,
                                        ELEMTYPE                            maskToSet,
                                        bool                                forceChgNotification = false,
                                        Kit::Dm::IModelPoint::LockRequest_T lockRequest          = Kit::Dm::IModelPoint::eNO_REQUEST ) noexcept
    {
        Kit::System::Mutex::ScopeLock criticalSection( Kit::Dm::ModelPointBase::m_modelDatabase.getMutex_() );
        return this->write( ( this->m_data & ~maskToClear ) | maskToSet, forceChgNotification, lockRequest );
    }

public:
    /// Atomic operation to set the zero indexed bit to a 1.
    inline uint16_t setBit( uint8_t                             bitPosition,
                            bool                                forceChgNotification = false,
                            Kit::Dm::IModelPoint::LockRequest_T lockRequest          = Kit::Dm::IModelPoint::eNO_REQUEST ) noexcept
    {
        Kit::System::Mutex::ScopeLock criticalSection( Kit::Dm::ModelPointBase::m_modelDatabase.getMutex_() );
        return this->write( this->m_data | ( 1 << bitPosition ), forceChgNotification, lockRequest );
    }

    /// Atomic operation to set the zero indexed bit to a 0.
    inline uint16_t clearBit( uint8_t                             bitPosition,
                              bool                                forceChgNotification = false,
                              Kit::Dm::IModelPoint::LockRequest_T lockRequest          = Kit::Dm::IModelPoint::eNO_REQUEST ) noexcept
    {
        Kit::System::Mutex::ScopeLock criticalSection( Kit::Dm::ModelPointBase::m_modelDatabase.getMutex_() );
        return this->write( this->m_data & ~( 1 << bitPosition ), forceChgNotification, lockRequest );
    }

    /// Atomic operation to toggle the zero indexed bit.
    inline uint16_t flipBit( uint8_t                             bitPosition,
                             bool                                forceChgNotification = false,
                             Kit::Dm::IModelPoint::LockRequest_T lockRequest          = Kit::Dm::IModelPoint::eNO_REQUEST ) noexcept
    {
        Kit::System::Mutex::ScopeLock criticalSection( Kit::Dm::ModelPointBase::m_modelDatabase.getMutex_() );
        return this->write( this->m_data ^ ( 1 << bitPosition ), forceChgNotification, lockRequest );
    }
};


/// This template class provides the to/from JSON methods for Signed Integers
template <class ELEMTYPE, class MPTYPE>
class SignedInteger : public Integer<ELEMTYPE, MPTYPE>
{
protected:
    /// Constructor: Invalid MP
    SignedInteger( Kit::Dm::IModelDatabase& myModelBase, const char* symbolicName )
        : Integer<ELEMTYPE, MPTYPE>( myModelBase, symbolicName )
    {
    }

    /// Constructor: Valid MP (requires initial value)
    SignedInteger( Kit::Dm::IModelDatabase& myModelBase, const char* symbolicName, ELEMTYPE initialValue )
        : Integer<ELEMTYPE, MPTYPE>( myModelBase, symbolicName, initialValue )
    {
    }

protected:
    /** Override the Parent's implementation. See Kit::Dm::Point.
        \code
            Output: val{ "dec": <decimal value>, "hex": <hexadecimal value> }
        \endcode
     */
    bool setJSONVal( JsonDocument& doc ) noexcept
    {
        Kit::Text::FString<sizeof( OPTION_KIT_DM_MP_MAX_SIGNED_INT_TYPE ) * 2 + 2> hexString;
        //
        OPTION_KIT_DM_MP_MAX_SIGNED_INT_TYPE val    = (typename std::make_unsigned<ELEMTYPE>::type)this->m_data;
        JsonObject                           valObj = doc.createNestedObject( "val" );
        hexString.format( "0x%" OPTION_KIT_DM_MP_MAX_INT_HEX_PRINTF_FORMAT, val );
        valObj["dec"] = this->m_data;
        valObj["hex"] = (char*)hexString.getString();
        return true;
    }

public:
    /** Override the Parent's implementation. See Kit::Dm::Point.
        \code
            Input: val: <number> OR val: "<string that can be parsed as a number>"
            For example:
                { "val": 123 }
                { "val": "123" }
                { "val": "0x7B" }
        \endcode
     */
    bool fromJSON_( JsonVariant& src, Kit::Dm::IModelPoint::LockRequest_T lockRequest, uint16_t& retSequenceNumber, Kit::Text::IString* errorMsg ) noexcept
    {
        // Parse a number
        if ( src.is<ELEMTYPE>() )
        {
            retSequenceNumber = this->write( src.as<ELEMTYPE>(), false, lockRequest );
            return true;
        }

        // Parse as a string (e.g. "0x12" or "123")
        if ( src.is<const char*>() )
        {
            // Try to parse the string as a number
            OPTION_KIT_DM_MP_MAX_SIGNED_INT_TYPE val;
            if ( Kit::Text::StringTo::signedInt( val, src.as<const char*>(), 0 ) )
            {
                retSequenceNumber = this->write( static_cast<ELEMTYPE>(val), false, lockRequest );
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

/// This template class provides the to/from JSON methods for Unsigned Integers
template <class ELEMTYPE, class MPTYPE>
class UnsignedInteger : public Integer<ELEMTYPE, MPTYPE>
{
protected:
    /// Constructor: Invalid MP
    UnsignedInteger( Kit::Dm::IModelDatabase& myModelBase, const char* symbolicName )
        : Integer<ELEMTYPE, MPTYPE>( myModelBase, symbolicName )
    {
    }

    /// Constructor: Valid MP (requires initial value)
    UnsignedInteger( Kit::Dm::IModelDatabase& myModelBase, const char* symbolicName, ELEMTYPE initialValue )
        : Integer<ELEMTYPE, MPTYPE>( myModelBase, symbolicName, initialValue )
    {
    }

protected:
    /** Override the Parent's implementation. See Kit::Dm::Point.
        \code
            Output: val{ "dec": <decimal value>, "hex": <hexadecimal value> }
        \endcode
     */
    bool setJSONVal( JsonDocument& doc ) noexcept
    {
        JsonObject                             valObj = doc.createNestedObject( "val" );
        Kit::Text::FString<20>                 hexString;
        OPTION_KIT_DM_MP_MAX_UNSIGNED_INT_TYPE val = Integer<ELEMTYPE, MPTYPE>::m_data;
        hexString.format( "0x%" OPTION_KIT_DM_MP_MAX_INT_HEX_PRINTF_FORMAT, val );
        valObj["dec"] = Integer<ELEMTYPE, MPTYPE>::m_data;
        valObj["hex"] = (char*)hexString.getString();
        return true;
    }

public:
    /** Override the Parent's implementation. See Kit::Dm::Point.
        \code
            Input: val: <number> OR val: "<string that can be parsed as a number>"
            For example:
                { "val": 123 }
                { "val": "123" }
                { "val": "0x7B" }
        \endcode
     */
    bool fromJSON_( JsonVariant& src, Kit::Dm::IModelPoint::LockRequest_T lockRequest, uint16_t& retSequenceNumber, Kit::Text::IString* errorMsg ) noexcept
    {
        // Parse a number
        if ( src.is<ELEMTYPE>() )
        {
            retSequenceNumber = this->write( src.as<ELEMTYPE>(), false, lockRequest );
            return true;
        }

        // Parse as a string (e.g. "0x12" or "123")
        if ( src.is<const char*>() )
        {
            // Try to parse the string as a number
            OPTION_KIT_DM_MP_MAX_UNSIGNED_INT_TYPE val;
            if ( Kit::Text::StringTo::unsignedInt( val, src.as<const char*>(), 0 ) )
            {
                retSequenceNumber = this->write( static_cast<ELEMTYPE>(val), false, lockRequest );
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


}  // end namespaces
}
}
#endif  // end header latch
