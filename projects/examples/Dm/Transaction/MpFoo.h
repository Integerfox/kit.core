#ifndef DM_TRANSACTION_MP_FOO_H_
#define DM_TRANSACTION_MP_FOO_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Dm/Transaction/FooT.h"
#include "Kit/Dm/Mp/Scalar.h"

///
namespace Dm {
///
namespace Transaction {

/** This concreate class implements a type safe MP for the data structure: Foo_T

    The toJSON() format is:
        ```
        { name:"<mpname>", type:"<mptypestring>", valid:true|false, seqnum:nnnn, locked:true|false,
          val: {
            "upper":nnnn, "lower":nnnn, "result":nnnn, "isValid":true|false
          }
        }
        ```

    The "val" format for the fromJSON() format is:
        ```
        val: {"upper":nnnn, "lower":nnnn, "result":nnnn, "isValid":true|false }
        ```

    NOTES:
    - The parent class (Kit::Dm::Mp::Scalar) provides the implementation
      for all of the standard MP operations (e.g. read, write, attachObserver,
      detachObserver, etc).

    - The MpFoo class must provide the toJSON() and fromJSON() methods for the
      Foo_T structure

    - The MpFoo class provides additional "write" operations to enforce its
      intended transaction paradigm.

    - The toJSON() method supports only updating the specified/included KVPs,
      e.g. "val":{"upper":12}" - is a read/modify operation that only updates
      the 'upperLimit' field of the MP's data structure.

*/
class MpFoo : public Kit::Dm::Mp::Scalar<Foo_T, MpFoo>
{
public:
    /// Constructor. Invalid MP.
    MpFoo( Kit::Dm::IModelDatabase& myModelBase, const char* symbolicName )
        : Kit::Dm::Mp::Scalar<Foo_T, MpFoo>( myModelBase, symbolicName )
    {
    }

    /// Constructor. Valid MP.  Requires an initial value.  A copy is made of the 'initialValue' argument.
    MpFoo( Kit::Dm::IModelDatabase& myModelBase, const char* symbolicName, const Foo_T& initialValue )
        : Kit::Dm::Mp::Scalar<Foo_T, MpFoo>( myModelBase, symbolicName, initialValue )
    {
    }

public:
    /// Convenience method to trigger a Server request
    inline uint16_t triggerRequest( int32_t                             upperLimit,
                                    int32_t                             lowerLimit,
                                    bool                                forceChangeNotification = false,
                                    Kit::Dm::IModelPoint::LockRequest_T lockRequest             = Kit::Dm::IModelPoint::eNO_REQUEST ) noexcept
    {
        Foo_T newData = { upperLimit, lowerLimit, 0, false };
        return write( newData, forceChangeNotification, lockRequest );
    }

public:
    /// See Kit::Dm::ModelPoint.
    const char* getTypeAsText() const noexcept override
    {
        return "Dm::Transaction::Mp::MpFoo";
    }

protected:
    /// See Kit::Dm::Point.
    bool setJSONVal( JsonDocument& doc ) noexcept override
    {
        JsonObject obj = doc.createNestedObject( "val" );
        obj["upper"]   = this->m_data.upperLimit;
        obj["lower"]   = this->m_data.lowerLimit;
        obj["result"]  = this->m_data.randomNumber;
        obj["isValid"] = this->m_data.isValid;
        return true;
    }

public:
    /// See Kit::Dm::Point.
    bool fromJSON_( JsonVariant& src, LockRequest_T lockRequest, uint16_t& retSequenceNumber, Kit::Text::IString* errorMsg ) noexcept override
    {
        // Start with the current data and update only the fields that are
        // included in the JSON document.  No explicit error checking is done.
        // Semantic failures are silently ignored with no update to the struct fields.
        Foo_T newData = this->m_data;

        // Parse the fields
        if ( src["upper"].is<int32_t>() )
        {
            newData.upperLimit = src["upper"].as<int32_t>();
        }
        if ( src["lower"].is<int32_t>() )
        {
            newData.lowerLimit = src["lower"].as<int32_t>();
        }
        if ( src["result"].is<int32_t>() )
        {
            newData.randomNumber = src["result"].as<int32_t>();
        }
        if ( src["isValid"].is<bool>() )
        {
            newData.isValid = src["isValid"].as<bool>();
        }

        retSequenceNumber = write( newData, lockRequest );
        return true;
    }
};

}  // end namespaces
}
#endif  // end header latch
