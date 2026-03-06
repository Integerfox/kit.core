#ifndef KIT_DM_MP_INT16_H_
#define KIT_DM_MP_INT16_H_
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
    int16_t.

    The toJSON() format is:
        \code

        { name:"<mpname>", type:"<mptypestring>", valid:true|false, seqnum:nnnn, locked:true|false, 
          val:{ "dec":<numvalue>,
                "hex":<hexvalue>
              }
        }

        \endcode

    The "val" format for the fromJSON() format is:
        \code

        val:<numvalue> OR val:"0x<hexvalue>"

        \endcode

    NOTE: All methods in this class ARE thread Safe unless explicitly
          documented otherwise.
 */
class Int16 : public SignedInteger<int16_t, Int16>
{
public:
    /** Constructor. Invalid MP.
     */
    Int16( Kit::Dm::IModelDatabase& myModelBase, const char* symbolicName )
        : SignedInteger<int16_t, Int16>( myModelBase, symbolicName )
    {
    }

    /// Constructor. Valid MP.  Requires an initial value
    Int16( Kit::Dm::IModelDatabase& myModelBase, const char* symbolicName, int16_t initialValue )
        : SignedInteger<int16_t, Int16>( myModelBase, symbolicName, initialValue )
    {
    }

public:
    ///  See Kit::Dm::ModelPoint.
    const char* getTypeAsText() const noexcept
    {
        return "Kit::Dm::Mp::Int16";
    }
};


}       // end namespaces
}
}
#endif  // end header latch
