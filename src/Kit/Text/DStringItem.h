#ifndef KIT_TEXT_DSTRING_ITEM_H_
#define KIT_TEXT_DSTRING_ITEM_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Text/StringItem.h"
#include "Kit/Text/DString.h"


///
namespace Kit {
///
namespace Text {



/** This concrete class implements the StringItem class for a 'String' using
	a 'DString' as the internal storage for String instance.
 */
class DStringItem : public StringItem
{
protected:
	/// Underlying String
	DString m_internalString;

public:
	/// Constructor
	DStringItem( const IString& string, int initialSize=0, int blocksize=OPTION_KIT_TEXT_DSTRING_ALLOC_BLOCK_SIZE ) :StringItem( m_internalString ), m_internalString( string, initialSize, blocksize ) {}

	/// Constructor
	DStringItem( const char* string="", int initialSize=0, int blocksize=OPTION_KIT_TEXT_DSTRING_ALLOC_BLOCK_SIZE ) :StringItem( m_internalString ), m_internalString( string, initialSize, blocksize ) {}


private:
	/// Prevent access to the copy constructor -->Container Items should not be copied!
	DStringItem( const DStringItem& m );

	/// Prevent access to the assignment operator -->Container Items should not be copied!
	const DStringItem& operator=( const DStringItem& m );

};


};      // end namespaces
};
#endif  // end header latch
