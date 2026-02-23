#ifndef KIT_DM_MP_FLOAT_H_
#define KIT_DM_MP_FLOAT_H_
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
#include "Kit/Math/real.h"

///
namespace Kit {
///
namespace Dm {
///
namespace Mp {


/** This class provides a concrete implementation for a Point who's data is a
    float.

    The toJSON() format is:
        \code

        { name:"<mpname>", type:"<mptypestring>", valid:true|false, seqnum:nnnn, locked:true|false, 
          val:<floatvalue>}

        \endcode

    The "val" format for the fromJSON() format is:
        \code

        val:<floatvalue>

        \endcode

    NOTE: All methods in this class ARE thread Safe unless explicitly
          documented otherwise.
 */
class Float : public NumericBase<float, Float>
{
public:
    /** Constructor. Invalid MP.
     */
    Float( Kit::Dm::IModelDatabase& myModelBase, const char* symbolicName )
        : NumericBase<float, Float>( myModelBase, symbolicName )
    {
    }

    /// Constructor. Valid MP.  Requires an initial value
    Float( Kit::Dm::IModelDatabase& myModelBase, const char* symbolicName, float initialValue )
        : NumericBase<float, Float>( myModelBase, symbolicName, initialValue )
    {
    }

public:
    ///  See Kit::Dm::ModelPoint.
    const char* getTypeAsText() const noexcept
    {
        return "Kit::Dm::Mp::Float";
    }

protected:
    /// Override parent implementation for 'correct' floating point comparison
    bool isDataEqual_( const void* otherData ) const noexcept
    {
        const float* other = static_cast<const float*>(otherData);
        return Kit::Math::areFloatsEqual( m_data, *other );
    }
};



}       // end namespaces
}
}
#endif  // end header latch
