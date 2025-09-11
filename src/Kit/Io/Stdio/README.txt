/** @namespace Kit::Io::Stdio
The Stdio namespace provides concrete classes for the C Library's 'stdin', 
'stdout', and 'stderr' input/output streams.  The class definitions (i.e the
header files) are platform independent so the Application can use them directly
without incurring any platform specific dependencies.  However, It is the 
Application's responsibility to build & link the appropriate platform specific
implementation of these classes. 

Note: The "_ansi/' implementation of the Stdio classes uses the C library's 
      'FILE*' methods (fwrite, fread, etc.).

 */  


  