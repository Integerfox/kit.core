#ifndef KIT_DM_OBSERVER_CALLBACK_H_
#define KIT_DM_OBSERVER_CALLBACK_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Dm/Observer.h"

///
namespace Kit {
///
namespace Dm {

/** This template class implements a Observer that uses a callback function
    (or similar callable) for change notifications. This approach avoids the
    need for inheritance when an associated class needs to subscribe to multiple
    Model Points.

    Usage:
        class MyClass 
        {
            ObserverCallback<Kit::Dm::Mp::Uint32> m_sub1;

            void onM1(Kit::Dm::Mp::Uint32& mp, Kit::Dm::IObserver& sub) { ... }

            MyClass(Kit::EventQueue::IChangeNotification& eventQueServer)
            : m_sub1(eventQueServer) 
            {
                m_sub1.setCallback<MyClass, &MyClass::onM1>(this);
            }
        };

    Template Arguments:
        MP      - The concrete Model Point Type
 */
template <class MP>
class ObserverCallback : public Observer<MP>
{
public:
    /** Define the Callback function signature.
     */
    typedef void ( *CallbackFunc_T )( void* context, MP& modelPointThatChanged, IObserver& clientObserver );

protected:
    /// Context pointer (passed to the callback function)
    void* m_context;

    /// The callback function
    CallbackFunc_T m_callback;

public:
    /// Constructor
    ObserverCallback( Kit::EventQueue::IChangeNotification& myEventLoop )
        : Observer<MP>( myEventLoop )
        , m_context( 0 )
        , m_callback( 0 )
    {
    }

    /// Default Constructor
    ObserverCallback()
        : Observer<MP>()
        , m_context( 0 )
        , m_callback( 0 )
    {
    }

public:
    /// Type safe change notification.  See Kit::Dm::IObserver
    void modelPointChanged( MP& modelPointThatChanged, IObserver& clientObserver ) noexcept override
    {
        if ( m_callback )
        {
            m_callback( m_context, modelPointThatChanged, clientObserver );
        }
    }

public:
    /** Sets the callback to a Member Function of the 'CONTEXT' class type.
        This provides a type-safe wrapper that converts the Context pointer
        to the appropriate class pointer.
     */
    template <class CONTEXT, void ( CONTEXT::*FUNC )( MP&, IObserver& )>
    void setCallback( CONTEXT* context ) noexcept
    {
        m_context  = context;
        m_callback = &entryPoint<CONTEXT, FUNC>;
    }

    /** Sets the callback to a generic C-style function pointer.
     */
    void setCallback( CallbackFunc_T callbackFunction, void* context = 0 ) noexcept
    {
        m_context  = context;
        m_callback = callbackFunction;
    }

protected:
    /// Internal helper method to adapt Member Function to C-style callback
    template <class CONTEXT, void ( CONTEXT::*FUNC )( MP&, IObserver& )>
    static void entryPoint( void* context, MP& modelPointThatChanged, IObserver& clientObserver )
    {
        CONTEXT* obj = static_cast<CONTEXT*>( context );
        ( obj->*FUNC )( modelPointThatChanged, clientObserver );
    }
};


}       // end namespaces
}
#endif  // end header latch
