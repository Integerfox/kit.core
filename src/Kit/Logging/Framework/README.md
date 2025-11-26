# Kit::Logging::Framework
@brief Namespace description for Kit::Logging::Framework
@namespace Kit::Logging::Framework @brief

The Framework namespace provides the non-domain specific framework for Logging
events.  Note: This namespace does NOT include, NOT responsible for persistently
storing log entries.

## Logging vs Tracing

While Logging and Tracing have some surface similarity - there are significantly
different in purpose.

- **Tracing** is *printf* debugging (to output stream/UART) that can be compiled
  out for production builds. While there is some structure (e.g. trace section
  string used for selective filtering run-time output) it essentially volatile
  and subject to change per the needs of develop on any given day.
  - This means the TRACE statements are not *reliable* when it comes building
    test cases
  - TRACE statements can easily contain intellectual property that should not be
    shared with customers or even field/tech support.
  - Typically no mechanism (e.g. the KIT library does not) for persistently
    the trace statements.
    - This means direct "console" access is required to get access to the TRACE
      statements.
- **Logging** provides persistently stored entries that can be retrieved later
  for troubleshooting and audit purposes.  The KIT logging framework adds
  the following features/semantics:
  - Each log entry has unique identifier that is independent of the information
    text.  This means the entries are *reliable* with respect creating tests.
    For example if the information text changes because and additional variable
    was added - the existing test cases do not break because they are keyed
    off the entry ID - not the contents of the info text.
    - Because all log entries have a unique identifier - it simplifies creating
      documentation for consumption by field/tech support, end-customer, and
      regulatory requirements.
  - It is *easier* (than tracing) to enforce not leaking any intellectual
    property in its information text.  Note: *easy* is relative term ;-).

## The KIT Framework provides the following features

- The interface is thread safe.

- When an entry is logged it is timestamped.  The application is required to
  provide the implementation of the time-stamp interface.  

- Each entry 'logged' has a classification (e.g. CRITICAL, WARNING, etc.), a
  unique entry identifier, and free formatted information text

- A Log entry contains the following:
  - Time-stamp
  - Classification ID
  - Message ID.  This is tuple of 3 separate fields. Message ID are unique per Domain
    - Package ID
    - SubSystem ID
    - Message ID
  - Free formatted Information Text.  Has printf semantics.
- The application is responsible defining the set of Classification IDs as well
  as providing the application specific mappings for Package IDs.

- Supports run time filtering - with respect to what gets written to persistently
  store - by Classification and Package IDs.
  - Log entries that are excluded by the filter - are silently discarded.

- All Log entries are echoed to the KIT trace engine (when trace is enabled). The
  Classification ID text is used as the trace 'section'

- The framework supports caching log entries **before** the application's media
  is available for writing.

- The application is responsible for storing log entries to a (presumably)
  persistent storage media.  The logging framework dispatches each log entry to
  a thread-safe FIFO.  It is the application's responsibility to consume the
  entries from the FIFO and record them in persistent storage.  The FIFO supports
  change notifications via a model point - see `Kit::Containers::RingBufferMP`.
  - The log entry data structure inherits from the `Kit::Persistent::Payload`
    interface.  The Payload interface defines two methods to serialize and
    deserialize the log entry data structure.  While this introduces a dependency
    on the Kit::Persistent namespace - the Payload interface definition has no
    other dependencies, i.e. the application is NOT required to use the
    Kit::Persistent namespace for storing the log entry to persistent media.

- The log entry FIFO can overflow if the application is not storing log entries
  faster than the application is generating entries. When this happens, the
  framework discards log entries until space frees up in the queue.  The framework
  tracks how many log entries where discarded and generates a special log entry
  with the details once there is space in the queue.

## Design Goals and Constraints

The KIT framework attempts to solve/address the following requirements.

1. The business needs of an Application dedicates what Classification options it
   requires.
2. It is the Application that has knowledge of what *packages* are
   used to implement it.  Where packages can in-house code-bases and
   namespaces, libraries, etc. as well as third-party source code.
   1. It is assumed these packages have no (and should **not** have)
      any dependencies on any specific Application.
   2. Individual Packages define their own Message IDs and logging methods.
      1. However, the shall be no code edits to a Package with respect to the
         Logging interface when a Package is used/consumed by an Application.
   3. When there is more than one packages - there shall not be any Package ID
      collisions, OR Package ID collisions are prevented by only editing
      Application specific files (not any Package files).
   4. Packages are not required have a 1-to-1 mapping of their Classification ID
      to the Application IDs.  Resolving differences in the Classification **must**
      be done in Application code (no edits to Package files).

NOTE: For implementation purposes the Application specific logging methods and
      identifiers are treated as a package.  This means there is always at least
      one logging package.  

### Logging Packages: A Typical Application

The following is a typical logging *package configuration* when using the KIT library
on a project:

- There are 3 logging packages:
  - The application.  This encompasses all Application specific code/namespaces
  - In house code base or namespace for *reusable* domain specific code.
    - With the assumption that this package use the KIT logging framework.
  - The KIT Library.
