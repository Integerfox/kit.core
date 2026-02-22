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


#include "Kit/Dm/Mp/NumericBase.h"

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
          val:"<hexValueWithNoLeading0xPrefix>" }

        \endcode

    The "val" format for the fromJSON() format is below.  NOTE: setting a Pointer
    value from JSON is not something that is likely to be useful in general (i.e
    DON'T do it), but it is supported for completeness.
        \code

        val:"<hexValueWithNoLeading0xPrefix>"

        \endcode

    NOTE: All methods in this class ARE thread Safe unless explicitly
          documented otherwise.
 */
class Void : public Numeric<void*, Void>
{
public:
    /// Constructor. Invalid MP.
    Void( Kit::Dm::IModelDatabase& myModelBase, const char* symbolicName )
        : Numeric<void*, Void>( myModelBase, symbolicName )
    {
    }

    /// Constructor. Valid MP.  Requires an initial value
    Void( Kit::Dm::IModelDatabase& myModelBase, const char* symbolicName, void* initialValue )
        : Numeric<void*, Void>( myModelBase, symbolicName, initialValue )
    {
    }


public:
    ///  See Kit::Dm::ModelPoint.
    const char* getTypeAsText() const noexcept
    {
        return "Kit::Dm::Mp::Void*";
    }

protected:
    /** See Kit::Dm::Point. Note: NO loading '0x' prefix
        \code
            Output: val:"<hexadecimal value>"
        \endcode
     */
    bool setJSONVal( JsonDocument& doc ) noexcept
    {
        Kit::Text::FString<20> hexString;
        hexString.format( "%" PRIXPTR, (uintptr_t)Numeric<void*, Void>::m_data );
        doc["val"] = (char*)hexString.getString();
        return true;
    }

public:
    /** See Kit::Dm::Point. Note: NO loading '0x' prefix
        \code
            Input: val:"<hexvalue>"
            For example:
                { "val": "7B" }
        \endcode
     */
    bool fromJSON_( JsonVariant& src, Kit::Dm::IModelPoint::LockRequest_T lockRequest, uint16_t& retSequenceNumber, Kit::Text::IString* errorMsg ) noexcept
    {
        // Parse as a hex string (e.g. "0x12")
        if ( src.is<const char*>() )
        {
            // Try to parse the string as a number
            size_t val;
            if ( Kit::Text::StringTo::unsignedInt( val, src.as<const char*>(), 16 ) )
            {
                retSequenceNumber = this->write( (void*)val, false, lockRequest );
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
