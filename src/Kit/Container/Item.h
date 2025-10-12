#ifndef KIT_CONTAINER_ITEM_H_
#define KIT_CONTAINER_ITEM_H_
/*------------------------------------------------------------------------------
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


/** This class is used by the Intrusive Container classes to enforce the 
    semantic and item/element can be in at most ONE container at any given
    time.  Intrusive containers are 'limitless' containers in that can
    hold any number of elements because each element contains the 'container-
    linkage' fields, i.e. no dynamic memory required to added an element to
    the container.

    NOTE: Enforcing the above semantics adds RAM overhead to a Item class.  
          However, debugging a logic error where a single item is inserted into
          multiple containers at the same time IS EXTREMELY DIFFICULT to debug, 
          i.e. the extra overhead is worth it.

    NOTE: Client code, i.e. application code that needs to define a
          'containerized' class only needs to inherit from this
          interface.  The Client code SHOULD/SHALL NOT access any of its
          members or methods!  These members/methods are intended to ONLY be
          accessible by the container classes.
 */
class Item
{
protected:
    /// Constructor
    Item() noexcept;

    /** Constructor used ONLY with the child class MapItem: -->special
        constructor to allow a Map to be statically allocated.  Only the Map
        itself should ever use this constructor -->not intended for Items in a
        Map
     */
    Item( const char* /* ignoreThisParameter_usedToCreateAUniqueConstructor */ ) noexcept {}


public:
    /** Helper method to trap when inserting an item in multiple containers.
        A fatal error will be generated if 'Item' is attempted to be put into
        more than on container.

        Notes:
            o The method returns false when there is error - which is ONLY
              usefully during unittesting when the invoking a FatalError
              does NOT terminate the application.
     */
    bool insert_( void* newContainerPtr ) noexcept;

    /// Returns 'true' if the instance is in the specified container.
    bool isInContainer_( const void* containerPtr ) const noexcept;

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
    static void remove_( Item* itemPtr ) noexcept;

public:
    /** Debug field.  This member is used to trap when there is an attempt
        to insert a item into a container when it is already in a container
     */
    void* m_inListPtr_;
};

}  // end namespaces
}
#endif  // end header latch
