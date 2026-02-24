#ifndef KIT_DM_MP_NUMERIC_ARRAY_H_
#define KIT_DM_MP_NUMERIC_ARRAY_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */


#include "Kit/Dm/Mp/ArrayBase.h"
#include "Kit/Dm/Observer.h"

///
namespace Kit {
///
namespace Dm {
///
namespace Mp {


/** This template class extends the implementation of ArrayBase for arrays of
    numeric types (e.g. int, float, double, etc).
*/
template <class ELEMTYPE, int NUMELEMS, class MPTYPE>
class NumericArray : public ArrayBase
{
protected:
    /// Constructor: Invalid MP
    NumericArray( Kit::Dm::IModelDatabase& myModelBase,
                  const char*              symbolicName )
        : ArrayBase( myModelBase, symbolicName, m_data, NUMELEMS, sizeof( ELEMTYPE ) )
    {
    }

    /** Constructor.  Valid MP.  Requires an initial value.  If the 'srcData'
        pointer is set to zero, then the entire array will be initialized to
        zero.   Note: 'srcData' MUST contain at least 'numElements' elements.
    */
    NumericArray( Kit::Dm::IModelDatabase& myModelBase,
                  const char*              symbolicName,
                  ELEMTYPE*                srcData )
        : ArrayBase( myModelBase, symbolicName, m_data, NUMELEMS, sizeof( ELEMTYPE ), srcData )
    {
    }

public:
    /// Type safe read. See Kit::Dm::IModelPoint
    inline bool read( ELEMTYPE* dstArrray,
                      size_t    dstNumElements,
                      size_t    srcIndex  = 0,
                      uint16_t* seqNumPtr = 0 ) const noexcept
    {
        return ArrayBase::readArrayElements( dstArrray, dstNumElements, srcIndex, seqNumPtr );
    }

    /// Type safe write. See Kit::Dm::IModelPoint
    inline uint16_t write( const ELEMTYPE*                     srcArray,
                           size_t                              srcNumElements,
                           size_t                              dstIndex                = 0,
                           bool                                forceChangeNotification = false,
                           Kit::Dm::IModelPoint::LockRequest_T lockRequest             = Kit::Dm::IModelPoint::eNO_REQUEST ) noexcept
    {
        return ArrayBase::writeArrayElements( srcArray, srcNumElements, dstIndex, forceChangeNotification, lockRequest );
    }

public:
    /// Updates the MP's data/valid-state from 'src'.
    inline uint16_t copyFrom( const MPTYPE& src, Kit::Dm::IModelPoint::LockRequest_T lockRequest = Kit::Dm::IModelPoint::eNO_REQUEST ) noexcept
    {
        return ArrayBase::copyArrayFrom( src, lockRequest );
    }

    /// Type safe register observer
    inline void attach( Kit::Dm::Observer<MPTYPE>& observer, uint16_t initialSeqNumber = Kit::Dm::IModelPoint::SEQUENCE_NUMBER_UNKNOWN ) noexcept
    {
        attachObserver( observer, initialSeqNumber );
    }

    /// Type safe un-register observer
    inline void detach( Kit::Dm::Observer<MPTYPE>& observer ) noexcept
    {
        detachObserver( observer );
    }

public:
    /** This method is used to read the MP contents and synchronize
        the observer with the current MP contents.  This method should ONLY be
        used in the notification callback method and the 'observerToSync'
        argument MUST be the argument provided by the callback method

        Note: The observer will be subscribed for change notifications after
              this call.
     */
    inline bool readAndSync( ELEMTYPE*  dstArrray,
                             size_t     dstNumElements,
                             IObserver& observerToSync,
                             size_t     srcIndex = 0 )
    {
        uint16_t seqNum;
        return readAndSync( dstArrray, dstNumElements, observerToSync, seqNum, srcIndex );
    }

    /** Same as readAndSync() above, but in addition returns the
        sequence number of the MP.
     */
    inline bool readAndSync( ELEMTYPE*  dstArrray,
                             size_t     dstNumElements,
                             IObserver& observerToSync,
                             uint16_t&  seqNum,
                             size_t     srcIndex = 0 )
    {
        bool result = ArrayBase::readArrayElements( dstArrray, dstNumElements, srcIndex, &seqNum );
        attach( observerToSync, seqNum );
        return result;
    }

protected:
    /// See Kit::Dm::Point.
    bool setJSONVal( JsonDocument& doc ) noexcept override
    {
        JsonObject obj     = doc.createNestedObject( "val" );
        JsonArray  arr     = obj.createNestedArray( "elems" );
        ELEMTYPE*  elemPtr = (ELEMTYPE*)m_dataPtr;
        for ( size_t i = 0; i < m_numElements; i++ )
        {
            arr.add( elemPtr[i] );
        }
        return true;
    }

public:
    /// See Kit::Dm::Point.
    bool fromJSON_( JsonVariant& src, Kit::Dm::IModelPoint::LockRequest_T lockRequest, uint16_t& retSequenceNumber, Kit::Text::IString* errorMsg ) noexcept override
    {
        // Check for object
        if ( src.is<JsonObject>() == false )
        {
            if ( errorMsg )
            {
                *errorMsg = "'val' key/value pair is NOT an JSON object";
            }
            return false;
        }

        // Check for embedded array
        JsonArray elems = src["elems"];
        if ( elems.isNull() )
        {
            if ( errorMsg )
            {
                *errorMsg = "'val' key/value pair is missing the embedded 'elems' array";
            }
            return false;
        }

        // Get starting index (note: if not present a default of '0' will be returned)
        size_t startIdx = src["start"] | 0;

        // Check for exceeding array limits
        size_t numElements = elems.size();
        if ( numElements + startIdx > m_numElements )
        {
            if ( errorMsg )
            {
                errorMsg->format( "Number of array elements ([%lu+%lu)] exceeds the MP's element count (%lu)", startIdx, numElements, m_numElements );
            }
            return false;
        }

        // Is the element syntactically correct?
        if ( elems[0].is<ELEMTYPE>() == false )
        {
            if ( errorMsg )
            {
                errorMsg->format( "Failed parsing element[0]. Content of the MP is suspect!" );
            }
            return false;
        }

        // Update the Model Point in 'M' elements at a time (helps to reduce 'noise' on the MP's sequence number)
        size_t offset = 0;
        while ( numElements )
        {
            // Attempt to parse the value key/value pair (as a simple numeric)
            ELEMTYPE tempArray[OPTION_KIT_DM_MP_ARRAY_TEMP_ARRAY_NUM_ELEMENTS];
            size_t   idx;
            for ( idx = 0; idx < numElements && idx < OPTION_KIT_DM_MP_ARRAY_TEMP_ARRAY_NUM_ELEMENTS; idx++ )
            {
                tempArray[idx] = elems[idx + offset].as<ELEMTYPE>();
            }
            retSequenceNumber  = ArrayBase::writeArrayElements( tempArray, idx, startIdx + offset, lockRequest );
            offset            += idx;
            numElements       -= idx;
        }

        return true;
    }

protected:
    /// The data store the MP
    ELEMTYPE m_data[NUMELEMS];
};

//////////////////////////////////////////////////////////////////////////////
/* The following classes provide concrete numeric ArrayBase types for basic types
 */

/// uint8_t ArrayBase
template <int N>
class ArrayUint8 : public NumericArray<uint8_t, N, ArrayUint8<N>>
{
public:
    /// Constructor. Invalid Point
    ArrayUint8( Kit::Dm::IModelDatabase& myModelBase, const char* symbolicName )
        : Mp::NumericArray<uint8_t, N, ArrayUint8<N>>( myModelBase, symbolicName )
    {
    }

    /// Constructor. Valid Point.  Requires an initial value. The array size of 'initialValueArray' must match 'N'
    ArrayUint8( Kit::Dm::IModelDatabase& myModelBase, const char* symbolicName, uint8_t initialValueArray[] )
        : Mp::NumericArray<uint8_t, N, ArrayUint8<N>>( myModelBase, symbolicName, initialValueArray )
    {
    }

    ///  See Kit::Dm::IModelPoint.
    const char* getTypeAsText() const noexcept override
    {
        return "Kit::Dm::Mp::ArrayUint8";
    }
};

/// uint32_t ArrayBase
template <int N>
class ArrayUint32 : public NumericArray<uint32_t, N, ArrayUint32<N>>
{
public:
    /// Constructor. Invalid Point
    ArrayUint32( Kit::Dm::IModelDatabase& myModelBase, const char* symbolicName )
        : Mp::NumericArray<uint32_t, N, ArrayUint32<N>>( myModelBase, symbolicName )
    {
    }

    /// Constructor. Valid Point.  Requires an initial value. The array size of 'initialValueArray' must match 'N'
    ArrayUint32( Kit::Dm::IModelDatabase& myModelBase, const char* symbolicName, uint32_t initialValueArray[] )
        : Mp::NumericArray<uint32_t, N, ArrayUint32<N>>( myModelBase, symbolicName, initialValueArray )
    {
    }

    ///  See Kit::Dm::IModelPoint.
    const char* getTypeAsText() const noexcept override
    {
        return "Kit::Dm::Mp::ArrayUint32";
    }
};

/// uint64_t ArrayBase
template <int N>
class ArrayUint64 : public NumericArray<uint64_t, N, ArrayUint64<N>>
{
public:
    /// Constructor. Invalid Point
    ArrayUint64( Kit::Dm::IModelDatabase& myModelBase, const char* symbolicName )
        : Mp::NumericArray<uint64_t, N, ArrayUint64<N>>( myModelBase, symbolicName )
    {
    }

    /// Constructor. Valid Point.  Requires an initial value. The array size of 'initialValueArray' must match 'N'
    ArrayUint64( Kit::Dm::IModelDatabase& myModelBase, const char* symbolicName, uint64_t initialValueArray[] )
        : Mp::NumericArray<uint64_t, N, ArrayUint64<N>>( myModelBase, symbolicName, initialValueArray )
    {
    }

    ///  See Kit::Dm::IModelPoint.
    const char* getTypeAsText() const noexcept override
    {
        return "Kit::Dm::Mp::ArrayUint64";
    }
};

/// int8_t ArrayBase
template <int N>
class ArrayInt8 : public NumericArray<int8_t, N, ArrayInt8<N>>
{
public:
    /// Constructor. Invalid Point
    ArrayInt8( Kit::Dm::IModelDatabase& myModelBase, const char* symbolicName )
        : Mp::NumericArray<int8_t, N, ArrayInt8<N>>( myModelBase, symbolicName )
    {
    }

    /// Constructor. Valid Point.  Requires an initial value. The array size of 'initialValueArray' must match 'N'
    ArrayInt8( Kit::Dm::IModelDatabase& myModelBase, const char* symbolicName, int8_t initialValueArray[] )
        : Mp::NumericArray<int8_t, N, ArrayInt8<N>>( myModelBase, symbolicName, initialValueArray )
    {
    }

    ///  See Kit::Dm::IModelPoint.
    const char* getTypeAsText() const noexcept override
    {
        return "Kit::Dm::Mp::ArrayInt8";
    }
};

/// int32_t ArrayBase
template <int N>
class ArrayInt32 : public NumericArray<int32_t, N, ArrayInt32<N>>
{
public:
    /// Constructor. Invalid Point
    ArrayInt32( Kit::Dm::IModelDatabase& myModelBase, const char* symbolicName )
        : Mp::NumericArray<int32_t, N, ArrayInt32<N>>( myModelBase, symbolicName )
    {
    }

    /// Constructor. Valid Point.  Requires an initial value. The array size of 'initialValueArray' must match 'N'
    ArrayInt32( Kit::Dm::IModelDatabase& myModelBase, const char* symbolicName, int32_t initialValueArray[] )
        : Mp::NumericArray<int32_t, N, ArrayInt32<N>>( myModelBase, symbolicName, initialValueArray )
    {
    }

    ///  See Kit::Dm::IModelPoint.
    const char* getTypeAsText() const noexcept override
    {
        return "Kit::Dm::Mp::ArrayInt32";
    }
};

/// int64_t ArrayBase
template <int N>
class ArrayInt64 : public NumericArray<int64_t, N, ArrayInt64<N>>
{
public:
    /// Constructor. Invalid Point
    ArrayInt64( Kit::Dm::IModelDatabase& myModelBase, const char* symbolicName )
        : Mp::NumericArray<int64_t, N, ArrayInt64<N>>( myModelBase, symbolicName )
    {
    }

    /// Constructor. Valid Point.  Requires an initial value. The array size of 'initialValueArray' must match 'N'
    ArrayInt64( Kit::Dm::IModelDatabase& myModelBase, const char* symbolicName, int64_t initialValueArray[] )
        : Mp::NumericArray<int64_t, N, ArrayInt64<N>>( myModelBase, symbolicName, initialValueArray )
    {
    }

    ///  See Kit::Dm::IModelPoint.
    const char* getTypeAsText() const noexcept override
    {
        return "Kit::Dm::Mp::ArrayInt64";
    }
};

/// float ArrayBase
template <int N>
class ArrayFloat : public NumericArray<float, N, ArrayFloat<N>>
{
public:
    /// Constructor. Invalid Point
    ArrayFloat( Kit::Dm::IModelDatabase& myModelBase, const char* symbolicName )
        : Mp::NumericArray<float, N, ArrayFloat<N>>( myModelBase, symbolicName )
    {
    }

    /// Constructor. Valid Point.  Requires an initial value. The array size of 'initialValueArray' must match 'N'
    ArrayFloat( Kit::Dm::IModelDatabase& myModelBase, const char* symbolicName, float initialValueArray[] )
        : Mp::NumericArray<float, N, ArrayFloat<N>>( myModelBase, symbolicName, initialValueArray )
    {
    }

    ///  See Kit::Dm::ModelPoint.
    const char* getTypeAsText() const noexcept override
    {
        return "Kit::Dm::Mp::ArrayFloat";
    }
};

/// double ArrayBase
template <int N>
class ArrayDouble : public NumericArray<double, N, ArrayDouble<N>>
{
public:
    /// Constructor. Invalid Point
    ArrayDouble( Kit::Dm::IModelDatabase& myModelBase, const char* symbolicName )
        : Mp::NumericArray<double, N, ArrayDouble<N>>( myModelBase, symbolicName )
    {
    }

    /// Constructor. Valid Point.  Requires an initial value. The array size of 'initialValueArray' must match 'N'
    ArrayDouble( Kit::Dm::IModelDatabase& myModelBase, const char* symbolicName, double initialValueArray[] )
        : Mp::NumericArray<double, N, ArrayDouble<N>>( myModelBase, symbolicName, initialValueArray )
    {
    }

    ///  See Kit::Dm::ModelPoint.
    const char* getTypeAsText() const noexcept override
    {
        return "Kit::Dm::Mp::ArrayDouble";
    }
};

};  // end namespaces
};
};
#endif  // end header latch
