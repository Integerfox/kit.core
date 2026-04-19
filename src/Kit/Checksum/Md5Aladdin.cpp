/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Md5Aladdin.h"
#include "Cpl/Text/format.h"

using namespace Cpl::Checksum;


///////////////////////////////////////
Md5Aladdin::Md5Aladdin() noexcept
{
	reset();
}

void Md5Aladdin::reset( void ) noexcept
{
	md5_init( &m_state );
}


///////////////////////////////////////
void Md5Aladdin::accumulate( const void* bytes, unsigned numbytes ) noexcept
{
	md5_append( &m_state, (const md5_byte_t *) bytes, numbytes );
}

Md5Aladdin::Digest_T& Md5Aladdin::finalize( Cpl::Text::String* convertToString, bool uppercase, bool append ) noexcept
{
	md5_finish( &m_state, m_result );

	if ( convertToString )
	{
		Cpl::Text::bufferToAsciiHex( m_result, eDIGEST_LEN, *convertToString, uppercase, append );
	}

	return m_result;
}



