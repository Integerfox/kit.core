#ifndef KIT_DM_MP_UINT64_H_
#define KIT_DM_MP_UINT64_H_
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
#include <stdint.h>

///
namespace Kit {
///
namespace Dm {
///
namespace Mp {


/** This class provides a concrete implementation for a Point who's data is a
    uint64_t.

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
class Uint64 : public UnsignedInteger<uint64_t, Uint64>
{
public:
    /** Constructor. Invalid MP.
     */
    Uint64( Kit::Dm::IModelDatabase& myModelBase, const char* symbolicName )
        : UnsignedInteger<uint64_t, Uint64>( myModelBase, symbolicName )
    {
    }

    /// Constructor. Valid MP.  Requires an initial value
    Uint64( Kit::Dm::IModelDatabase& myModelBase, const char* symbolicName, uint64_t initialValue )
        : UnsignedInteger<uint64_t, Uint64>( myModelBase, symbolicName, initialValue )
    {
    }

public:
    ///  See Kit::Dm::ModelPoint.
    const char* getTypeAsText() const noexcept
    {
        return "Kit::Dm::Mp::Uint64";
    }
};


}       // end namespaces
}
}
#endif  // end header latch
