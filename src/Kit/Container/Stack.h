#ifndef KIT_CONTAINER_STACK_H_
#define KIT_CONTAINER_STACK_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include <stdint.h>


///
namespace Kit {
///
namespace Container {


/** This template class implements a Stack that has a fixed depth and stores
    copies of the data items.

    Template Args:
        ITEM:=      Type of the data stored in the Stack
 */
template <class ITEM>
class Stack
{
public:
    /** Constructor.  The application is responsible for providing the memory
        for the Stack.  The argument 'maxElements' is the number of items that
        will fit in the memory allocated by 'memoryForElements' - it is NOT
        the number of bytes of 'memoryForElements'.
     */
    Stack( unsigned maxElements, ITEM memoryForElements[] ) noexcept
        : m_elements( memoryForElements )
        , m_count( 0 )
        , m_maxItems( maxElements )
    {
    }


public:
    /** Adds an item to the top of the stack.  Returns true if successful;
        else false is returned (e.g. on stack overflow).

        NOTE: A copy of the item is what is pushed on the stack
     */
    bool push( const ITEM& src ) noexcept
    {
        if ( isFull() )
        {
            return false;
        }

        m_elements[m_count++] = src;
        return true;
    }


    /** Removes the top item of the stack.  If the stack is empty then the 
        method returns false and no value is returned.
     */
    bool pop( ITEM& dst ) noexcept
    {
        if ( isEmpty() )
        {
            return false;
        }

        dst = m_elements[--m_count];
        return true;
    }

    /** Removes AND discards the top item of the stack.  If the stack is empty
        then the method returns false and no value is returned.
     */
    bool pop() noexcept
    {
        if ( isEmpty() )
        {
            return false;
        }

        --m_count;
        return true;
    }

    /** Returns a copy of the item on the top of the Stack. The state of the
        stack is unchanged. If the stack is empty then the method returns false
        and no value is returned.
     */
    bool peekTop( ITEM& dst ) const noexcept
    {
        if ( isEmpty() )
        {
            return false;
        }

        dst = m_elements[m_count - 1];
        return true;
    }

    /** Returns a POINTER to the top item of the Stack. The state of the stack
        is unchanged. If the stack is empty then the method returns a null
        pointer. The pointer is valid until the next push or pop operation.

        NOTE: The application CAN modify the item in place on the Stack.  Use
              this power wisely!
     */
    ITEM* peekTop() noexcept
    {
    if ( isEmpty() )
    {
        return nullptr;
    }

    return &( m_elements[m_count - 1] );
}

public:
    /// This method returns true if the Stack is empty
    bool isEmpty( void ) const noexcept
    {
        return m_count == 0;
    }

    /// This method returns true if the Stack is full
    bool isFull( void ) const noexcept
    {
        return m_count == m_maxItems;
    }

    /// This method returns the current number of items in the Stack
    unsigned getNumItems( void ) const noexcept
    {
        return m_count;
    }

    /// This method returns the maximum number of items that can be stored in the Stack.
    unsigned getMaxItems( void ) const noexcept
    {
        return m_maxItems;
    }

public:
    /// Empties the Stack.  All references to the item(s) in the stack are lost.
    void clearTheStack() noexcept
    {
        m_count = 0;
    }


protected:
    /// Memory for the Elements
    ITEM* m_elements;

    /// Current number of items in the stack
    unsigned m_count;

    /// Maximum number of items in that the stack can hold
    unsigned m_maxItems;


protected:
    /// Prevent access to the copy constructor -->Containers can not be copied!
    Stack( const Stack& m ) = delete;

    /// Prevent access to the assignment operator -->Containers can not be copied!
    Stack& operator=( const Stack<ITEM>& m ) = delete;

    /// Prevent access to the move constructor -->Containers can not be implicitly moved!
    Stack( Stack&& m ) = delete;

    /// Prevent access to the move assignment operator -->Containers can not be implicitly moved!
    Stack<ITEM>& operator=( Stack<ITEM>&& m ) = delete;
};

}       // end namespaces
}
#endif  // end header latch
