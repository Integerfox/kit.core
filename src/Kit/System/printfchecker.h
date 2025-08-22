#ifndef KIT_SYSTEM_PRINTF_CHECKER_H_
#define KIT_SYSTEM_PRINTF_CHECKER_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file

    This file contains macro that enables the GCC (and Clang) compiler to
    validate printf flags with their corresponding arguments for home grown
    functions that have printf semantics (e.g. Tracing and Logging).
 */

 /** The KIT_SYSTEM_PRINTF_CHECKER() macro is used to enable printf-style
     argument checking - by the compiler - for custom logging functions. This
     macro should be placed above the function declaration to specify the
     positions of the format string and its corresponding arguments.
     \code

    'formatNum' - The 1-based position of the format string parameter. In the 
                  function below 'formatNum' is parameter 2:

                    void log(int level, const char* format, ...);
                
                  NOTE: For class methods, count the implicit 'this' pointer as parameter 1

    'vargsNum' -  The 1-based position of the first variadic argument. This is 
                  typically formatNum + 1. In the function below 'vargsNum' is 
                  parameter 3:

                    void log(int level, const char* format, ...), 
    
    Usage example:
        KIT_SYSTEM_PRINTF_CHECKER(2, 3);  
        void log(int level, const char* format, ...) 
      
        KIT_SYSTEM_PRINTF_CHECKER(3, 4); // The implicit 'this' argument is param 1
        void MyClass::log(int level, const char* format, ...) 

    \endcode
 */
#if defined( __GNUC__ )
#define KIT_SYSTEM_PRINTF_CHECKER( formatNum, vargsNum ) __attribute__( ( format( printf, formatNum, vargsNum ) ) )
#elif defined( __clang__ )
#define KIT_SYSTEM_PRINTF_CHECKER( formatNum, vargsNum ) __attribute__( ( __format__( __printf__, formatNum, vargsNum ) ) )
#else
#define KIT_SYSTEM_PRINTF_CHECKER( formatNum, vargsNum )
#endif


#endif  // end header latch
