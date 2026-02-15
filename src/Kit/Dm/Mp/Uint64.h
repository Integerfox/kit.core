#ifndef Cpl_Dm_Mp_Uint64_h_
#define Cpl_Dm_Mp_Uint64_h_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */


#include "Cpl/Dm/Mp/Numeric.h"

///
namespace Cpl {
///
namespace Dm {
///
namespace Mp {


/** This class provides a concrete implementation for a Point who's data is a
	uint64_t.

	The toJSON()/fromJSON format is:
		\code

		{ name:"<mpname>", type:"<mptypestring>", valid:true|false, seqnum:nnnn, locked:true|false, val:<numvalue> }

		where <numvalue> is integer numeric

		\endcode

 NOTE: All methods in this class ARE thread Safe unless explicitly
		  documented otherwise.
 */
class Uint64 : public Numeric<uint64_t, Uint64>
{
public:
	/** Constructor. Invalid MP.
	 */
	Uint64( Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName )
		: Numeric<uint64_t, Uint64>( myModelBase, symbolicName )
	{
	}

	/// Constructor. Valid MP.  Requires an initial value
	Uint64( Cpl::Dm::ModelDatabase& myModelBase, const char* symbolicName, uint64_t initialValue )
		: Numeric<uint64_t, Uint64>( myModelBase, symbolicName, initialValue )
	{
	}

public:
	/// Type safe subscriber
	typedef Cpl::Dm::Subscriber<Uint64> Observer;


public:
	///  See Cpl::Dm::ModelPoint.
	const char* getTypeAsText() const noexcept
	{
		return "Cpl::Dm::Mp::Uint64";
	}
};



};      // end namespaces
};
};
#endif  // end header latch
