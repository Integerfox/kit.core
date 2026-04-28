# Kit::Driver

@brief namespace description for Kit::Driver
@namespace Kit::Driver @brief

The Driver namespace contains various types of drivers, driver abstractions,
and concrete drivers.

NOTE: The design model for the drivers is to use pure virtual interfaces as their
      public interface to decouple the client code from any specific implementation
      or hardware dependence.  This approach is functionally equivalent (in memory
      footprint and CPU cycle times) to using a C struct with function pointers.
