/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */


#include "String.h"
#include "Kit/Dm/ModelPointBase.h"
#include <string.h>


//------------------------------------------------------------------------------
namespace Kit {
namespace Dm {
namespace Mp {

///////////////////////////////////////////////////////////////////////////////
String::String( Kit::Dm::IModelDatabase& myModelBase,
                const char*              symbolicName,
                char*                    myDataPtr,
                size_t                   dataSizeInBytesIncludingNullTerminator )
    : Kit::Dm::ModelPointBase( myModelBase, symbolicName, myDataPtr, dataSizeInBytesIncludingNullTerminator, false )
{
    // Clear the entire string INCLUDING the null terminator
    memset( (void*)myDataPtr, 0, dataSizeInBytesIncludingNullTerminator );
}

/// Constructor. Valid MP.  Requires an initial value
String::String( Kit::Dm::IModelDatabase& myModelBase,
                const char*              symbolicName,
                char*                    myDataPtr,
                size_t                   dataSizeInBytesIncludingNullTerminator,
                const char*              initialValue )
    : Kit::Dm::ModelPointBase( myModelBase, symbolicName, myDataPtr, dataSizeInBytesIncludingNullTerminator, true )
{
    // Set the initial value
    strncpy( myDataPtr, initialValue, dataSizeInBytesIncludingNullTerminator );

    // Make sure that the last byte of the raw storage is the NULL TERMINATOR
    myDataPtr[dataSizeInBytesIncludingNullTerminator - 1] = '\0';
}

///////////////////////////////////////////////////////////////////////////////
bool String::read( Kit::Text::IString& dstData, uint16_t* seqNumPtr ) const noexcept
{
    int   bufferMaxLength;
    char* dstStringPtr = dstData.getBuffer( bufferMaxLength );
    return read( dstStringPtr, bufferMaxLength + 1, seqNumPtr );  // Note: the 'bufferMaxLength' returned does NOT include space for the null terminator
}

bool String::read( char* dstData, size_t dataSizeInBytesIncludingNullTerminator, uint16_t* seqNumPtr ) const noexcept
{
    // Max sure the length does not exceed the MP's 'string' storage
    if ( dataSizeInBytesIncludingNullTerminator > m_dataSize )
    {
        dataSizeInBytesIncludingNullTerminator = m_dataSize;
    }

    // Ensure the returned result is always null terminated
    bool valid                                          = readData( dstData, dataSizeInBytesIncludingNullTerminator, seqNumPtr );
    dstData[dataSizeInBytesIncludingNullTerminator - 1] = '\0';
    return valid;
}

uint16_t String::write( const char*   srcData,
                        size_t        srcLen,
                        bool          forceChangeNotification,
                        LockRequest_T lockRequest ) noexcept
{
    // Trap the null pointer case -->Do NOTHING
    if ( srcData == 0 )
    {
        return getSequenceNumber();
    }

    // Max sure the length does not exceed the MP's 'string' storage
    if ( srcLen > m_dataSize - 1 )  // Note: the 'srcLen' should NOT include the null terminator
    {
        srcLen = m_dataSize - 1;
    }

    Kit::System::Mutex::ScopeLock criticalSection( Kit::Dm::ModelPointBase::m_modelDatabase.getMutex_() );
    uint16_t                      seqNum    = writeData( srcData, srcLen, lockRequest );
    char*                         myDataPtr = static_cast<char*>( m_dataPtr );
    myDataPtr[srcLen]                       = '\0';  // Ensure my new value properly null terminated
    return seqNum;
}

bool String::isDataEqual_( const void* otherData ) const noexcept
{
    const char* otherStringPtr = static_cast<const char*>( otherData );
    size_t      otherLen       = strlen( otherStringPtr );
    size_t      myLen          = strlen( static_cast<const char*>( m_dataPtr ) );
    return otherLen == myLen && strncmp( otherStringPtr, static_cast<const char*>( m_dataPtr ), myLen ) == 0;
}

bool String::setJSONVal( JsonDocument& doc ) noexcept
{
    // Create value object
    JsonObject valObj = doc.createNestedObject( "val" );

    // Construct the 'val' key/value pair
    valObj["maxLen"] = getMaxLength();
    valObj["text"]   = (char*)m_dataPtr;
    return true;
}

bool String::fromJSON_( JsonVariant&        src,
                        LockRequest_T       lockRequest,
                        uint16_t&           retSequenceNumber,
                        Kit::Text::IString* errorMsg ) noexcept
{
    // Note: Max size is ignored, i.e. do NOT support reallocating sizes via JSON

    // Get the string
    const char* newValue = src["text"];
    if ( newValue == nullptr )
    {
        if ( errorMsg )
        {
            *errorMsg = "Invalid syntax for the 'val' key/value pair";
        }
        return false;
    }

    // Note: if the incoming string is longer than the MP storage, it will be truncated
    retSequenceNumber = write( newValue, false, lockRequest );
    return true;
}

}  // end namespace
}
}
//------------------------------------------------------------------------------