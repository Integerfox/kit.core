#ifndef Cpl_Dm_Mp_Uint32_h_
#define Cpl_Dm_Mp_Uint32_h_
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
#include <stdint.h>

///
namespace Kit {
///
namespace Dm {
///
namespace Mp {


/** This class provides a concrete implementation for a Point who's data is a
    uint32_t.

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
class Uint32 : public UnsignedInteger<uint32_t, Uint32>
{
public:
    /** Constructor. Invalid MP.
     */
    Uint32( Kit::Dm::IModelDatabase& myModelBase, const char* symbolicName )
        : UnsignedInteger<uint32_t, Uint32>( myModelBase, symbolicName )
    {
    }

    /// Constructor. Valid MP.  Requires an initial value
    Uint32( Kit::Dm::IModelDatabase& myModelBase, const char* symbolicName, uint32_t initialValue )
        : UnsignedInteger<uint32_t, Uint32>( myModelBase, symbolicName, initialValue )
    {
    }

public:
    ///  See Kit::Dm::ModelPoint.
    const char* getTypeAsText() const noexcept
    {
        return "Kit::Dm::Mp::Uint32";
    }
};


}       // end namespaces
}
}
#endif  // end header latch
