#ifndef KIT_DM_MP_ARRAYBASE_H_
#define KIT_DM_MP_ARRAYBASE_H_
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


/** The number of Elements in the temporary array (that is allocated on the
    STACK) when parsing the array elements in the fromJSON_() method.
 */
#ifndef OPTION_KIT_DM_MP_ARRAY_TEMP_ARRAY_NUM_ELEMENTS
#define OPTION_KIT_DM_MP_ARRAY_TEMP_ARRAY_NUM_ELEMENTS 8
#endif


///
namespace Kit {
///
namespace Dm {
///
namespace Mp {


/** This a mostly concrete class provides 'common' implementation for a Model
    Point who's data is a array of elements

    The toJSON() format is:
        \code

        { name:"<mpname>", type:"<mptypestring>", valid:true|false, seqnum:nnnn, locked:true|false,
          val:{ elems:[
                 <elemN>,
                 <elemN+1>,...
                 ]
              }
        }

        \endcode

    The "val" format for the fromJSON() format is:
        \code

        val:{ start:<firstIndex>,
              elems:[
                <elemN>,<elemN+1>,...
              ]
            }

        \endcode
 */
class ArrayBase : public Kit::Dm::ModelPointBase
{
protected:
    /// Meta data for read/write/copy operations
    struct MetaData_T
    {
        uint8_t* elemPtr;      //!< Pointer to the 1st element in the array to read/write
        size_t   numElements;  //!< Number of element to read/write
        size_t   elemIndex;    //!< Starting array index
    };

protected:
    /// Constructor: Invalid MP
    ArrayBase( Kit::Dm::IModelDatabase& myModelBase,
               const char*              symbolicName,
               void*                    myDataPtr,
               size_t                   numElements,
               size_t                   elementSize );


    /** Constructor.  Valid MP.  Requires an initial value.  If the 'initialValueSrcPtr'
        pointer is set to zero, then the entire array will be initialized to
        zero.   Note: The array that 'initialValueSrcPtr' points to ' MUST contain
        at least 'numElements' elements.
     */
    ArrayBase( Kit::Dm::IModelDatabase& myModelBase,
               const char*              symbolicName,
               void*                    myDataPtr,
               size_t                   numElements,
               size_t                   elementSize,
               void*                    initialValueSrcPtr );

protected:
    /** The caller can read a subset of array starting from the specified index
        in the Model Point's array.  Note: if srcIndex + dstNumElements exceeds
        the size of the MP's data then the read operation will be truncated.
    */
    virtual bool readArrayElements( void*     dstData,
                                    size_t    dstNumElements,
                                    size_t    srcIndex  = 0,
                                    uint16_t* seqNumPtr = 0 ) const noexcept;

    /** The caller can write a subset of array starting from the specified index
        in the Model Point's array.  Note: if dstIndex + srcNumElements exceeds
        the size of the MP's data then the write operation will be truncated

        NOTE: The application/caller is responsible for what a 'partial write'
        means to the integrity of the MP's data.  WARNING: Think before
        doing a partial write!  For example, if the MP is in the invalid
        state and a partial write is done - then the MP's data/array is
        only partially initialized AND then MP is now in the valid
        state!
    */
    virtual uint16_t writeArrayElements( const void*                         srcData,
                                         size_t                              srcNumElements,
                                         size_t                              dstIndex                = 0,
                                         bool                                forceChangeNotification = false,
                                         Kit::Dm::IModelPoint::LockRequest_T lockRequest             = Kit::Dm::IModelPoint::eNO_REQUEST ) noexcept;

    /// Updates the MP with the valid-state/data from 'src'. Note: the src.lock state is NOT copied
    virtual uint16_t copyArrayFrom( const ArrayBase& src, Kit::Dm::IModelPoint::LockRequest_T lockRequest = Kit::Dm::IModelPoint::eNO_REQUEST ) noexcept;

public:
    /// Returns the number of element in the array. This method IS thread safe.
    inline size_t getNumElements() const noexcept
    {
        return m_numElements;
    }

public:
    /// See Kit::Dm::ModelPoint
    void copyDataTo_( void* dstData, size_t dstSize ) const noexcept override;

    /// See Kit::Dm::ModelPoint
    void copyDataFrom_( const void* srcData, size_t srcSize ) noexcept override;

    /// See Kit::Dm::ModelPoint.
    bool isDataEqual_( const void* otherData ) const noexcept override;

    /// See Kit::Dm::Point.
    size_t getInternalDataSize_() const noexcept override;

    /// See Kit::Dm::ModelPoint.
    bool importMetadata_( const void* srcDataStream, size_t& bytesConsumed ) noexcept override;

    /// See Kit::Dm::ModelPoint.
    bool exportMetadata_( void* dstDataStream, size_t& bytesAdded ) const noexcept override;

protected:
    /// Number of elements in the array
    size_t m_numElements;

    /// Size, in bytes, of an element
    size_t m_elementSize;
};

}       // end namespaces
}
}
#endif  // end header latch
