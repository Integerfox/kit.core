#ifndef KIT_CONTAINER_ORDEREDLISTBASE_H_
#define KIT_CONTAINER_ORDEREDLISTBASE_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Container/DListBase.h"
#include "Kit/Container/KeyedItem.h"

///
namespace Kit {
///
namespace Container {

/** This protected base class implements a NON-TYPE-SAFE an ordered doubly
    linked list.  The order is determined by the compare of function of the
    'Keyed Item' being stored in the list.  It is intended to be the base class
    for a type-safe template class. Having a non-templatized base class reduces 
    the memory footprint when an application has more than one type specific
    OrderedListBase class.

    NOTE: The insert algorithm is STUPID. It is a brute force walk of the existing
          items in the list and inserting new item in order.  If performance with
          respect to ordering is an issue, you will need to use the Map container
          which is implemented as a binary tree.
 */
class OrderedListBase : public DListBase
{
protected:
    /// Constructor initializes head and tail pointers.
    OrderedListBase() noexcept
        : DListBase()
    {
    }

    /** See DListBase::DListBase(const char*) for details. 
     */
    OrderedListBase( const char* ignoreThisParameter_usedToCreateAUniqueConstructor ) noexcept
        : DListBase( ignoreThisParameter_usedToCreateAUniqueConstructor )
    {
    }

protected:
    /** Inserts the 'item' into the list in sorted order based on the
         compare function of the KeyedItem.
     */
    void insert( KeyedItem& item ) noexcept;

private:
    /// Prevent access to the copy constructor -->Containers can not be copied!
    OrderedListBase( const OrderedListBase& m ) = delete;

    /// Prevent access to the assignment operator -->Containers can not be copied!
    OrderedListBase& operator=( const OrderedListBase& m ) = delete;

    /// Prevent access to the move constructor -->Containers can not be implicitly moved!
    OrderedListBase( OrderedListBase&& m ) = delete;

    /// Prevent access to the move assignment operator -->Containers can not be implicitly moved!
    OrderedListBase& operator=( OrderedListBase&& m ) = delete;
};


}  // end namespaces
}
#endif  // end header latch