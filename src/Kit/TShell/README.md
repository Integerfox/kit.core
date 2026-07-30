# Kit::TShell 
@brief namespace description for Kit::TShell
@namespace Kit::TShell @brief

The TShell namespace provides a text based shell that can be used interact with
an Application.  What functionality the TShell provides is application specific.  
One example is a Debug Shell and/or Maintenance Port that provides a developer
white-box access to the running the Application.

Note: The TShell is ONLY a framework/pattern for a text based shell - the
      application is responsible for connecting it to the application and
      filling in the 'missing bits'.
