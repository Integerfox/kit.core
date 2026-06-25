#ifndef KIT_TSHELL_NOSECURITY_H_
#define KIT_TSHELL_NOSECURITY_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/TShell/ISecurity.h"

///
namespace Kit {
///
namespace TShell {

/** This concrete security policy allows all commands and does not suppress
    shell output.
 */
class NoSecurity : public ISecurity
{
public:
    /// See Kit::TShell::ISecurity
    bool isAuthorized( Permissions_T,
                       const char*,
                       bool = false ) noexcept override
    {
        return true;
    }

    /// See Kit::TShell::ISecurity
    bool isSilent() noexcept override
    {
        return false;
    }
};


}  // end namespaces
}
#endif  // end header latch