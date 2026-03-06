/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */


#include "ArrayBase.h"
#include "Kit/System/Assert.h"
#include <string.h>

//------------------------------------------------------------------------------
namespace Kit {
namespace Dm {
namespace Mp {

/////////////////////////////////////////////////////
ArrayBase::ArrayBase( Kit::Dm::IModelDatabase& myModelBase,
                      const char*              symbolicName,
                      void*                    myDataPtr,
                      size_t                   numElements,
                      size_t                   elementSize )
    : Kit::Dm::ModelPointBase( myModelBase, symbolicName, myDataPtr, numElements * elementSize, false )
    , m_numElements( numElements )
    , m_elementSize( elementSize )
{
    // Initialize the array to all zero - so as to have deterministic 'invalid' value
    memset( myDataPtr, 0, m_dataSize );
}


ArrayBase::ArrayBase( Kit::Dm::IModelDatabase& myModelBase,
                      const char*              symbolicName,
                      void*                    myDataPtr,
                      size_t                   numElements,
                      size_t                   elementSize,
                      void*                    initialValueSrcPtr )
    : Kit::Dm::ModelPointBase( myModelBase, symbolicName, myDataPtr, numElements * elementSize, true )
    , m_numElements( numElements )
    , m_elementSize( elementSize )
{
    // Initialize the array to all zero
    if ( initialValueSrcPtr == nullptr )
    {
        memset( myDataPtr, 0, m_dataSize );
    }

    // Initialize the array to the provided data
    else
    {
        memcpy( myDataPtr, initialValueSrcPtr, m_dataSize );
    }
}


/////////////////////////////////////////////////////
bool ArrayBase::readArrayElements( void*     dstData,
                                   size_t    dstNumElements,
                                   size_t    srcIndex,
                                   uint16_t* seqNumPtr ) const noexcept
{
    MetaData_T dst = { (uint8_t*)dstData, dstNumElements, srcIndex };
    return readData( &dst, sizeof( dst ), seqNumPtr );
}

uint16_t ArrayBase::writeArrayElements( const void*                         srcData,
                                        size_t                              srcNumElements,
                                        size_t                              dstIndex,
                                        bool                                forceChangeNotification,
                                        Kit::Dm::IModelPoint::LockRequest_T lockRequest ) noexcept
{
    MetaData_T src = { (uint8_t*)srcData, srcNumElements, dstIndex };
    return writeData( &src, sizeof( src ), forceChangeNotification, lockRequest );
}

uint16_t ArrayBase::copyArrayFrom( const ArrayBase& src, LockRequest_T lockRequest ) noexcept
{
    // Handle the src.invalid case
    if ( src.isNotValid() )
    {
        return setInvalid();
    }

    return ArrayBase::writeArrayElements( src.m_dataPtr, src.m_numElements, 0, false, lockRequest );
}

/////////////////////////////////////////////////////
void ArrayBase::copyDataTo_( void* dstData, size_t dstSize ) const noexcept
{
    KIT_SYSTEM_ASSERT( dstSize == sizeof( MetaData_T ) );
    MetaData_T* dstInfo = (MetaData_T*)dstData;

    // Make sure we don't read past the m_data storage
    if ( dstInfo->elemIndex + dstInfo->numElements > m_numElements )
    {
        dstInfo->numElements = m_numElements - dstInfo->elemIndex;
    }

    // Copy the data to 'dst'
    uint8_t* arrayStartPtr = static_cast<uint8_t*>( m_dataPtr );
    memcpy( dstInfo->elemPtr, &( arrayStartPtr[dstInfo->elemIndex * m_elementSize] ), dstInfo->numElements * m_elementSize );
}

void ArrayBase::copyDataFrom_( const void* srcData, size_t srcSize ) noexcept
{
    KIT_SYSTEM_ASSERT( srcSize == sizeof( MetaData_T ) );
    MetaData_T* srcInfo = (MetaData_T*)srcData;

    // Make sure we don't write past the m_data storage
    if ( srcInfo->elemIndex + srcInfo->numElements > m_numElements )
    {
        srcInfo->numElements = m_numElements - srcInfo->elemIndex;
    }

    // Copy the data to 'src'
    uint8_t* arrayStartPtr = (uint8_t*)m_dataPtr;
    memcpy( &( arrayStartPtr[srcInfo->elemIndex * m_elementSize] ), srcInfo->elemPtr, srcInfo->numElements * m_elementSize );
}

bool ArrayBase::isDataEqual_( const void* otherData ) const noexcept
{
    MetaData_T* otherInfo = (MetaData_T*)otherData;

    // Make sure we don't compare past the m_data storage
    if ( otherInfo->elemIndex + otherInfo->numElements > m_numElements )
    {
        otherInfo->numElements = m_numElements - otherInfo->elemIndex;
    }

    uint8_t* arrayStartPtr = (uint8_t*)m_dataPtr;
    return memcmp( &( arrayStartPtr[otherInfo->elemIndex * m_elementSize] ), otherInfo->elemPtr, otherInfo->numElements * m_elementSize ) == 0;
}

/////////////////////////////////////////////////////
size_t ArrayBase::getInternalDataSize_() const noexcept
{
    return getSize() + sizeof( m_numElements ) + sizeof( m_elementSize );
}

bool ArrayBase::importMetadata_( const void* srcDataStream, size_t& bytesConsumed ) noexcept
{
    // NOTE: Use memcpy instead of the assignment operator since the alignment of 'srcDataStream' is unknown/not-guaranteed
    uint8_t* incoming = (uint8_t*)srcDataStream;
    size_t   incomingNumElements;
    size_t   incomingElementSize;
    memcpy( &incomingNumElements, incoming, sizeof( incomingNumElements ) );
    memcpy( &incomingElementSize, incoming + sizeof( incomingNumElements ), sizeof( incomingElementSize ) );

    // ArrayBase size and element sizes MUST match!
    if ( incomingNumElements != m_numElements || incomingElementSize != m_elementSize )
    {
        return false;
    }

    // No additional actions required
    bytesConsumed = sizeof( incomingNumElements ) + sizeof( incomingElementSize );
    return true;
}

bool ArrayBase::exportMetadata_( void* dstDataStream, size_t& bytesAdded ) const noexcept
{
    // NOTE: Use memcpy instead of the assignment operator since the alignment of 'dstDataStream' is unknown/not-guaranteed
    uint8_t* outgoing = (uint8_t*)dstDataStream;
    memcpy( outgoing, &m_numElements, sizeof( m_numElements ) );
    memcpy( outgoing + sizeof( m_numElements ), &m_elementSize, sizeof( m_elementSize ) );
    bytesAdded = sizeof( m_numElements ) + sizeof( m_elementSize );
    return true;
}

}  // end namespace
}
}
//------------------------------------------------------------------------------