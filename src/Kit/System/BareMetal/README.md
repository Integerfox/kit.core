# Kit::System::BareMetal
@brief namespace description for Kit::System::BareMetal
@namespace Kit::System::BareMetal @brief

Concrete implementation of the platform specific classes/features of the
Kit::System namespace interfaces for a Baremetal/Singled-threaded system.

Platform Dependent Behaviors:

- **ElapsedTime**.  The platform is required to provide the number of milliseconds
                    since power-up.

- **EventLoop**.  Fully functional.  This includes EventFlags and Software Timers.

- **GlobalLock**.  The GlobalLock is mapped the BSP's `Bsp_disable_irqs()` and
                   `Bsp_enable_irqs()` methods.

- **IRunnable**. The Application is required to have a single instance of the
                 `IRunnable` class.  This instance's `entry()` method is called
                 when the `enableScheduling()` method is called, i.e. the
                 IRunnable instance is effectively the application.

- **Mutex**.  The mutex interface is effectively all NOPs

- **Semaphore**.  Fully functional.  However since there is only 'one thread' - if
                  a semaphore's count is at zero and the wait() method is called the
                  "main thread" will block/busy-wait forever unless the semaphore's
                  su_signal() method is called from an ISR context.

- **sleep()**.  The `sleep()` method's blocking-wait semantics are implemented as
                a busy-wait delay.  The platform is required to provide a busy-wait
                function with millisecond resolution.

- **suspendScheduling()** and **resumeScheduling().**  The methods have no meaning/purpose
               for a single threaded system.  These functions do nothing (i.e.
               are NOPs).

- **Thread**.  The threading class is stubbed out so that any call to the
               interface are benign.  The Thread interface will "report" a
               single thread.  The application is required to create **one and
               only one** Thread object prior to calling `enableScheduling()`.

  - The application uses the `Kit::System::Thread::create()` method
    to create the one and only one thread object.
  - The `enableScheduling()` method returns if/when the Application's
    `IRunnable` instance exits its `entry()` method

- **Tls**.  Fully functional.

- **WatchDog**.  Fully functional.  The Application is required to make its
                 one-and-only-one `IRunnable` instance the Watchdog Supervisor
                 thread.
