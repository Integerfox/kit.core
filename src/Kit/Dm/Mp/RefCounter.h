#ifndef KIT_DM_MP_REFCOUNTER_H_
#define KIT_DM_MP_REFCOUNTER_H_
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


/** This class provides a concrete implementation for a Point who's data is 32
    bit unsigned reference counter.  A reference counter can be increment or
    decremented.  When incremented, the value is clamped at 2^32 -1 (not allowed
    to overflow).  When decremented, the value is clamped at zero (not allowed
    to underflow).  In addition, change notifications are only generated on the
    following transitions:

        1) Invalid state to the Valid state
        2) Valid state to the Invalid state
        3) Transition to zero
        4) Transition from zero to value greater than zero

    The toJSON() format is:
        \code

        { name:"<mpname>", type:"<mptypestring>", valid:true|false, seqnum:nnnn, locked:true|false,
          val:<numvalue>
        }

        \endcode

    The "val" format for the fromJSON() format is:
        \code

        val:"[<act>]<numvalue>
            where <act> can be:
            "+"             -->increment the counter
            "-"             -->decrement the counter
            (no prefix)     -->reset the counter to the specified value

        Examples:
                { name:"mp_visitors", val:"+2" }    // Increments the point by 2
                { name:"mp_visitors", val:"-1" }    // Decrements the point by 1
                { name:"mp_visitors", val:0 }       // Resets the counter to zero

        \endcode

    NOTE: All methods in this class ARE thread Safe unless explicitly
          documented otherwise.
 */
class RefCounter : public PrimitiveType<uint32_t, RefCounter>
{
public:
    /// Constructor. Invalid MP.
    RefCounter( Kit::Dm::IModelDatabase& myModelBase, const char* symbolicName )
        : PrimitiveType<uint32_t, RefCounter>( myModelBase, symbolicName )
    {
    }

    /// Constructor. Valid MP.  Requires an initial value
    RefCounter( Kit::Dm::IModelDatabase& myModelBase, const char* symbolicName, uint32_t initialValue )
        : PrimitiveType<uint32_t, RefCounter>( myModelBase, symbolicName, initialValue )
    {
    }


public:
    /// Increments the counter.  Note: The counter is protected from overflowing
    uint16_t increment( uint32_t      incrementAmount         = 1,
                        bool          forceChangeNotification = false,
                        LockRequest_T lockRequest             = eNO_REQUEST ) noexcept;

    /// Decrements the counter. Note: the counter is protected from underflowing
    uint16_t decrement( uint32_t      decrementAmount         = 1,
                        bool          forceChangeNotification = false,
                        LockRequest_T lockRequest             = eNO_REQUEST ) noexcept;


    /// Resets the counter to zero (or to a specific value). Alias for write()
    uint16_t reset( uint32_t      newValue                = 0,
                    bool          forceChangeNotification = false,
                    LockRequest_T lockRequest             = eNO_REQUEST ) noexcept;

    ///  See Kit::Dm::ModelPoint.
    const char* getTypeAsText() const noexcept override
    {
        return "Kit::Dm::Mp::RefCounter";
    }


public:
    /// See Kit::Dm::Point.
    bool fromJSON_( JsonVariant& src, LockRequest_T lockRequest, uint16_t& retSequenceNumber, Kit::Text::IString* errorMsg ) noexcept override;

protected:
    /// See Kit::Dm::Point.
    bool setJSONVal( JsonDocument& doc ) noexcept override;

    /// Helper method for only generating change notification on certain transitions
    void updateAndCheckForChangeNotification( uint32_t newValue );

private:
    /// write() is not accessible - use reset(), increment(), or decrement() instead
    using PrimitiveType<uint32_t, RefCounter>::write;
};


}  // end namespaces
}
}
#endif  // end header latch
