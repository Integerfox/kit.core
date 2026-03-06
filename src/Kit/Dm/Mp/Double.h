#ifndef KIT_DM_MP_DOUBLE_H_
#define KIT_DM_MP_DOUBLE_H_
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
    double.

    The toJSON() format is:
        \code

        { name:"<mpname>", type:"<mptypestring>", valid:true|false, seqnum:nnnn, locked:true|false, 
          val:<doublevalue>}

        \endcode

    The "val" format for the fromJSON() format is:
        \code

        val:<doublevalue>

        \endcode

    NOTE: All methods in this class ARE thread Safe unless explicitly
          documented otherwise.
 */
class Double : public NumericBase<double, Double>
{
public:
    /** Constructor. Invalid MP.
     */
    Double( Kit::Dm::IModelDatabase& myModelBase, const char* symbolicName )
        : NumericBase<double, Double>( myModelBase, symbolicName )
    {
    }

    /// Constructor. Valid MP.  Requires an initial value
    Double( Kit::Dm::IModelDatabase& myModelBase, const char* symbolicName, double initialValue )
        : NumericBase<double, Double>( myModelBase, symbolicName, initialValue )
    {
    }

public:
    ///  See Kit::Dm::ModelPoint.
    const char* getTypeAsText() const noexcept
    {
        return "Kit::Dm::Mp::Double";
    }

protected:
    /// Override parent implementation for 'correct' double-precision floating-point comparison
    bool isDataEqual_( const void* otherData ) const noexcept
    {
        const double* other = static_cast<const double*>(otherData);
        return Kit::Math::areDoublesEqual( m_data, *other );
    }
};



}       // end namespaces
}
}
#endif  // end header latch
