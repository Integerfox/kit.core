#ifndef KIT_TEXT_FSTRING_BASE_H_
#define KIT_TEXT_FSTRING_BASE_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Text/StringBase.h"

///
namespace Kit {
///
namespace Text {


/** This mostly concrete class implements a "fixed storage" String Type.
    This class is built on the assumption that "behind-the-scenes"
    dynamic memory allocation is a bad thing (for a long list of reasons)!
    Therefore, NO memory allocation is done in this base class.  The sub-class
    is responsible for allocating ALL of storage used by the FStringBase.

    NOTE: All methods that "modify" the string  guarantee that the internal
          storage limit is not exceeded and the string is always NULL
          terminated.  The "incoming" data is silently truncated if the data
          size exceeds the storage limit.
 */

class FStringBase : public StringBase
{
protected:
    /// Maximum length of the internal string (not including the null terminator)
    int m_internalMaxlen;

    /// Empty string that is used to return a 'string' when my internal string is not valid
    static char m_noMemory[1];

protected:
    /// Constructor
    FStringBase( const char* string, char* internalString, int maxLen ) noexcept;

    /// Constructor
    FStringBase( char c, char* internalString, int maxLen ) noexcept;

    /// Constructor
    FStringBase( int num, char* internalString, int maxLen ) noexcept;

    /// Constructor
    FStringBase( unsigned num, char* internalString, int maxLen ) noexcept;

    /// Constructor
    FStringBase( long num, char* internalString, int maxLen ) noexcept;

    /// Constructor
    FStringBase( long long num, char* internalString, int maxLen ) noexcept;

    /// Constructor
    FStringBase( unsigned long num, char* internalString, int maxLen ) noexcept;

    /// Constructor
    FStringBase( unsigned long long num, char* internalString, int maxLen ) noexcept;


public:
    /// see Cpl::Text::String
    void copyIn( const char* string, int n ) noexcept override;

    /// see Cpl::Text::String
    void appendTo( const char* string, int n ) noexcept override;

    /// see Cpl::Text::String
    void insertAt( int insertOffset, const char* stringToInsert ) noexcept override;

    /// see Cpl::Text::String
    int maxLength() const noexcept override;

public:
    /// Make parent method visible
    using IString::operator=;

    /// Assignment
    IString& operator=( int num ) noexcept override;

    /// Assignment
    IString& operator=( unsigned num ) noexcept override;

    /// Assignment
    IString& operator=( long num ) noexcept override;

    /// Assignment
    IString& operator=( unsigned long num ) noexcept override;


public:
    /// Make parent method visible
    using IString::operator+=;

    /// Append
    IString& operator +=( int num ) noexcept override;

    /// Append
    IString& operator +=( unsigned num ) noexcept override;

    /// Append
    IString& operator +=( long num ) noexcept override;

    /// Append
    IString& operator +=( unsigned long num ) noexcept override;
};


};      // end namespaces
};
#endif  // end header latch
