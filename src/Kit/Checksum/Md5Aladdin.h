#ifndef Cpl_Checksum_Md5Aladdin_h_
#define Cpl_Checksum_Md5Aladdin_h_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Cpl/Checksum/ApiMd5.h"
#include "Cpl/Checksum/md5_aladdin_.h"



///
namespace Cpl {
///
namespace Checksum {


/** This class provides an implementation for the MD5 Hash interface
	that is wrapper to the third party MD5 Library code developed
	by  L. Peter Deutsch, ghost@aladdin.com
 */
class Md5Aladdin : public ApiMd5
{
private:
	/// Hash state
	md5_state_t m_state;

	/// Hask result
	Digest_T    m_result;

public:
	/// Constructor
	Md5Aladdin() noexcept;


public:
	/// See Cpl::Checksum::ApiMd5
	void reset( void ) noexcept;

	/// See Cpl::Checksum::ApiMd5
	void accumulate( const void* bytes, unsigned numbytes=1 ) noexcept;

	/// See Cpl::Checksum::ApiMd5
	Digest_T& finalize( Cpl::Text::String* convertToString=0, bool uppercase=true, bool append=false ) noexcept;
};

};      // end namespaces
};
#endif  // end header latch

