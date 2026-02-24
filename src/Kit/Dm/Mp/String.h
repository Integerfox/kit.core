#ifndef KIT_DM_MP_STRING_H_
#define Kit_Dm_Mp_String_h_
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


/** This mostly concrete class provides the base implementation for a Point
    who's data is a null terminated string.  The concrete child class is
    responsible for providing the string storage

    The toJSON() format is:
        \code

        { name:"<mpname>", type:"<mptypestring>", valid:true|false, seqnum:nnnn, locked:true|false,
          val:"<stringvalue>"
        }

        \endcode

    The "val" format for the fromJSON() format is:
        \code

        val:"<stringvalue>"

        \endcode
    NOTE: All methods in this class ARE thread Safe unless explicitly
          documented otherwise.

    NOTE: The MP's null terminator for the string storage IS imported/exported.
 */
class String : public Kit::Dm::ModelPointBase
{
protected:
    /** Constructor. Invalid MP.
     */
    String( Kit::Dm::IModelDatabase& myModelBase,
                const char*              symbolicName,
                char*                    myDataPtr,
                size_t                   dataSizeInBytesIncludingNullTerminator );

    /// Constructor. Valid MP.  Requires an initial value
    String( Kit::Dm::IModelDatabase& myModelBase,
                const char*              symbolicName,
                char*                    myDataPtr,
                size_t                   dataSizeInBytesIncludingNullTerminator,
                const char*              initialValue );


public:
    /// Type safe read. See Kit::Dm::ModelPoint
    bool read( Kit::Text::IString& dstData, uint16_t* seqNumPtr = 0 ) const noexcept;

    /// Type safe read. See Kit::Dm::ModelPoint
    bool read( char*     dstData,
               size_t    dataSizeInBytesIncludingNullTerminator,
               uint16_t* seqNumPtr = 0 ) const noexcept;

    /// Type safe write of a null terminated string. See Kit::Dm::ModelPoint
    inline uint16_t write( const char*   srcNullTerminatedString,
                           bool          forceChangeNotification = false,
                           LockRequest_T lockRequest             = eNO_REQUEST ) noexcept
    {
        return write( srcNullTerminatedString, strlen( srcNullTerminatedString ), forceChangeNotification, lockRequest );
    }

    /// Same as write(), except only writes at most 'srcLen' bytes, 'srcLen' should NOT include the null terminator.
    uint16_t write( const char*   srcData,
                    size_t        srcLen,
                    bool          forceChangeNotification = false,
                    LockRequest_T lockRequest             = eNO_REQUEST ) noexcept;

    /// Returns the maximum size WITHOUT the null terminator of the string storage
    inline size_t getMaxLength() const noexcept
    {
        return m_dataSize - 1;
    }

    /// Updates the MP with the valid-state/data from 'src'. Note: the src.lock state is NOT copied
    inline uint16_t copyFrom( const String& src, LockRequest_T lockRequest = eNO_REQUEST ) noexcept
    {
        return copyDataAndStateFrom( src, lockRequest );
    }

    ///  See Kit::Dm::ModelPoint.
    const char* getTypeAsText() const noexcept override
    {
        return "Kit::Dm::Mp::String";
    }

public:
    /// Type safe register observer
    inline void attach( Kit::Dm::Observer<String>& observer,
                        uint16_t                       initialSeqNumber = SEQUENCE_NUMBER_UNKNOWN ) noexcept
    {
        attachObserver( observer, initialSeqNumber );
    }

    /// Type safe un-register observer
    inline void detach( Kit::Dm::Observer<String>& observer ) noexcept
    {
        detachObserver( observer );
    }

public:
    /// See Kit::Dm::ModelPointCommon
    inline bool readAndSync( Kit::Text::IString& dstData, IObserver& observerToSync )
    {
        uint16_t seqNum;
        return readAndSync( dstData, seqNum, observerToSync );
    }

    /// See Kit::Dm::ModelPointCommon
    inline bool readAndSync( Kit::Text::IString& dstData,
                             uint16_t&           seqNum,
                             IObserver&          observerToSync )
    {
        bool result = read( dstData, &seqNum );
        attachObserver( observerToSync, seqNum );
        return result;
    }

    /// See Kit::Dm::ModelPointCommon
    inline bool readAndSync( char* dstData, size_t dataSizeInBytesIncludingNullTerminator, IObserver& observerToSync )
    {
        uint16_t seqNum;
        return readAndSync( dstData, dataSizeInBytesIncludingNullTerminator, seqNum, observerToSync );
    }

    /// See Kit::Dm::ModelPointCommon
    inline bool readAndSync( char*      dstData,
                             size_t     dataSizeInBytesIncludingNullTerminator,
                             uint16_t&  seqNum,
                             IObserver& observerToSync )
    {
        bool result = read( dstData, dataSizeInBytesIncludingNullTerminator, &seqNum );
        attachObserver( observerToSync, seqNum );
        return result;
    }

public:
    /// See Kit::Dm::Point.
    bool fromJSON_( JsonVariant& src, LockRequest_T lockRequest, uint16_t& retSequenceNumber, Kit::Text::IString* errorMsg ) noexcept override;

    /// See Kit::Dm::Point.
    bool isDataEqual_( const void* otherData ) const noexcept override;

protected:
    /// See Kit::Dm::Point.
    bool setJSONVal( JsonDocument& doc ) noexcept override;
};


/** This concrete template class provides the storage for a Point
    who's data is a null terminated string.

    Template Args:
        S:=      Max Size of the String WITHOUT the null terminator!
 */
template <int S>
class StringAllocate : public String
{
public:
    /** Constructor. Invalid Point.
     */
    StringAllocate( Kit::Dm::IModelDatabase& myModelBase, const char* symbolicName )
        : String( myModelBase, symbolicName, m_data, sizeof( m_data ) )
    {
    }

    /// Constructor. Valid Point.  Requires an initial value
    StringAllocate( Kit::Dm::IModelDatabase& myModelBase, const char* symbolicName, const char* initialValue )
        : String( myModelBase, symbolicName, m_data, sizeof( m_data ), initialValue )
    {
    }

protected:
    /// The MP's raw storage
    char m_data[S + 1];
};


}       // end namespaces
}
}
#endif  // end header latch
