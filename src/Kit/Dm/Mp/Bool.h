#ifndef KIT_DM_MP_BOOL_H_
#define KIT_DM_MP_BOOL_H_
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

///
namespace Kit {
///
namespace Dm {
///
namespace Mp {


/** This class provides a concrete implementation for a Point who's data is a
    bool.

    The toJSON() format is:
        \code

        { name:"<mpname>", type:"<mptypestring>", valid:true|false, seqnum:nnnn, locked:true|false,
          val:true|false}

        \endcode

    The "val" format for the fromJSON() format is:
        \code

        val:true|false

        \endcode

    NOTE: All methods in this class ARE thread Safe unless explicitly
          documented otherwise.
 */
class Bool : public PrimitiveType<bool, Bool>
{
public:
    /// Constructor. Invalid MP.
    Bool( Kit::Dm::IModelDatabase& myModelBase, const char* symbolicName )
        : PrimitiveType<bool, Bool>( myModelBase, symbolicName )
    {
    }

    /// Constructor. Valid MP.  Requires an initial value
    Bool( Kit::Dm::IModelDatabase& myModelBase, const char* symbolicName, bool initialValue )
        : PrimitiveType<bool, Bool>( myModelBase, symbolicName, initialValue )
    {
    }

public:
    const char* getTypeAsText() const noexcept override
    {
        return "Kit::Dm::Mp::Bool";
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
        if ( src.is<bool>() )
        {
            retSequenceNumber = write( src.as<bool>(), lockRequest );
            return true;
        }
        if ( errorMsg )
        {
            *errorMsg = "Invalid syntax for the 'val' key/value pair";
        }
        return false;
    }
};

}       // end namespaces
}
}
#endif  // end header latch
