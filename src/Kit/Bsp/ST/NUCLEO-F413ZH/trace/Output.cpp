/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */


#include "Kit/Bsp/ST/NUCLEO-F413ZH/console/Output.h"
#include "Kit/System/Trace.h"




////////////////////////////////////////////////////////////////////////////////
#else
// Prototype for io_putchar() function
#ifdef __GNUC__
/* With GCC, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
extern "C"                  int __io_putchar( int ch );
#define CALL_PUTCHAR(c)     __io_putchar(c)

#else
extern "C"                  int fputc( int ch, FILE * f );
#define CALL_PUTCHAR(c)     fputc(c,nullptr)


#endif /* __GNUC__ */



/// Anonymous namespace
namespace
{
/** This concrete class implement the CPL IO Output stream using GCC's low
    level __io_putchar() call (aka same output stream as printf).
 */
class IoPutChar : public Kit::Io::IOutput
{
public:
    /// Constructor
    IoPutChar(){}

    /// See Kit::Io::Output
    bool write( const void* buffer, int maxBytes, int& bytesWritten )
    {
        bytesWritten = maxBytes;
        const uint8_t* pt = (const uint8_t*) buffer;
        while ( maxBytes-- )
        {
            CALL_PUTCHAR( *pt++ );
        }
        return true;
    }

    /// See Kit::Io::Output
    void flush() {}
    void close() {}
    bool isEos() { return false; }

};

} // end anonymous namespace



////////////////////////////////////////////////////////////////////////////////
///
static IoPutChar fd_;

Kit::Io::IOutput* Kit::System::Trace::getDefaultOutputStream_( void ) noexcept
{
    return &fd_;
}
#endif  // end USE_PRINTF_IO