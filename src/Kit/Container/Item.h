#ifndef KIT_CONTAINER_ITEM_H_
#define KIT_CONTAINER_ITEM_H_
/*-----------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */


///
namespace Kit {
///
namespace Container {


/** This class is used by the Container classes to implement a various types
    of singly linked containers.

    Note: Client code, i.e. application code that needs to define a
          'containerized' class only needs to inherit from this
          interface.  The Client code SHOULD/SHALL NOT access any of its
          members or methods!  These members/methods are intended to ONLY be
          accessible by the container classes.
 */
class Item
{
protected:
    /// Constructor
    Item()
        : m_nextPtr_( nullptr ), m_inListPtr_( nullptr ) {}

    /** Constructor used ONLY with the child class MapItem: -->special
        constructor to allow a Map to be statically allocated.  Only the Map
        itself should ever use this constructor -->not intended for Items in a
        Map
     */
    Item( const char* /* ignoreThisParameter_usedToCreateAUniqueConstructor */ ) {}


public:
    /** Helper method to trap when inserting an item in multiple containers.
        A fatal error will be generated if 'Item' is attempted to be put into
        more than on container.

        Notes:
            o The method returns false when there is error - which is ONLY
              usefully during unittesting when the invoking a FatalError
              does NOT terminate the application.
     */
    bool insert_( void* newContainerPtr );

    /// Returns 'true' if the instance is in the specified container.
    inline bool isInContainer_( const void* containerPtr ) const noexcept
    {
        if ( m_inListPtr_ != containerPtr )
        {
            return false;
        }
        return true;
    }

    /** Verifies that the item is in the list for use when the next() method is 
        called.  If the item is not in the list, a fatal error is generated.

        Notes:
            o The method returns false when there is error - which is ONLY
              usefully during unittesting when the invoking a FatalError
              does NOT terminate the application.
     */
    bool validateNextOkay_( const void* containerPtr ) const noexcept;

    /** Helper method to do the proper 'clean-up' for the multiple-containers-error-trap 
        when removing an item from a container.
     */
    inline static void remove_( Item* itemPtr ) noexcept
    {
        if ( itemPtr )
        {
            itemPtr->m_inListPtr_ = nullptr;
        }
    }

public:
    /// The link field.
    void* m_nextPtr_;

    /** Debug field.  This member is used to trap when there is an attempt
        to insert a item into a container when it is already in a container
     */
    void* m_inListPtr_;
};

/** This class is used by the Container classes
    to implement a various types of DOUBLY linked
    containers.

    Note: Client code, i.e. application code that needs to define a
          'containerized' class only needs to inherit from this
          interface.  The Client code SHOULD/SHALL NOT access any of its
          members or methods!  These members/methods are intended to ONLY be
          accessible by the container classes.
 */

class ExtendedItem : public Item
{
public:
    /// The previous link field.
    void* m_prevPtr_;

protected:
    /// Constructor
    ExtendedItem()
        : m_prevPtr_( nullptr ) {}

    /** Constructor used ONLY with the child class MapItem: -->special
        constructor to allow a Map to be statically allocated.  Only the Map
        itself should ever use this constructor -->not intended for Items in a
        Map
     */
    ExtendedItem( const char* ignoreThisParameter_usedToCreateAUniqueConstructor )
        : Item( ignoreThisParameter_usedToCreateAUniqueConstructor ) {}
};


/** This template class defines wrapper class - that is makes a reference
    'listable'. This class is useful when the Application needs to put a
    single entity into multiple containers.

    Template Arguments:
        REFITEM    - The type of the Reference being wrapped.
        ITEMTYPE   - The Item/Container type, e.g. Item, ExtendedItem, MapItem, etc.
 */
template <class REFITEM, class ITEMTYPE>
class ReferenceItem : public ITEMTYPE
{
public:
    /// Reference to the item that is being 'containerized'
    REFITEM& m_reference;

    /// Constructor
    ReferenceItem( REFITEM& item )
        : ITEMTYPE(), m_reference( item ) {}

    /** Constructor used ONLY with the child class MapItem: -->special
        constructor to allow a Map to be statically allocated.  Only the Map
        itself should ever use this constructor -->not intended for Items in a
        Map
     */
    ReferenceItem( REFITEM& item, const char* ignoreThisParameter_usedToCreateAUniqueConstructor )
        : ITEMTYPE( ignoreThisParameter_usedToCreateAUniqueConstructor ), m_reference( item ) {}
};

}  // end namespaces
}
#endif  // end header latch
