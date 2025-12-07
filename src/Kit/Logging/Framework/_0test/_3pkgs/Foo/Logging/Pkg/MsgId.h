#ifndef FOO_LOGGING_PKG_MSGID_H_
#define FOO_LOGGING_PKG_MSGID_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "kit_config.h"
#include "Kit/Type/BetterEnum.h"
#include <stdint.h>

///
namespace Foo {
///
namespace Logging {
///
namespace Pkg {

/** This enumeration defines Logging message codes for the Foo UI sub-system

        Note: Each symbol's length must be less than or equal to 32 characters
           123456789 123456789 123456789 12

    @param UiMsgId                           Enum

    @param SCREEN_LOADED                     UI screen loaded
    @param BUTTON_CLICKED                    Button was clicked
    @param FORM_VALIDATED                    Form validation succeeded
    @param VALIDATION_ERROR                  Form validation failed
 */
// clang-format off
BETTER_ENUM( UiMsgId, uint8_t
             , SCREEN_LOADED
             , BUTTON_CLICKED
             , FORM_VALIDATED
             , VALIDATION_ERROR
);
// clang-format on

/** This enumeration defines Logging message codes for the Foo Database sub-system

        Note: Each symbol's length must be less than or equal to 32 characters
           123456789 123456789 123456789 12

    @param DatabaseMsgId                     Enum

    @param QUERY_SUCCESS                     Database query successful
    @param QUERY_FAILED                      Database query failed
    @param CONNECTION_LOST                   Database connection lost
    @param TRANSACTION_COMPLETE              Transaction completed
 */
// clang-format off
BETTER_ENUM( DatabaseMsgId, uint8_t
             , QUERY_SUCCESS
             , QUERY_FAILED
             , CONNECTION_LOST
             , TRANSACTION_COMPLETE
);
// clang-format on

/** This enumeration defines Logging message codes for the Foo API sub-system

        Note: Each symbol's length must be less than or equal to 32 characters
           123456789 123456789 123456789 12

    @param ApiMsgId                          Enum

    @param REQUEST_RECEIVED                  API request received
    @param RESPONSE_SENT                     API response sent
    @param AUTH_FAILED                       Authentication failed
    @param RATE_LIMIT_EXCEEDED               Rate limit exceeded
 */
// clang-format off
BETTER_ENUM( ApiMsgId, uint8_t
             , REQUEST_RECEIVED
             , RESPONSE_SENT
             , AUTH_FAILED
             , RATE_LIMIT_EXCEEDED
);
// clang-format on

}  // end namespaces
}
}
#endif  // end header latch
