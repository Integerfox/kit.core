#ifndef KIT_DM_MP_VOID_H_
#define KIT_DM_MP_VOID_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */


#include "Kit/Dm/Mp/PrimitiveType.h"
#include <cstdint>

///
namespace Kit {
///
namespace Dm {
///
namespace Mp {


/** This class provides a concrete implementation for a Point who's data is a
    void pointer.

    On general principle there should NOT be model point type that is generic
    (it goes against the whole 'model-points-are-type-safe' thingy). HOWEVER,
    there are some cases where it is desirable to have MP contain a pointer -
    where the actual definition of what that pointer points to is 'not visible'

    Shorter version: DO NOT USE THIS MODEL POINT TYPE UNLESS YOU HAVE NO
    ALTERNATIVE. Not wanting to take the time to create a new model point type
    (with test code) is NOT an acceptable excuse for using this model point
    type.

    The toJSON() format is:
        \code

        { name:"<mpname>", type:"<mptypestring>", valid:true|false, seqnum:nnnn, locked:true|false,
          val:"0x<hexValue" }

        \endcode

    The "val" format for the fromJSON() format is below.  NOTE: setting a Pointer
    value from JSON is not something that is likely to be useful in general (i.e
    DON'T do it), but it is supported for completeness.
        \code

        val:"0x<hexValue>"

        \endcode

    NOTE: All methods in this class ARE thread Safe unless explicitly
          documented otherwise.
 */
class Void : public PrimitiveType<uintptr_t, Void>
{
public:
    /// Constructor. Invalid MP.
    Void( Kit::Dm::IModelDatabase& myModelBase, const char* symbolicName )
        : PrimitiveType<uintptr_t, Void>( myModelBase, symbolicName )
    {
    }

    /// Constructor. Valid MP.  Requires an initial value
    Void( Kit::Dm::IModelDatabase& myModelBase, const char* symbolicName, void* initialValue )
        : PrimitiveType<uintptr_t, Void>( myModelBase, symbolicName, (uintptr_t)initialValue )
    {
    }

public:
    /** Type safe read. See Kit::Dm::IModelPoint. Note: the underlying data is
        a uintptr_t, but the read/write methods use void* for type safety.
     */
    inline bool read( void*& dstData, uint16_t* seqNumPtr = nullptr ) const noexcept
    {
        return Kit::Dm::ModelPointBase::readData( &dstData, sizeof( uintptr_t ), seqNumPtr );
    }

    /** Type safe write. See Kit::Dm::IModelPoint. Note: the underlying data is
        a uintptr_t, but the read/write methods use void* for type safety.
     */
    inline uint16_t write( void*                               newValue,
                           bool                                forceChgNotification = false,
                           Kit::Dm::IModelPoint::LockRequest_T lockRequest          = Kit::Dm::IModelPoint::eNO_REQUEST ) noexcept
    {
        return Kit::Dm::ModelPointBase::writeData( &newValue, sizeof( uintptr_t ), forceChgNotification, lockRequest );
    }

public:
    ///  See Kit::Dm::ModelPoint.
    const char* getTypeAsText() const noexcept override
    {
        return "Kit::Dm::Mp::Void*";
    }

protected:
    /** See Kit::Dm::Point. Note: NO loading '0x' prefix
        \code
            Output: val:"0x<hexadecimal value>"
        \endcode
     */
    bool setJSONVal( JsonDocument& doc ) noexcept override
    {
        Kit::Text::FString<20> hexString;
        hexString.format( "0x%" PRIXPTR, this->m_data );
        doc["val"] = (char*) hexString.getString();
        return true;
    }

public:
    /** See Kit::Dm::Point.
        \code
            Input: val:"0x<hexvalue>"
        \endcode
     */
    bool fromJSON_( JsonVariant& src, LockRequest_T lockRequest, uint16_t& retSequenceNumber, Kit::Text::IString* errorMsg ) noexcept override
    {
        // Parse as a hex string (e.g. "0xA12F")
        if ( src.is<const char*>() )
        {
            // Try to parse the string as a number
            uintptr_t val;
            if ( Kit::Text::StringTo::unsignedInt( val, src.as<const char*>(), 16 ) )
            {
                retSequenceNumber = PrimitiveType<uintptr_t, Void>::write( val, false, lockRequest );
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
