# Kit::Logging::Framework::Mocked4Test
@brief Namespace description for Kit::Logging::Framework::Mocked4Test
@namespace Kit::Logging::Framework::Mocked4Test @brief

The Mocked4Test namespace provides a ready-to-use, self-contained instance of
the Logging framework for unit testing purposes.  It exists so that modules that
generate log entries can be unit tested without each test having to supply its
own `IApplication` implementation and log entry FIFO.

The `KitOnly` class provides:

- An `IApplication` implementation that supports a single logging package -->the
  KIT library (see the Kit::Logging::Pkg namespace).
- The log entry FIFO, along with the Data Model instance required by
  `Kit::Container::RingBufferMP`.
- Initialization of the Logging framework (performed by its constructor).
- WhiteBox accessors - e.g. the queued entry count, the overflow status, the
  number of `vlogf()` calls - so that tests can inspect the framework's internal
  state.

NOTE: `KitOnly` only implements the required (i.e. pure virtual) portions of the
`IApplication` interface.  Optional, application defined concepts - such as the
per-entry Application State - are intentionally NOT implemented because the KIT
library has no knowledge of them.  Tests that need to exercise those features
supply their own `IApplication` instance.

CAUTION: This namespace is for unit testing ONLY.  It must NOT be used in
production code.
