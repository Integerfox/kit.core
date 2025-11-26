@brief namespace description for Kit::System::Watchdog

@namespace Kit::System::Watchdog @brief

This namespace provides the infrastructure for incorporating a hardware
watchdog into an application where individual threads can *watched* independently
of each.

Features supported:

- Monitor any event base thread (e.g. Mailbox Servers and EventLoops).

- The ability to monitor a 'raw thread' (i.e. any thread that does **not** use
  the KIT library's Event Loop framework)

- The Watchdog framework supports being 'compiled-out', i.e. the public
  facing API calls are wrapped in macros that optionally compile to 'nops'
  when building.  To enable the framework at compile time - the application
  must define: USE_KIT_SYSTEM_WATCHDOG

- HAL interface defined to decouple the framework from the target
  platform.

- Once the Hardware Watchdog has been started it is not be disabled.

- The framework requires ONE event based thread (i.e. executes the
  `Kit::System::EventLoop` code) that the 'Watchdog Supervisor' runs in.  This
  thread does NOT have to be a dedicated Watchdog thread, i.e. the watchdog Supervisor can run in any application event thread.

- Individual application threads (both event and raw threads) can be monitored,
  with each thread having its own 'timeout period'.  The application selects which
  thread(s) are or are not monitored.

- The Watched Event threads have a software timer that is used to perform a
  periodic health check themselves.  If the health check fails, the Watchdog is
  immediately tripped. The health check interval time must be less than the
  threads' Watchdog timeout interval. The health check is customizable on a per
  thread basis.

NOTE: **Not** all threads should or can be 'watched-dogged'.  For example the
      TShell typically runs in a 'raw thread' that spends 99.9% of its time blocked on reading a serial stream.  For this use case - the thread should **never** be registered with "entity" that is responsible for monitoring individual threads.
