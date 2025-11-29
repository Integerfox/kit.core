#ifndef KIT_TEXT_BSTRING_H_
#define KIT_TEXT_BSTRING_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Text/FStringBase.h"

///
namespace Kit {
namespace Text {


/** This class constructs a IString from client supplied memory. The memory
    size cannot be changed once instantiated. Any 'write' operations that exceeds
    the size of provided memory (with the null terminator taken into account),
    the results are silently truncated and the string is ALWAYS left in null
    terminated state.

    See base class - IString - for a complete listing/description of the class's
    methods.

    NOTE: When creating an instance of this class, the caller must provide
          the memory for the string storage and the total size of the memory.
          The maximum string length that can be stored will be (totalRawMemorySize - 1)
 */

class BString : public FStringBase
{
public:
    /// Constructor
    BString( char* rawMemory, int totalRawMemorySize, const BString& initialValue ) noexcept
        : FStringBase( initialValue.getString(), rawMemory, totalRawMemorySize-1 ) {}

    /// Constructor
    BString( char* rawMemory, int totalRawMemorySize, const IString& string ) noexcept
        : FStringBase( string.getString(), rawMemory, totalRawMemorySize-1 ) {}
    /// Constructor
    BString( char* rawMemory, int totalRawMemorySize, const char* string = "" ) noexcept
        : FStringBase( string, rawMemory, totalRawMemorySize-1 ) {}

    /// Constructor
    BString( char* rawMemory, int totalRawMemorySize, char c ) noexcept
        : FStringBase( c, rawMemory, totalRawMemorySize-1 ) {}
    /// Constructor
    BString( char* rawMemory, int totalRawMemorySize, int num ) noexcept
        : FStringBase( num, rawMemory, totalRawMemorySize-1 ) {}

    /// Constructor
    BString( char* rawMemory, int totalRawMemorySize, unsigned num ) noexcept
        : FStringBase( num, rawMemory, totalRawMemorySize-1 ) {}
    /// Constructor
    BString( char* rawMemory, int totalRawMemorySize, long num ) noexcept
        : FStringBase( num, rawMemory, totalRawMemorySize-1 ) {}

    /// Constructor
    BString( char* rawMemory, int totalRawMemorySize, long long num ) noexcept
        : FStringBase( num, rawMemory, totalRawMemorySize-1 ) {}
    /// Constructor
    BString( char* rawMemory, int totalRawMemorySize, unsigned long num ) noexcept
        : FStringBase( num, rawMemory, totalRawMemorySize-1 ) {}

    /// Constructor
    BString( char* rawMemory, int totalRawMemorySize, unsigned long long num ) noexcept
        : FStringBase( num, rawMemory, totalRawMemorySize-1 ) {}
public:
    // Add this using declaration to make base class operators visible
    using FStringBase::operator=;
};


}  // end namespaces
}
#endif  // end header latch
