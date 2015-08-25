/******************************************************************************
 * Copyright AllSeen Alliance. All rights reserved.
 *
 *    Permission to use, copy, modify, and/or distribute this software for any
 *    purpose with or without fee is hereby granted, provided that the above
 *    copyright notice and this permission notice appear in all copies.
 *
 *    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *    WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *    MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *    ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *    WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *    ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 ******************************************************************************/

#ifndef LOGMODULESNAMES_H_
#define LOGMODULESNAMES_H_

/*
 * Common Macros
 */

namespace ajn {
namespace services {
namespace logModules {

static char const* const NOTIFICATION_MODULE_LOG_NAME = "Notification";
static char const* const CONTROLPANEL_MODULE_LOG_NAME = "ControlPanel";
static char const* const ONBOARDING_MODULE_LOG_NAME = "Onboarding";
static char const* const CONFIG_MODULE_LOG_NAME = "Config";
static char const* const SERVICES_COMMON_MODULE_LOG_NAME = "services_common";
static char const* const TIME_MODULE_LOG_NAME = "Time";
static const uint32_t ALL_LOG_LEVELS = 15;
} //namespace logModules
} //namespace services
} //namespace ajn

#endif /* LOGMODULESNAMES_H_ */
